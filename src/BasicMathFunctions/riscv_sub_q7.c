/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:		arm_sub_q7.c    
*    
* Description:	Q7 vector subtraction.    
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
 * @ingroup groupMath    
 */

/**    
 * @addtogroup BasicSub    
 * @{    
 */

/**    
 * @brief Q7 vector subtraction.    
 * @param[in]       *pSrcA points to the first input vector    
 * @param[in]       *pSrcB points to the second input vector    
 * @param[out]      *pDst points to the output vector    
 * @param[in]       blockSize number of samples in each vector    
 * @return none.    
 *    
 * <b>Scaling and Overflow Behavior:</b>    
 * \par    
 * The function uses saturating arithmetic.    
 * Results outside of the allowable Q7 range [0x80 0x7F] will be saturated.    
 */

void riscv_sub_q7(
  q7_t * pSrcA,
  q7_t * pSrcB,
  q7_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */


#if defined (USE_DSP_RISCV)

  shortV VectInA;
  shortV VectInB;  
  shortV VectInC; 
  /*loop Unrolling */
  blkCnt = blockSize >> 1u;

  while (blkCnt > 0u)
  {
    /*read 2 elements from each source*/
    VectInA[0] = (short)pSrcA[0];
    VectInA[1] = (short)pSrcA[1];
    VectInB[0] = (short)pSrcB[0];
    VectInB[1] = (short)pSrcB[1];
    /*subract them*/
    VectInC = sub2(VectInA,VectInB); 
    /*saturate the results then save to destination buufer*/
    *pDst++ =(q7_t)clip(VectInC[0],-128,127);
    *pDst++ =(q7_t)clip(VectInC[1],-128,127);
    /*increment source buffer*/
    pSrcA+=2;
    pSrcB+=2;
    /*decrement loop counter*/
    blkCnt--;
  }

  blkCnt = blockSize % 0x2u;

  while (blkCnt > 0u)
  {
    /* C = A + B */
    /* subtract then saturate*/
    *pDst++ =(q7_t)clip((*pSrcA++ - *pSrcB++),-128,127);
    /* Decrement the loop counter */
    blkCnt--;
  }

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* C = A - B */
    /* Subtract and then store the result in the destination buffer. */
    *pDst++ = (q7_t) __SSAT((q15_t) * pSrcA++ - *pSrcB++, 8);

    /* Decrement the loop counter */
    blkCnt--;
  }
#endif
}

/**    
 * @} end of BasicSub group    
 */
