/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:		arm_cmplx_mag_squared_q15.c    
*    
* Description:	Q15 complex magnitude squared.    
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
 * @brief  Q15 complex magnitude squared    
 * @param  *pSrc points to the complex input vector    
 * @param  *pDst points to the real output vector    
 * @param  numSamples number of complex samples in the input vector    
 * @return none.    
 *    
 * <b>Scaling and Overflow Behavior:</b>    
 * \par    
 * The function implements 1.15 by 1.15 multiplications and finally output is converted into 3.13 format.    
 */

void riscv_cmplx_mag_squared_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t numSamples)
{
  q31_t acc0, acc1;                              /* Accumulators */

  q15_t real, imag;                              /* Temporary variables to store real and imaginary values */

#if defined (USE_DSP_RISCV)

  shortV *VectInA;
  while (numSamples > 0u)
  {
    VectInA =  (shortV*)pSrc;/*read 2 elements (a complex number)*/
    acc0 = dotpv2(*VectInA, *VectInA);/*dot product with itself == magnitude squared*/
    *pDst++ = (q15_t) (((q63_t)acc0) >> 17);/*normalize*/
    pSrc+=2;
    numSamples--;
  }

#else
  while(numSamples > 0u)
  {
    /* out = ((real * real) + (imag * imag)) */
    real = *pSrc++;
    imag = *pSrc++;
    acc0 = (real * real);
    acc1 = (imag * imag);
    /* store the result in 3.13 format in the destination buffer. */
    *pDst++ = (q15_t) (((q63_t) acc0 + acc1) >> 17);

    /* Decrement the loop counter */
    numSamples--;
  }
#endif
}

/**    
 * @} end of cmplx_mag_squared group    
 */
