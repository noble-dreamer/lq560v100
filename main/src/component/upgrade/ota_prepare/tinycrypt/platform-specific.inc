/* Platform-specific types and macros for ARM Linux (SD3589E) */

#ifndef _UECC_PLATFORM_SPECIFIC_H_
#define _UECC_PLATFORM_SPECIFIC_H_

#include <stdint.h>

/* 32-bit ARM, use pure C (no asm optimization) */
#define uECC_PLATFORM      uECC_arch_other
#define uECC_WORD_SIZE     4
#define uECC_OPTIMIZATION_LEVEL 0

typedef uint32_t uECC_word_t;
typedef uint64_t uECC_dword_t;
/* Use signed int for wordcount_t to avoid infinite loop in
 * for (i = num_words - 1; i >= 0; --i) when num_words == 0 */
typedef int wordcount_t;
typedef int bitcount_t;  /* must be signed: uECC_verify() uses i >= 0 loop exit */
typedef int cmpresult_t;

#define HIGH_BIT_SET        0x80000000
#define uECC_WORD_BITS      32
#define uECC_WORD_BITS_SHIFT 5
#define uECC_WORD_BITS_MASK 0x1F

#endif /* _UECC_PLATFORM_SPECIFIC_H_ */
