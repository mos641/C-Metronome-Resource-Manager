// metronome.c
// program that runs a metronome using a resource manager, a timer and pulses

#include "metronome.h"

name_attach_t *attach;
Metronome_t Metronome;

char data[255];
int server_coid;

int main(int argc, char *argv[]) {
	dispatch_t *dpp;
	resmgr_io_funcs_t io_funcs;
	resmgr_connect_funcs_t conn_funcs;

	ioattr_t ioMetronome;
	ioattr_t ioMetronomeHelp;

	dispatch_context_t *ctp;

	pthread_attr_t thread_attrib;
	int id;

	int server_coid;

	// check command line arguments
	if (argc != 4) {
		fprintf(stderr,
				"Error - Invalid Usage, use format of ./metronome beats-per-minute time-signature-top time-signature-bottom\n");
		exit(EXIT_FAILURE);
	}

	// store command line arguments
	Metronome.beatsPerMinute = atoi(argv[1]);
	Metronome.timeSignatureTop = atoi(argv[2]);
	Metronome.timeSignatureBottom = atoi(argv[3]);

	if ((dpp = dispatch_create()) == NULL) {
		fprintf(stderr, "%s:  Unable to allocate dispatch context.\n", argv[0]);
		return (EXIT_FAILURE);
	}

	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &conn_funcs, _RESMGR_IO_NFUNCS,
			&io_funcs);
	conn_funcs.open = io_open;
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	// override functions for ocb to attach device type
	iofunc_funcs_t metro_ocb_funcs = {
	_IOFUNC_NFUNCS, metro_ocb_calloc, metro_ocb_free, };
	iofunc_mount_t metro_mount = { 0, 0, 0, 0, &metro_ocb_funcs };

	// metronome
	iofunc_attr_init(&ioMetronome.attr, S_IFCHR | 0666, NULL, NULL);
	ioMetronome.device = METRONOME;
	// mount functions
	ioMetronome.attr.mount = &metro_mount;
	// attach
	if ((id = resmgr_attach(dpp, NULL, "/dev/local/metronome", _FTYPE_ANY, 0,
			&conn_funcs, &io_funcs, &ioMetronome)) == -1) {
		fprintf(stderr, "%s:  Unable to attach name.\n", argv[0]);
		return (EXIT_FAILURE);
	}

	// metronom-help
	iofunc_attr_init(&ioMetronomeHelp.attr, S_IFCHR | 0666, NULL, NULL);
	ioMetronomeHelp.device = METRONOME_HELP;
	// mount functions
	ioMetronomeHelp.attr.mount = &metro_mount;
	// attach
	if ((id = resmgr_attach(dpp, NULL, "/dev/local/metronome-help", _FTYPE_ANY,
			0, &conn_funcs, &io_funcs, &ioMetronomeHelp)) == -1) {
		fprintf(stderr, "%s:  Unable to attach name.\n", argv[0]);
		return (EXIT_FAILURE);
	}

	ctp = dispatch_context_alloc(dpp);

	// create thread
	pthread_attr_init(&thread_attrib);
	pthread_create(NULL, &thread_attrib, &metronome_thread, &Metronome);

	while (1) {
		if ((ctp = dispatch_block(ctp))) {
			dispatch_handler(ctp);
		} else
			fprintf(stderr, "ERROR \n");

	}

	pthread_attr_destroy(&thread_attrib);
	name_detach(attach, 0);
	name_close(server_coid);
	return EXIT_SUCCESS;

}

void* metronome_thread(void *ta) {
	struct sigevent event;
	my_message_t msg;
	int rcvid;
	char *intervalString;
	struct itimerspec itime;
	timer_t timer_id;

	if ((attach = name_attach(NULL, RESOURCE_NAME, 0)) == NULL) {
		fprintf(stderr, "\nError - name_attach failed\n");
		exit(EXIT_FAILURE);
	}

	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, attach->chid,
	_NTO_SIDE_CHANNEL, 0);
	event.sigev_priority = SIGEV_PULSE_PRIO_INHERIT;
	event.sigev_code = GENERAL_PULSE;

	// create timer
	timer_create(CLOCK_REALTIME, &event, &timer_id);
	start_interval_timer(&itime, timer_id, &Metronome);

	intervalString = get_interval_string(
			Metronome.timeSignatureTop + Metronome.timeSignatureBottom);

	while (1) {
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg),
		NULL);

		// received pulse
		if (rcvid == 0) {
			switch (msg.pulse.code) {
			case GENERAL_PULSE:
				// keep writing interval string
				if (*intervalString == '\0') {
					printf("\n");
					intervalString = get_interval_string(
							Metronome.timeSignatureTop
									+ Metronome.timeSignatureBottom);
				} else if (*intervalString == '|') {
					printf("%.2s", intervalString);
					intervalString = (intervalString + 2);
				} else {
					printf("%c", *intervalString++);
				}
				break;
			case RESET_PULSE:
				// set new timing
				intervalString = get_interval_string(
						Metronome.timeSignatureTop
								+ Metronome.timeSignatureBottom);
				printf("\n");
				start_interval_timer(&itime, timer_id, &Metronome);
				break;
			case START_PULSE:
				// start only if stopped
				if (Metronome.timeStatus == STOPPED) {
					Metronome.timeStatus = RUNNING;
					start_interval_timer(&itime, timer_id, &Metronome);
				}
				break;
			case PAUSE_PULSE:
				// pause only if running
				if (Metronome.timeStatus == RUNNING) {
					itime.it_value.tv_sec = msg.pulse.value.sival_int;
					timer_settime(timer_id, 0, &itime, NULL);
				}
				break;
			case STOP_PULSE:
				// stop only if running
				if (Metronome.timeStatus == RUNNING) {
					Metronome.timeStatus = STOPPED;
					itime.it_value.tv_sec = 0;
					timer_settime(timer_id, 0, &itime, NULL);
				}
				break;
			case QUIT_PULSE:
				// quit and end
				timer_delete(timer_id);
				name_detach(attach, 0);
				name_close(server_coid);
				exit(EXIT_SUCCESS);
			}
		} else {
			// error received
			fprintf(stderr, "\nerror message received\n");
			exit(EXIT_FAILURE);
		}
		fflush(stdout);
	}
	return NULL;
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, metro_ocb_t *mocb) {

	int nb;

	if (data == NULL)
		return 0;

	// corresponding response message data, /metronome or /metronome-help
	if (mocb->ocb.attr->device == METRONOME) {

		sprintf(data,
				"\n[metronome: %d beats/min, time signature: %d/%d, sec-per-interval: %.2f, nanoSecs: %.0lf]\n",
				Metronome.beatsPerMinute, Metronome.timeSignatureTop,
				Metronome.timeSignatureBottom, Metronome.secondsPerInterval,
				Metronome.nanoSeconds);

	} else {
		sprintf(data,
				"\nMetronome Resource Manager (Resmgr)"
						"\n\n  Usage: metronome <bpm> <ts-top> <ts-bottom>"
						"\n\n  API:"
						"\n   pause[1-9]\t\t\t\t- pause the metronome for 1-9 seconds"
						"\n   quit:\t\t\t\t- quit the metronome"
						"\n   set <bpm> <ts-top> <ts-bottom>\t- set the metronome to <beatsPerMinute> ts-top/ts-bottom"
						"\n   start\t\t\t\t- start the metronome from stopped state"
						"\n   stop\t\t\t\t\t- stop the metronome; use 'start' to resume\n\n");

	}

	nb = strlen(data);

	// test to see if we have already sent the whole message
	if (mocb->ocb.offset == nb)
		return 0;

	// we will return which ever is smaller the size of our data or the size of the buffer
	nb = min(nb, msg->i.nbytes);

	// set the number of bytes we will return
	_IO_SET_READ_NBYTES(ctp, nb);

	// copy data into reply buffer
	SETIOV(ctp->iov, data, nb);

	// update offset into our data used to determine start position for next read
	mocb->ocb.offset += nb;

	// if we are going to send any bytes update the access time for this resource
	if (nb > 0)
		mocb->ocb.flags |= IOFUNC_ATTR_ATIME;

	return (_RESMGR_NPARTS(1));
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, metro_ocb_t *mocb) {

	int nb = 0;

	// if user attempts to write to metronome help display error message
	if (mocb->ocb.attr->device == METRONOME_HELP) {
		fprintf(stderr,
				"\nError - can not write to /dev/local/metronome-help\n");

		nb = msg->i.nbytes;
		_IO_SET_WRITE_NBYTES(ctp, nb);
		return (_RESMGR_NPARTS(0));
	}

	if (msg->i.nbytes == ctp->info.msglen - (ctp->offset + sizeof(*msg))) {
		char *buf;
		char *alert_msg;
		char *timing_msg;
		int i, small_integer = 0;
		buf = (char*) (msg + 1);

		// echo set
		if (strstr(buf, "set") != NULL) {
			for (i = 0; i < 4; i++) {
				timing_msg = strsep(&buf, " ");
				if (timing_msg == NULL) {
					fprintf(stderr,
							"\nError - invalid arguments for set command\n");
					break;
				}

				if (i == 1) {
					Metronome.beatsPerMinute = atoi(timing_msg);
				} else if (i == 2) {
					Metronome.timeSignatureTop = atoi(timing_msg);
				} else if (i == 3) {
					Metronome.timeSignatureBottom = atoi(timing_msg);
				}
			}
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), RESET_PULSE,
					small_integer);
			// echo pause
		} else if (strstr(buf, "pause") != NULL) {
			for (i = 0; i < 2; i++) {
				alert_msg = strsep(&buf, " ");
				if (alert_msg == NULL) {
					fprintf(stderr,
							"\nError - invalid arguments for pause command\n");
					break;
				}
			}
			small_integer = atoi(alert_msg);
			if (small_integer >= 1 && small_integer <= 9) {
				MsgSendPulse(server_coid, SchedGet(0, 0, NULL), PAUSE_PULSE,
						small_integer);
			} else {
				printf("\nInteger is not between 1 and 9.\n");
			}
			// echo start
		} else if (strstr(buf, "start") != NULL) {
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), START_PULSE,
					small_integer);
			// echo stop
		} else if (strstr(buf, "stop") != NULL) {
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), STOP_PULSE,
					small_integer);
			// echo quit
		} else if (strstr(buf, "quit") != NULL) {
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), QUIT_PULSE,
					small_integer);
		} else {
			alert_msg = strsep(&buf, " ");
			alert_msg[strcspn(alert_msg, "\r\n")] = 0;
			fprintf(stderr, "\nError - \'%s\' is not a valid command\n",
					alert_msg);
		}

		nb = msg->i.nbytes;
	}

	_IO_SET_WRITE_NBYTES(ctp, nb);

	if (msg->i.nbytes > 0) {
		mocb->ocb.flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;
	}

	return (_RESMGR_NPARTS(0));
}

int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle,
		void *extra) {

	if ((server_coid = name_open(RESOURCE_NAME, 0)) == -1) {
		perror("\nError - name_open failed\n");
		return EXIT_FAILURE;
	}
	return (iofunc_open_default(ctp, msg, &handle->attr, extra));
}

char* get_interval_string(int tstPlusTsb) {
	// return the corresponding string
	switch (tstPlusTsb) {	// tst	tsb	noi	pattern
	case (2 + 4): 			// 2 	4 	4 	|1&2&
		return "|1&2&";
		break;
	case (3 + 4): 			// 3 	4 	6 	|1&2&3&
		return "|1&2&3&";
		break;
	case (4 + 4):			// 4 	4 	8 	|1&2&3&4&
		return "|1&2&3&4&";
		break;
	case (5 + 4):			// 5 	4 	10 	|1&2&3&4-5-
		return "|1&2&3&4-5-";
		break;
	case (3 + 8):			// 3 	8	6 	|1-2-3-
		return "|1-2-3-";
		break;
	case (6 + 8):			// 6 	8 	6 	|1&a2&a
		return "|1&a2&a";
		break;
	case (9 + 8):			// 9 	8 	9 	|1&a2&a3&a
		return "|1&a2&a3&a";
		break;
	case (12 + 8):			// 12	8 	12 	|1&a2&a3&a4&a
		return "|1&a2&a3&a4&a";
		break;
	default:
		return "|1&2&";
		break;
	}
}

void start_interval_timer(struct itimerspec *itime, timer_t timer_id,
		Metronome_t *Metronome) {
	// calculate timing
	Metronome->beatsPerSecond = (double) 60 / Metronome->beatsPerMinute;
	Metronome->beatsPerMeasure = Metronome->beatsPerSecond * 2;
	Metronome->secondsPerInterval = Metronome->beatsPerMeasure
			/ Metronome->timeSignatureBottom;
	Metronome->nanoSeconds = (Metronome->secondsPerInterval
			- (int) Metronome->secondsPerInterval) * 1e+9;

	// start timer
	itime->it_value.tv_sec = 1;
	itime->it_value.tv_nsec = 0;
	itime->it_interval.tv_sec = Metronome->secondsPerInterval;
	itime->it_interval.tv_nsec = Metronome->nanoSeconds;
	timer_settime(timer_id, 0, itime, NULL);
}

metro_ocb_t* metro_ocb_calloc(resmgr_context_t *ctp, ioattr_t *mattr) {
	metro_ocb_t *mocb;
	mocb = calloc(1, sizeof(metro_ocb_t));
	mocb->ocb.offset = 0;
	return (mocb);
}

void metro_ocb_free(metro_ocb_t *mocb) {
	free(mocb);
}
