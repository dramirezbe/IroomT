#include <libhackrf/hackrf.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include "bacn_RF.h"


static volatile bool do_exit = false;
FILE* file = NULL;

volatile uint32_t byte_count = 0;

uint64_t stream_size = 0;
uint32_t stream_head = 0;
uint32_t stream_tail = 0;
uint32_t stream_drop = 0;
uint8_t* stream_buf = NULL;

bool limit_num_samples = true;
size_t bytes_to_xfer = 0;
int64_t lo_freq = 0;
int64_t hi_freq = 0;

static hackrf_device* device = NULL;

void stop_main_loop(void)
{
	do_exit = true;
	kill(getpid(), SIGALRM);
}

int rx_callback(hackrf_transfer* transfer)
{
	size_t bytes_to_write;
	size_t bytes_written;

	if (file == NULL) {
		stop_main_loop();
		return -1;
	}

	/* Determina cuántos bytes escribir */
	bytes_to_write = transfer->valid_length;

	/* Actualiza el conteo de bytes */
	byte_count += transfer->valid_length;
	
	if (limit_num_samples) {
		if (bytes_to_write >= bytes_to_xfer) {
			bytes_to_write = bytes_to_xfer;
		}
		bytes_to_xfer -= bytes_to_write;
	}

	/* Escribe los datos directamente en el archivo si no hay búfer de transmisión */
	if (stream_size == 0) {
		bytes_written = fwrite(transfer->buffer, 1, bytes_to_write, file);
		if ((bytes_written != bytes_to_write) ||
		    (limit_num_samples && (bytes_to_xfer == 0))) {
			stop_main_loop();
			fprintf(stderr, "[driver] Total Bytes: %u\n",byte_count);
			return -1;
		} else {
			return 0;
		}
	}

	if ((stream_size - 1 + stream_head - stream_tail) % stream_size <
	    bytes_to_write) {
		stream_drop++;
	} else {
		if (stream_tail + bytes_to_write <= stream_size) {
			memcpy(stream_buf + stream_tail,
			       transfer->buffer,
			       bytes_to_write);
		} else {
			memcpy(stream_buf + stream_tail,
			       transfer->buffer,
			       (stream_size - stream_tail));
			memcpy(stream_buf,
			       transfer->buffer + (stream_size - stream_tail),
			       bytes_to_write - (stream_size - stream_tail));
		};
		__atomic_store_n(
			&stream_tail,
			(stream_tail + bytes_to_write) % stream_size,
			__ATOMIC_RELEASE);
	}
	return 0;
}

extern volatile sig_atomic_t running;
void sigint_callback_handler(int signum)
{
	if (signum == 2) {
		running = 0;
		do_exit = true;
	}
	fprintf(stderr, "[driver] Caught signal %d\n", signum);
	do_exit = true;

}

void sigalrm_callback_handler()
{
}

int getSamples(int64_t lo_freq, int64_t hi_freq)
{
    int result = 0;
	uint8_t tSample = 0;
	int64_t central_freq;
	uint64_t byte_count_now = 0;
	char path[60];

    tSample = (hi_freq - lo_freq)/DEFAULT_SAMPLE_RATE_HZ;
				
    central_freq = lo_freq + DEFAULT_CENTRAL_FREQ_HZ;
    fprintf(stderr, "[driver] central frequency: %lu\n", central_freq);

    result = hackrf_init();
	if (result != HACKRF_SUCCESS) {
		fprintf(stderr,
			"[driver] hackrf_init() failed: %s (%d)\n",
			hackrf_error_name(result),
			result);
		return -1;
	}

	// Configura manejadores de señales
	signal(SIGINT, &sigint_callback_handler);
	signal(SIGILL, &sigint_callback_handler);
	signal(SIGFPE, &sigint_callback_handler);
	signal(SIGSEGV, &sigint_callback_handler);
	signal(SIGTERM, &sigint_callback_handler);
	signal(SIGABRT, &sigint_callback_handler);

	signal(SIGALRM, &sigalrm_callback_handler);

	fprintf(stderr, "[driver] Device initialized\r\n");

    for(uint8_t i=0; i<tSample; i++)
	{		
		byte_count_now = 0;
		bytes_to_xfer = DEFAULT_SAMPLES_TO_XFER_MAX * 2ull;
		
		memset(path, 0, 20);
		sprintf(path, "backend/Core/Samples/%d", i);
		file = fopen(path, "wb");
	
		if (file == NULL) {
			fprintf(stderr, "[driver] Failed to open file: %s\n", path);
			return -1;
		}
		/* Change file buffer to have bigger one to store or read data on/to HDD */
		result = setvbuf(file, NULL, _IOFBF, FD_BUFFER_SIZE);
		if (result != 0) {
			fprintf(stderr, "[driver] setvbuf() failed: %d\n", result);
			return -1;
		}

		fprintf(stderr,"[driver] Start Acquisition\n");

		result = hackrf_open(&device);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"[driver] hackrf_open() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
			return -1;
		}
		
        result = hackrf_set_sample_rate(device, DEFAULT_SAMPLE_RATE_HZ);
        if (result != HACKRF_SUCCESS) {
            fprintf(stderr,
                "[driver] hackrf_set_sample_rate() failed: %s (%d)\n",
                hackrf_error_name(result),
                result);
            return -1;
        }		
		
		result = hackrf_set_hw_sync_mode(device, 0);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"[driver] hackrf_set_hw_sync_mode() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
			return -1;
		}
		
        result = hackrf_set_freq(device, central_freq);
        if (result != HACKRF_SUCCESS) {
            fprintf(stderr,
                "[driver] hackrf_set_freq() failed: %s (%d)\n",
                hackrf_error_name(result),
                result);
            return -1;
        }		
		
        result = hackrf_set_vga_gain(device, 0);
        result |= hackrf_set_lna_gain(device, 0);
        result |= hackrf_start_rx(device, rx_callback, NULL);

		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"[driver] hackrf_start_rx() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
			return -1;
		}

		// Wait for SIGALRM from interval timer, or another signal.
		pause();

		/* Read and reset both totals at approximately the same time. */
		byte_count_now = byte_count;
		byte_count = 0;

		if (!((byte_count_now == 0))) {
			fprintf(stderr, "[driver] Name file RDY: %d\n", i);				
		}

		if ((byte_count_now == 0)) {
			fprintf(stderr,
				"[driver] Couldn't transfer any bytes for one second.\n");
			break;
		}	

		// Stop interval timer.
		result = hackrf_is_streaming(device);
		if (do_exit) {
			fprintf(stderr, "Exiting...\n");
		} else {
			fprintf(stderr,
				"[driver] Exiting... device_is_streaming() result: %s (%d)\n",
				hackrf_error_name(result),
				result);
		}

		result = hackrf_stop_rx(device);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"[driver] stop_rx() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
		} else {
			fprintf(stderr, "[driver] stop_rx() done\n");
		}		

		if (file != NULL) {
			if (file != stdin) {
				fflush(file);
			}
			if ((file != stdout) && (file != stdin)) {
				fclose(file);
				file = NULL;
				fprintf(stderr, "[driver] fclose() done\n");
			}
		}

		result = hackrf_close(device);
		if (result != HACKRF_SUCCESS) {
			fprintf(stderr,
				"[driver] device_close() failed: %s (%d)\n",
				hackrf_error_name(result),
				result);
		} else {
			fprintf(stderr, "[driver] device_close() done\n");
		}			
	}

	if (device != NULL) 
	{
		hackrf_exit();
		fprintf(stderr, "[driver] device_exit() done\n");
	}

	fprintf(stderr, "[driver] exit\n");
	return 0;
}
