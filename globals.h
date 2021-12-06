#ifndef GLOBAL_H_
#define GLOBAL_H_


#include <stdint.h>
#include <stdbool.h>

#define SEC_TO_MS(sec) ((sec)*1000)
/// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
/// Convert seconds to nanoseconds
#define SEC_TO_NS(sec) ((sec)*1000000000)

/// Convert nanoseconds to seconds
#define NS_TO_SEC(ns)   ((ns)/1000000000)
/// Convert nanoseconds to milliseconds
#define NS_TO_MS(ns)    ((ns)/1000000)
/// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

uint64_t millis(void);
uint64_t micros(void);
uint64_t nanos(void);

void times_iso8601(char *gmt, char *lcl);




#endif
