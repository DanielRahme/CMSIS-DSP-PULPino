/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5  
*    
* Project: 	    CMSIS DSP Library    
* Title:		arm_rms_q31.c    
*    
* Description:	Root Mean Square of the elements of a Q31 vector.    
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
 * @addtogroup RMS        
 * @{        
 */


/**        
 * @brief Root Mean Square of the elements of a Q31 vector.        
 * @param[in]       *pSrc points to the input vector        
 * @param[in]       blockSize length of the input vector        
 * @param[out]      *pResult rms value returned here        
 * @return none.        
 *        
 * @details        
 * <b>Scaling and Overflow Behavior:</b>        
 *        
 *\par        
 * The function is implemented using an internal 64-bit accumulator.        
 * The input is represented in 1.31 format, and intermediate multiplication        
 * yields a 2.62 format.        
 * The accumulator maintains full precision of the intermediate multiplication results,         
 * but provides only a single guard bit.        
 * There is no saturation on intermediate additions.        
 * If the accumulator overflows, it wraps around and distorts the result.         
 * In order to avoid overflows completely, the input signal must be scaled down by         
 * log2(blockSize) bits, as a total of blockSize additions are performed internally.         
 * Finally, the 2.62 accumulator is right shifted by 31 bits to yield a 1.31 format value.        
 *        
 */

void riscv_rms_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult)
{
  q63_t sum = 0;                                 /* accumulator */
  q31_t in;                                      /* Temporary variable to store the input */
  uint32_t blkCnt;                               /* loop counter */

  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */
    /* Compute sum of the squares and then store the results in a temporary variable, sum */
    in = *pSrc++;
    sum += (q63_t) in *in;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Convert data in 2.62 to 1.31 by 31 right shifts and saturate */
  /* Compute Rms and store the result in the destination vector */
  riscv_sqrt_q31(clip_q63_to_q31((sum / (q63_t) blockSize) >> 31), pResult);
}

/**        
 * @} end of RMS group        
 */
