#ifndef __THREADED_DUMP_H__
#define __THREADED_DUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>

struct threaded_dump;

struct td_config {
	/* ram_size should be >= 0
	 * ram_size == 0 means bypass threaded dump, frwite directly
	 * ram_size > 0 means actual ram size(in bytes) for threaded dump
	 */
	size_t ram_size;
};

struct threaded_dump *td_create(const struct td_config *config, const char *filename);
void td_write(struct threaded_dump *td, const char *buf, size_t size);
size_t td_get_file_written_bytes(const struct threaded_dump *td);
void td_destroy(struct threaded_dump *td);

#ifdef __cplusplus
}
#endif

#endif
