/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:		arm_cmplx_mag_squared_q31.c    
*    
* Description:	Q31 complex magnitude squared.    
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
* ---------------------------------------------------------------------------- */

#include <riscv_dsp/riscv_math.h>

/**    
 * @ingroup groupCmplxMath    
 */

/**    
 * @addtogroup cmplx_mag_squared    
 * @{    
 */


/**    
 * @brief  Q31 complex magnitude squared    
 * @param  *pSrc points to the complex input vector    
 * @param  *pDst points to the real output vector    
 * @param  numSamples number of complex samples in the input vector    
 * @return none.    
 *    
 * <b>Scaling and Overflow Behavior:</b>    
 * \par    
 * The function implements 1.31 by 1.31 multiplications and finally output is converted into 3.29 format.    
 * Input down scaling is not required.    
 */

void riscv_cmplx_mag_squared_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t numSamples)
{
  q31_t real, imag;                              /* Temporary variables to store real and imaginary values */
  q31_t acc0, acc1;                              /* Accumulators */

  while(numSamples > 0u)
  {
    /* out = ((real * real) + (imag * imag)) */
    real = *pSrc++;
    imag = *pSrc++;
    acc0 = (q31_t) (((q63_t) real * real) >> 33);
    acc1 = (q31_t) (((q63_t) imag * imag) >> 33);
    /* store the result in 3.29 format in the destination buffer. */
    *pDst++ = acc0 + acc1;

    /* Decrement the loop counter */
    numSamples--;
  }

}

/**    
 * @} end of cmplx_mag_squared group    
 */
