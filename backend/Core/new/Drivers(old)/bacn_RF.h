#ifndef BACN_RF_H
#define BACN_RF_H

#include <libhackrf/hackrf.h>

#define DEFAULT_SAMPLE_RATE_HZ (20000000)

#define DEFAULT_CENTRAL_FREQ_HZ (10000000)

#define DEFAULT_SAMPLES_TO_XFER_MAX (20000000)

#define FD_BUFFER_SIZE (8 * 1024)

void stop_main_loop(void);
int rx_callback(hackrf_transfer* transfer);
void sigint_callback_handler(int signum);
void sigalrm_callback_handler();
int getSamples(int64_t lo_freq, int64_t hi_freq);

#endif // BACN_RF_H
