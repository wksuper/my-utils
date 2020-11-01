#include "easoundlib.h"
#include <klogging/klogging.h>
#include <string.h>

int main(int argc, char *argv[])
{
        printf("version: %s\n", VERSION);
        KLOG_SET_OPTIONS(KLOGGING_TO_STDOUT);
        KLOG_SET_LEVEL(KLOGGING_LEVEL_VERBOSE);

        struct epcm *epcm;
        struct pcm *pcm;

        struct epcm_config econfig;
        memset(&econfig, 0, sizeof(econfig));
        econfig.ram_millisec = 10/*seconds*/* 1000;

	struct pcm_config config;
	memset(&config, 0, sizeof(config));
	config.channels = 1;
	config.rate = 48000;
	config.period_size = 1024;
	config.period_count = 4;
	config.format = PCM_FORMAT_S16_LE;
	config.start_threshold = 0;
	config.stop_threshold = 0;
	config.silence_threshold = 0;

        FILE *f = fopen("cap.pcm", "wb");

        epcm = epcm_open(0, 0, PCM_IN, &config, &econfig);
	if (!epcm) {
                KLOGE("Unable to epcm_open()\n");
                return -1;
        }
        pcm = epcm_base(epcm);
        if (!pcm || !pcm_is_ready(pcm)) {
                KLOGE("Unable to open PCM device (%s)\n",
                        pcm_get_error(pcm));
                return 0;
        }

        size_t size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
        char *buf = malloc(size);

        while (1) {
                if (epcm_read(epcm, buf, size) == 0) {
                        KLOGV("Read %u bytes\n", size);
                        if (f)
                                fwrite(buf, size, 1, f);
                } else {
                        KLOGE("Error to read %u bytes\n", size);
                }
        }

        epcm_close(epcm);

        free(buf);
        fclose(f);
        return 0;
}
