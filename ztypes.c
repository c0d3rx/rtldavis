#include "ztypes.h"

CI16_t ci16_complex_conjugate(CI16_t arg)    {
    return (CI16_t) {.i=arg.i, .q=-arg.q};
}

CI16_t ci16_complex_product(CI16_t arg1, CI16_t arg2){
    return (CI16_t) {
        .i = (arg1.i * arg2.i) - (arg1.q * arg2.q),
        .q = (arg1.i * arg2.q) + (arg1.q * arg2.i)
    };
}

CF32_t cf32_complex_conjugate(CF32_t arg)    {
    return (CF32_t) {.i=arg.i, .q=-arg.q};
}

CF32_t cf32_complex_product(CF32_t arg1, CF32_t arg2){
    return (CF32_t) {
        .i = (arg1.i * arg2.i) - (arg1.q * arg2.q),
        .q = (arg1.i * arg2.q) + (arg1.q * arg2.i)
    };
}

CI16_t uiq8_iq16(CUI8_t in){
  return (CI16_t) {.i=(in.i-128)*256, .q=(in.q-128)*256};
}


CF32_t uiq8_fc32(CUI8_t in){
  return (CF32_t){.i=(float)(in.i-128)/127.0, .q=(float)(in.q-128)/127.0};
}

float cf32_abs2(CF32_t arg1){
	return arg1.i*arg1.i + arg1.q*arg1.q;
}

