#ifndef RTLTCP_READER_H_
#define RTLTCP_READER_H_

#define SOCKET          int
#define INVALID_SOCKET  (-1)
#define closesocket(x)  close(x)

#pragma pack(push, 1)
struct command {
    unsigned char cmd;
    unsigned int param;
};
#pragma pack(pop)

// rtl_tcp API
#define RTLTCP_SET_FREQ 0x01
#define RTLTCP_SET_SAMPLE_RATE 0x02
#define RTLTCP_SET_GAIN_MODE 0x03
#define RTLTCP_SET_GAIN 0x04
#define RTLTCP_SET_FREQ_CORRECTION 0x05
#define RTLTCP_SET_IF_TUNER_GAIN 0x06
#define RTLTCP_SET_TEST_MODE 0x07
#define RTLTCP_SET_AGC_MODE 0x08
#define RTLTCP_SET_DIRECT_SAMPLING 0x09
#define RTLTCP_SET_OFFSET_TUNING 0x0a
#define RTLTCP_SET_RTL_XTAL 0x0b
#define RTLTCP_SET_TUNER_XTAL 0x0c
#define RTLTCP_SET_TUNER_GAIN_BY_ID 0x0d
#define RTLTCP_SET_BIAS_TEE 0x0e

#pragma pack(push, 1)
struct rtl_tcp_info {
    char magic[4];             // "RTL0"
    uint32_t tuner_number;     // big endian
    uint32_t tuner_gain_count; // big endian
};
#pragma pack(pop)


typedef struct RTLTCPREADER{
	SOCKET sock;
	uint8_t *buffer;
	unsigned int buffer_len;

	void (*callback)(struct RTLTCPREADER *, uint8_t *, unsigned int);
	int  (*read_chunk)(struct RTLTCPREADER *);
	int  (*command) (struct RTLTCPREADER *, char, int);

} t_RTLTCPREADER;

t_RTLTCPREADER *new_rtltcp_from_args(int, char **, unsigned int, void (*)(struct RTLTCPREADER *, uint8_t *, unsigned int));
t_RTLTCPREADER *new_rtltcp(char *, char *, unsigned int, int, float, void (*)(struct RTLTCPREADER *, uint8_t *, unsigned int));
int read_rtltcp_chunk(t_RTLTCPREADER *);
int sendcommand_rtltcp (t_RTLTCPREADER * , char , int );
int free_rtltcp(t_RTLTCPREADER *);



#endif /* RTLTCP_READER_H_ */
