/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:	    arm_cmplx_mult_real_q31.c    
*    
* Description:	Q31 complex by real multiplication    
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

/**    
 * @ingroup groupCmplxMath    
 */

/**    
 * @addtogroup CmplxByRealMult    
 * @{    
 */


/**    
 * @brief  Q31 complex-by-real multiplication    
 * @param[in]  *pSrcCmplx points to the complex input vector    
 * @param[in]  *pSrcReal points to the real input vector    
 * @param[out]  *pCmplxDst points to the complex output vector    
 * @param[in]  numSamples number of samples in each vector    
 * @return none.    
 *    
 * <b>Scaling and Overflow Behavior:</b>    
 * \par    
 * The function uses saturating arithmetic.    
 * Results outside of the allowable Q31 range[0x80000000 0x7FFFFFFF] will be saturated.    
 */

void riscv_cmplx_mult_real_q31(
  q31_t * pSrcCmplx,
  q31_t * pSrcReal,
  q31_t * pCmplxDst,
  uint32_t numSamples)
{
  q31_t inA1;                                    /* Temporary variable to store input value */
  while(numSamples > 0u)
  {
    /* realOut = realA * realB.            */
    /* imagReal = imagA * realB.               */
    inA1 = *pSrcReal++;
    /* store the result in the destination buffer. */
    *pCmplxDst++ =
      (q31_t) clip_q63_to_q31(((q63_t) * pSrcCmplx++ * inA1) >> 31);
    *pCmplxDst++ =
      (q31_t) clip_q63_to_q31(((q63_t) * pSrcCmplx++ * inA1) >> 31);

    /* Decrement the numSamples loop counter */
    numSamples--;
  }
}

/**    
 * @} end of CmplxByRealMult group    
 */
