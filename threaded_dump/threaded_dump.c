#include "threaded_dump.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <KLogging.h>

struct threaded_dump {
	char *ram;
	size_t ram_size;
	size_t r_pos; /* r_pos would be blocked by w_pos */
	size_t w_pos; /* w_pos won't be blocked */
	pthread_mutex_t mutex_for_w_pos;
	pthread_cond_t cond;
	FILE *file;
	size_t file_written_bytes;
	pthread_t tid;
	bool stop;
};

static size_t td_get_read_available(const struct threaded_dump *td)
{
	return (td->w_pos >= td->r_pos)
	       ? (td->w_pos - td->r_pos)
	       : (td->ram_size - td->r_pos + td->w_pos);
}

static void td_dump_to_file(struct threaded_dump *td)
{
	size_t read_available, left;

	KLOGV("+%s()+\n", __FUNCTION__);
	pthread_mutex_lock(&td->mutex_for_w_pos);
	while ((read_available = td_get_read_available(td)) == 0 && !td->stop) {
		pthread_cond_wait(&td->cond, &td->mutex_for_w_pos);
	}
	pthread_mutex_unlock(&td->mutex_for_w_pos);

	left = read_available;
	while (left) {
		size_t bytes_to_end = td->ram_size - td->r_pos;
		if (left < bytes_to_end) {
			fwrite(td->ram + td->r_pos, 1, left, td->file);
			td->r_pos += left;
			left = 0;
		} else {
			fwrite(td->ram + td->r_pos, 1, bytes_to_end, td->file);
			td->r_pos = 0;
			left -= bytes_to_end;
		}
	}
	fflush(td->file);
	td->file_written_bytes += read_available;
	KLOGV("-%s()-\n", __FUNCTION__);
}

static void *ram_to_file_dump_thread(void *data)
{
	struct threaded_dump *td = (struct threaded_dump *)data;

	while (!td->stop)
		td_dump_to_file(td);

	return NULL;
}

struct threaded_dump *td_create(const struct td_config *config, const char *filename)
{
	KLOGD("+%s()+ ram_size=%zu\n", __FUNCTION__, config->ram_size);

	struct threaded_dump *td = (struct threaded_dump *)malloc(sizeof(struct threaded_dump));
	if (!td) {
		KLOGE("Failed to alloc memory\n");
		goto err;
	}

	memset(td, 0, sizeof(struct threaded_dump));

	td->file = fopen(filename, "wb");
	if (!td->file) {
		KLOGE("Failed open %s\n", filename);
		goto err;
	}

	td->file_written_bytes = 0;

	if (config->ram_size) {
		td->ram = (char *)malloc(config->ram_size);
		if (!td->ram) {
			KLOGE("Failed to alloc memory\n");
			goto err;
		}
		td->ram_size = config->ram_size;
		td->r_pos = td->w_pos = 0;
		td->stop = false;

		pthread_mutex_init(&td->mutex_for_w_pos, (const pthread_mutexattr_t *) NULL);
		pthread_cond_init(&td->cond, (const pthread_condattr_t *) NULL);

		if (pthread_create(&td->tid, NULL, ram_to_file_dump_thread, td)) {
			KLOGE("Failed to create the ram-to-file dump thread\n");
			goto err;
		}
	} else {
		/* bypass mode */
	}

	KLOGD("-%s()- success\n", __FUNCTION__);
	return td;

err:
	td_destroy(td);
	KLOGD("-%s()- failed\n", __FUNCTION__);
	return NULL;
}

void td_destroy(struct threaded_dump *td)
{
	KLOGD("+%s()+\n", __FUNCTION__);
	if (td) {
		if (td->ram_size) {
			td->stop = true;
			pthread_mutex_lock(&td->mutex_for_w_pos);
			pthread_cond_signal(&td->cond);
			pthread_mutex_unlock(&td->mutex_for_w_pos);
			pthread_join(td->tid, NULL);
			td_dump_to_file(td); // flush the remaining data in the ram

			pthread_cond_destroy(&td->cond);
			pthread_mutex_destroy(&td->mutex_for_w_pos);
			free(td->ram);
		} else {
			/* bypass mode */
		}

		fclose(td->file);
		free(td);
	}
	KLOGD("-%s()-\n", __FUNCTION__);
}

void td_write(struct threaded_dump *td, const char *buf, size_t size)
{
	KLOGV("+%s()+ size=%zu\n", __FUNCTION__, size);
	if (td->ram_size) {
		size_t w = td->w_pos;

		while (size) {
			size_t bytes_to_end = td->ram_size - w;
			if (size < bytes_to_end) {
				memcpy(td->ram + w, buf, size);
				w += size;
				size = 0;
			} else {
				memcpy(td->ram + w, buf, bytes_to_end);
				buf += bytes_to_end;
				w = 0;
				size -= bytes_to_end;
			}
		}

		pthread_mutex_lock(&td->mutex_for_w_pos);
		td->w_pos = w;
		pthread_cond_signal(&td->cond);
		pthread_mutex_unlock(&td->mutex_for_w_pos);
	} else {
		/* bypass mode */
		fwrite(buf, 1, size, td->file);
		fflush(td->file);
		td->file_written_bytes += size;
	}
	KLOGV("-%s()-\n", __FUNCTION__);
}

size_t td_get_file_written_bytes(const struct threaded_dump *td)
{
	return td->file_written_bytes;
}
