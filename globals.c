
#include <time.h>
#include "globals.h"




void times_iso8601(char *gmt, char *lcl){

	time_t rawtime = time(NULL);

	struct tm *ptm = gmtime(&rawtime);
	strftime(gmt, 40, "%FT%TZ", ptm);
	ptm = localtime(&rawtime);
	strftime(lcl, 40, "%FT%TZ", ptm);

}

/// Convert seconds to milliseconds

/// Get a time stamp in milliseconds.
uint64_t millis(){
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint64_t ms = SEC_TO_MS((uint64_t)ts.tv_sec) + NS_TO_MS((uint64_t)ts.tv_nsec);
    return ms;
}

/// Get a time stamp in microseconds.
uint64_t micros(){
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint64_t us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
    return us;
}

/// Get a time stamp in nanoseconds.
uint64_t nanos(){
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    uint64_t ns = SEC_TO_NS((uint64_t)ts.tv_sec) + (uint64_t)ts.tv_nsec;
    return ns;
}

// NB: for all 3 timestamp functions above: gcc defines the type of the internal
// `tv_sec` seconds value inside the `struct timespec`, which is used
// internally in these functions, as a signed `long int`. For architectures
// where `long int` is 64 bits, that means it will have undefined
// (signed) overflow in 2^64 sec = 5.8455 x 10^11 years. For architectures
// where this type is 32 bits, it will occur in 2^32 sec = 136 years. If the
// implementation-defined epoch for the timespec is 1970, then your program
// could have undefined behavior signed time rollover in as little as
// 136 years - (year 2021 - year 1970) = 136 - 51 = 85 years. If the epoch
// was 1900 then it could be as short as 136 - (2021 - 1900) = 136 - 121 =
// 15 years. Hopefully your program won't need to run that long. :). To see,
// by inspection, what your system's epoch is, simply print out a timestamp and
// calculate how far back a timestamp of 0 would have occurred. Ex: convert
// the timestamp to years and subtract that number of years from the present
// year.
