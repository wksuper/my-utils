#ifndef __EPCM_H__
#define __EPCM_H__

#include <tinyalsa/asoundlib.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct epcm;
struct epcm_config {
        unsigned int ram_millisec;
};

struct epcm *epcm_open(unsigned int card,
                      unsigned int device,
                      unsigned int flags,
                      const struct pcm_config *config,
                      const struct epcm_config *econfig);

struct pcm* epcm_base(struct epcm *epcm);

int epcm_read(struct epcm *epcm, void *data, unsigned int count);

int epcm_close(struct epcm *epcm);

#if defined(__cplusplus)
}
#endif

#endif
