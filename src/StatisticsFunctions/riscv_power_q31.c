/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5  
*    
* Project: 	    CMSIS DSP Library    
* Title:		arm_power_q31.c    
*    
* Description:	Sum of the squares of the elements of a Q31 vector.   
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
 * @ingroup groupStats    
 */

/**    
 * @addtogroup power    
 * @{    
 */

/**    
 * @brief Sum of the squares of the elements of a Q31 vector.    
 * @param[in]       *pSrc points to the input vector    
 * @param[in]       blockSize length of the input vector    
 * @param[out]      *pResult sum of the squares value returned here    
 * @return none.    
 *    
 * @details    
 * <b>Scaling and Overflow Behavior:</b>    
 *    
 * \par    
 * The function is implemented using a 64-bit internal accumulator.    
 * The input is represented in 1.31 format.    
 * Intermediate multiplication yields a 2.62 format, and this    
 * result is truncated to 2.48 format by discarding the lower 14 bits.    
 * The 2.48 result is then added without saturation to a 64-bit accumulator in 16.48 format.    
 * With 15 guard bits in the accumulator, there is no risk of overflow, and the    
 * full precision of the intermediate multiplication is preserved.    
 * Finally, the return result is in 16.48 format.     
 *    
 */

void riscv_power_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q63_t * pResult)
{
  q63_t sum = 0;                                 /* Temporary result storage */
  q31_t in;
  uint32_t blkCnt;                               /* loop counter */
  /* Loop over blockSize number of values */
  blkCnt = blockSize;
  while(blkCnt > 0u)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */
    /* Compute Power and then store the result in a temporary variable, sum. */
    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14u;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Store the results in 16.48 format  */
  *pResult = sum;
}

/**    
 * @} end of power group    
 */
