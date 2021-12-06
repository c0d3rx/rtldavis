#ifndef ZTYPES_H
#define ZTYPES_H

#include <stdint.h>

/** types for r/w samples **/
/** type and size in bytes **/

#define S_UI8   0x01
#define S_CUI8  0x12      // rtl
#define S_I8    0x21
#define S_CI8   0x32
#define S_UI16  0x42
#define S_CUI16 0x54
#define S_I16   0x62
#define S_CI16  0x74
#define S_F32   0x84
#define S_CF32  0x98

#define ztype_get_size(x) (x&0x0F)

typedef struct CI16 {
  int16_t i;
  int16_t q;
} CI16_t;

typedef struct CF32 {
  float i;
  float q;
} CF32_t;


typedef struct CUI8{
  uint8_t i;
  uint8_t q;
} CUI8_t;


CI16_t uiq8_iq16(CUI8_t in);
CF32_t uiq8_fc32(CUI8_t in);

CI16_t ci16_complex_conjugate(CI16_t arg);
CI16_t ci16_complex_product(CI16_t arg1, CI16_t arg2);

CF32_t cf32_complex_conjugate(CF32_t arg);
CF32_t cf32_complex_product(CF32_t arg1, CF32_t arg2);

float cf32_abs2(CF32_t arg1);

#endif
