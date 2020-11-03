#include "easoundlib.h"
#include <klogging/klogging.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

static int g_quit;

static void sigint_handler(int sig)
{
	if (sig == SIGINT) {
		g_quit = 1;
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	printf("version: %s\n", VERSION);
	KLOG_SET_OPTIONS(KLOGGING_TO_STDOUT);
	KLOG_SET_LEVEL(KLOGGING_LEVEL_VERBOSE);

	struct epcm *epcm = NULL;
	struct pcm *pcm = NULL;

	struct epcm_config econfig;
	memset(&econfig, 0, sizeof(econfig));
	econfig.ram_millisec = 10/*seconds*/ * 1000;

	struct pcm_config config;
	memset(&config, 0, sizeof(config));
	config.channels = 2;
	config.rate = 48000;
	config.period_size = 1024;
	config.period_count = 4;
	config.format = PCM_FORMAT_S16_LE;
	config.start_threshold = 0;
	config.stop_threshold = 0;
	config.silence_threshold = 0;

	FILE *f = fopen("cap.pcm", "wb");

	if (!f) {
		KLOGE("Unable to open cap.pcm\n");
		ret = -1;
		goto error;
	}

	signal(SIGINT, sigint_handler);

	epcm = epcm_open(0, 0, PCM_IN, &config, &econfig);
	if (!epcm) {
		KLOGE("Unable to epcm_open()\n");
		ret = -1;
		goto error;
	}
	pcm = epcm_base(epcm);

	size_t size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
	char *buf = malloc(size);
	if (!buf) {
		KLOGE("Failed to malloc(%d bytes)\n", size);
		ret = -1;
		goto error;
	}

	while (!g_quit) {
		if (epcm_read(epcm, buf, size) == 0) {
			fwrite(buf, size, 1, f);
		} else {
			KLOGE("Error to read %u bytes\n", size);
		}
	}

error:
	epcm_close(epcm);
	free(buf);
	fclose(f);
	return ret;
}
