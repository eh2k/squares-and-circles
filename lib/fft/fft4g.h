/*
 * http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html
 * Copyright Takuya OOURA, 1996-2001
 *
 * You may use, copy, modify and distribute this code for any purpose (include
 * commercial use) and without fee. Please refer to this package when you modify
 * this code.
 *
 * Changes:
 * replaced "double to float" by eh2k.
 */

#include <sys/cdefs.h>

__BEGIN_DECLS

void rdft(int n, int isgn, float *a, int *ip, float *w);
void cdft(int n, int isgn, float *a, int *ip, float *w);

__END_DECLS