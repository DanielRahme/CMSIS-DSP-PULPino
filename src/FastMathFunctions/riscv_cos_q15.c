/* ----------------------------------------------------------------------
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.
*
* $Date:        07. September 2015
* $Revision:    V.1.4.5 a
*
* Project:      CMSIS DSP Library
* Title:        arm_cos_q15.c
*
* Description:  Fast cosine calculation for Q15 values.
*
* Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.
*   - Neither the name of ARM LIMITED nor the names of its contributors
*     may be used to endorse or promote products derived from this
*     software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.

 Modifications 2017  Mostafa Saleh       (Ported to RISC-V PULPino)
* -------------------------------------------------------------------- */

#include <riscv_dsp/riscv_math.h>
#include <riscv_dsp/riscv_common_tables.h>

/**
 * @ingroup groupFastMath
 */

 /**
 * @addtogroup cos
 * @{
 */

/**
 * @brief Fast approximation to the trigonometric cosine function for Q15 data.
 * @param[in] x Scaled input value in radians.
 * @return  cos(x).
 *
 * The Q15 input value is in the range [0 +0.9999] and is mapped to a radian
 * value in the range [0 2*pi).
 */

q15_t riscv_cos_q15(
  q15_t x)
{
  q15_t cosVal;                                  /* Temporary variables for input, output */
  int32_t index;                                 /* Index variables */
  q15_t a, b;                                    /* Four nearest output values */
  q15_t fract;                                   /* Temporary values for fractional values */

  /* add 0.25 (pi/2) to read sine table */
  x = (uint16_t)x + 0x2000;
  if(x < 0)
  {   /* convert negative numbers to corresponding positive ones */
      x = (uint16_t)x + 0x8000;
  }

  /* Calculate the nearest index */
  index = (uint32_t)x >> FAST_MATH_Q15_SHIFT;

  /* Calculation of fractional value */
  fract = (x - (index << FAST_MATH_Q15_SHIFT)) << 9;

  /* Read two nearest values of input value from the sin table */
  a = sinTable_q15[index];
  b = sinTable_q15[index+1];

  /* Linear interpolation process */
  cosVal = (q31_t)(0x8000-fract)*a >> 16;
  cosVal = (q15_t)((((q31_t)cosVal << 16) + ((q31_t)fract*b)) >> 16);

  return cosVal << 1;
}

/**
 * @} end of cos group
 */
