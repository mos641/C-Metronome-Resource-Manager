// metronome.h
// header file for metronome program

#ifndef SRC_METRONOME_H_
#define SRC_METRONOME_H_

struct ioattr_t;
#define IOFUNC_ATTR_T struct ioattr_t
struct metro_ocb;
#define IOFUNC_OCB_T struct metro_ocb

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/types.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

#define RESOURCE_NAME  "metronome"

typedef struct ioattr_t {
	iofunc_attr_t attr;
	int device;
} ioattr_t;

typedef struct metro_ocb{
	iofunc_ocb_t ocb;
	char buffer[50];
}metro_ocb_t;

typedef union {
	struct _pulse pulse;
	char msg[255];
}my_message_t;

enum metronome_pulse{
	GENERAL_PULSE,
	RESET_PULSE,
	START_PULSE,
	PAUSE_PULSE,
	STOP_PULSE,
	QUIT_PULSE
};

enum time_status {
	RUNNING,
	STOPPED
};

enum device_type{
	METRONOME,
	METRONOME_HELP
};

struct Metronome {
	int beatsPerMinute;
	int timeSignatureTop;
	int timeSignatureBottom;

	enum time_status timeStatus;
	double beatsPerSecond;
	double beatsPerMeasure;
	double secondsPerInterval;
	double nanoSeconds;
}typedef Metronome_t;

void *metronome_thread(void*metronome);
int io_read(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);
int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle,void *extra);
char* get_interval_string(int tstPlusTsb);
void start_interval_timer(struct itimerspec * itime, timer_t timer_id,Metronome_t* Metronome);
metro_ocb_t * metro_ocb_calloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *mtattr);
void metro_ocb_free(IOFUNC_OCB_T *mocb);

#endif
