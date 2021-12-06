// C Standard Library (POSIX)
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "rtl-sdr.h"
#include "rtltcp_reader.h"


char *host="127.0.0.1";
char *port="1234";

int default_rtltcp_ppm_error;
float default_rtltcp_gain=43;
int default_ppm = 0;



extern void usage(void);
void rtltcp_reader_usage(void) {
	usage();
	fprintf(stderr, "\t[--host=<host> (default %s)]\n"
			"\t[--port=<port> (default %s)]\n"
			"\t[--gain=<gain> (default %f)]\n"
			"\t--ppm-error=<ppm> (default %d)\n\n",host,port, default_rtltcp_gain, default_ppm);
	exit(1);
}


t_RTLTCPREADER *new_rtltcp_from_args(int argc, char **argv, unsigned int buf_len, void (*callback)(struct RTLTCPREADER *, uint8_t *, unsigned int)){


	static struct option long_options[] = {
			{ "host", required_argument,	NULL, 0 },
			{ "port", required_argument, NULL, 0 },
			{ "gain", required_argument, NULL, 0 },
			{ "ppm-error", required_argument, NULL, 0 },
			{ NULL, 0,	NULL, 0 } };
	int c;
	int option_index = 0;
	float gain=default_rtltcp_gain;
	int ppm = default_ppm;

	optind = 0;

	while ((c = getopt_long(argc, argv, ":h", long_options, &option_index)) != -1) {
		switch (c) {
		case 0:
			switch (option_index) {
			case 0:
				host= malloc(strlen(optarg));
				strcpy(host, optarg);
				break;
			case 1:
				port= malloc(strlen(optarg));
				strcpy(port, optarg);
				break;
			case 2:
				gain = atof (optarg);
				fprintf (stderr, "gain setted to %f\n",gain);
				break;
			case 3:
				ppm = atoi (optarg);
				fprintf (stderr, "ppm setted to %d\n",ppm);
				break;
			}
			break;
		case 'h':
			rtltcp_reader_usage();
			break;
		}
	}

	return new_rtltcp(host, port, buf_len, ppm, gain, callback);

}


t_RTLTCPREADER *new_rtltcp(char *host, char *port , unsigned int buf_len, int ppm, float gain, void (*callback)(struct RTLTCPREADER *, uint8_t *, unsigned int)){
    struct addrinfo hints, *res, *res0;
    int ret;
    SOCKET sock;

	t_RTLTCPREADER *rt = malloc (sizeof(t_RTLTCPREADER));
	rt->buffer = malloc (buf_len);
	rt->buffer_len = buf_len;

    fprintf(stderr, "rtl_tcp input from %s port %s\n", host, port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags    = AI_ADDRCONFIG;

    ret = getaddrinfo(host, port, &hints, &res0);
    if (ret) {
        fprintf(stderr, "%s\n", gai_strerror(ret));
        return NULL;
    }
    sock = INVALID_SOCKET;
    for (res = res0; res; res = res->ai_next) {
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock >= 0) {
            ret = connect(sock, res->ai_addr, res->ai_addrlen);
            if (ret == -1) {
                perror("connect");
                sock = INVALID_SOCKET;
            }
            else
                break; // success
        }
    }
    freeaddrinfo(res0);
    if (sock == INVALID_SOCKET) {
        perror("socket");
        return NULL;
    }

    //int const value_one = 1;
    //ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&value_one, sizeof(value_one));
    //if (ret < 0)
    //    fprintf(stderr, "rtl_tcp TCP_NODELAY failed\n");

    struct rtl_tcp_info info;
    ret = recv(sock, (char *)&info, sizeof (info), 0);
    if (ret != 12) {
        fprintf(stderr, "Bad rtl_tcp header (%d)\n", ret);
        return NULL;
    }
    if (strncmp(info.magic, "RTL0", 4)) {
        info.tuner_number = 0; // terminate magic
        fprintf(stderr, "Bad rtl_tcp header magic \"%s\"\n", info.magic);
        return NULL;
    }

    unsigned tuner_number = ntohl(info.tuner_number);
    //int tuner_gain_count  = ntohl(info.tuner_gain_count);

    char const *tuner_names[] = { "Unknown", "E4000", "FC0012", "FC0013", "FC2580", "R820T", "R828D" };
    char const *tuner_name = tuner_number > sizeof (tuner_names) ? "Invalid" : tuner_names[tuner_number];

    fprintf(stderr, "rtl_tcp connected to %s:%s (Tuner: %s)\n", host, port, tuner_name);

    rt->sock = sock;

    rt->read_chunk = read_rtltcp_chunk;
    rt->command = sendcommand_rtltcp;
    rt->callback = callback;
    sendcommand_rtltcp(rt, RTLTCP_SET_GAIN, (int)(gain*10));
    sendcommand_rtltcp(rt, RTLTCP_SET_FREQ_CORRECTION, ppm);
    return rt;
}


int sendcommand_rtltcp (t_RTLTCPREADER * rtltcp, char cmd, int param){
    struct command command;
    command.cmd   = cmd;
    command.param = htonl(param);

    return sizeof(command) == send(rtltcp->sock, (const char*) &command, sizeof(command), 0) ? 0 : -1;
}


int free_rtltcp(t_RTLTCPREADER * rtltcp){
    int ret = shutdown(rtltcp->sock, SHUT_RDWR);
    if (ret == -1) {
        perror("shutdown");
        return -1;
    }
    ret = closesocket(rtltcp->sock);
    if (ret == -1) {
        perror("close");
        return -1;
    }
    free(rtltcp);
    return 0;
}


int read_rtltcp_chunk(t_RTLTCPREADER * rtltcp){
	bool dev_running=true;
	unsigned n_read = 0;
	int r;

	uint8_t *buffer = rtltcp->buffer;
	unsigned int buf_len = rtltcp->buffer_len;

	do {
		r = recv(rtltcp->sock, buffer+n_read, buf_len - n_read, MSG_WAITALL);
		if (r <= 0)
			break;
		n_read += r;
		//fprintf(stderr, "readStream ret=%d (of %u)\n", r, n_read);
	} while (n_read < buf_len);
	//fprintf(stderr, "readStream ret=%d (read %u)\n", r, n_read);
	if (r < 0) {
		fprintf(stderr, "WARNING: sync read failed. %d\n", r);
	}
	if (n_read == 0) {
		perror("rtl_tcp");
	}

	// callback....
	rtltcp->callback(rtltcp, rtltcp->buffer, rtltcp->buffer_len);

    return 0;
}
