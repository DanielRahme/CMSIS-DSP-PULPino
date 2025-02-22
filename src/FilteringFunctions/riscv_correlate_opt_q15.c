/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:		arm_correlate_opt_q15.c    
*    
* Description:	Correlation of Q15 sequences.  
*    
* Target Processor: Cortex-M4/Cortex-M3
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
 * @ingroup groupFilters    
 */

/**    
 * @addtogroup Corr    
 * @{    
 */

/**    
 * @brief Correlation of Q15 sequences.  
 * @param[in] *pSrcA points to the first input sequence.    
 * @param[in] srcALen length of the first input sequence.    
 * @param[in] *pSrcB points to the second input sequence.    
 * @param[in] srcBLen length of the second input sequence.    
 * @param[out] *pDst points to the location where the output result is written.  Length 2 * max(srcALen, srcBLen) - 1.    
 * @param[in]  *pScratch points to scratch buffer of size max(srcALen, srcBLen) + 2*min(srcALen, srcBLen) - 2.    
 * @return none.    
 *    
 * \par Restrictions    
 *  If the silicon does not support unaligned memory access enable the macro UNALIGNED_SUPPORT_DISABLE    
 *	In this case input, output, scratch buffers should be aligned by 32-bit    
 *     
 * @details    
 * <b>Scaling and Overflow Behavior:</b>    
 *    
 * \par    
 * The function is implemented using a 64-bit internal accumulator.    
 * Both inputs are in 1.15 format and multiplications yield a 2.30 result.    
 * The 2.30 intermediate results are accumulated in a 64-bit accumulator in 34.30 format.    
 * This approach provides 33 guard bits and there is no risk of overflow.    
 * The 34.30 result is then truncated to 34.15 format by discarding the low 15 bits and then saturated to 1.15 format.    
 *    
 * \par    
 * Refer to <code>riscv_correlate_fast_q15()</code> for a faster but less precise version of this function for Cortex-M3 and Cortex-M4.   
 *  
 * 
 */


void riscv_correlate_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  q15_t * pScratch)
{
  q15_t *pIn1;                                   /* inputA pointer               */
  q15_t *pIn2;                                   /* inputB pointer               */
  q63_t acc0, acc1, acc2, acc3;                  /* Accumulators                  */
  q15_t *py;                                     /* Intermediate inputB pointer  */
  q31_t x1, x2, x3;                              /* temporary variables for holding input1 and input2 values */
  uint32_t j, blkCnt, outBlockSize;              /* loop counter                 */
  int32_t inc = 1;                               /* output pointer increment     */
  uint32_t tapCnt;
  q31_t y1, y2;
  q15_t *pScr;                                   /* Intermediate pointers        */
  q15_t *pOut = pDst;                            /* output pointer               */
  shortV *VectInA;
  shortV *VectInB;
  shortV *VectInC;
  shortV VectInD;
  shortV *VectInC1;

  q15_t a, b;


  /* The algorithm implementation is based on the lengths of the inputs. */
  /* srcB is always made to slide across srcA. */
  /* So srcBLen is always considered as shorter or equal to srcALen */
  /* But CORR(x, y) is reverse of CORR(y, x) */
  /* So, when srcBLen > srcALen, output pointer is made to point to the end of the output buffer */
  /* and the destination pointer modifier, inc is set to -1 */
  /* If srcALen > srcBLen, zero pad has to be done to srcB to make the two inputs of same length */
  /* But to improve the performance,        
   * we include zeroes in the output instead of zero padding either of the the inputs*/
  /* If srcALen > srcBLen,        
   * (srcALen - srcBLen) zeroes has to included in the starting of the output buffer */
  /* If srcALen < srcBLen,        
   * (srcALen - srcBLen) zeroes has to included in the ending of the output buffer */
  if(srcALen >= srcBLen)
  {
    /* Initialization of inputA pointer */
    pIn1 = (pSrcA);

    /* Initialization of inputB pointer */
    pIn2 = (pSrcB);

    /* Number of output samples is calculated */
    outBlockSize = (2u * srcALen) - 1u;

    /* When srcALen > srcBLen, zero padding is done to srcB        
     * to make their lengths equal.        
     * Instead, (outBlockSize - (srcALen + srcBLen - 1))        
     * number of output samples are made zero */
    j = outBlockSize - (srcALen + (srcBLen - 1u));

    /* Updating the pointer position to non zero value */
    pOut += j;

  }
  else
  {
    /* Initialization of inputA pointer */
    pIn1 = (pSrcB);

    /* Initialization of inputB pointer */
    pIn2 = (pSrcA);

    /* srcBLen is always considered as shorter or equal to srcALen */
    j = srcBLen;
    srcBLen = srcALen;
    srcALen = j;

    /* CORR(x, y) = Reverse order(CORR(y, x)) */
    /* Hence set the destination pointer to point to the last output sample */
    pOut = pDst + ((srcALen + srcBLen) - 2u);

    /* Destination address modifier is set to -1 */
    inc = -1;

  }

  pScr = pScratch;

  /* Fill (srcBLen - 1u) zeros in scratch buffer */
  riscv_fill_q15(0, pScr, (srcBLen - 1u));

  /* Update temporary scratch pointer */
  pScr += (srcBLen - 1u);



  /* Copy (srcALen) samples in scratch buffer */
  riscv_copy_q15(pIn1, pScr, srcALen);

  /* Update pointers */
  //pIn1 += srcALen;    
  pScr += srcALen;


  /* Fill (srcBLen - 1u) zeros at end of scratch buffer */
  riscv_fill_q15(0, pScr, (srcBLen - 1u));

  /* Update pointer */
  pScr += (srcBLen - 1u);

  /* Temporary pointer for scratch2 */
  py = pIn2;


  /* Actual correlation process starts here */
  blkCnt = (srcALen + srcBLen - 1u) >> 2;

  while(blkCnt > 0)
  {
    /* Initialze temporary scratch pointer as scratch1 */
    pScr = pScratch;

    /* Clear Accumlators */
    acc0 = 0;
    acc1 = 0;
    acc2 = 0;
    acc3 = 0;

    VectInA= (shortV*)pScr;
    pScr+=2;
    VectInB= (shortV*)pScr;
    pScr+=2;

    tapCnt = (srcBLen) >> 2u;

    while(tapCnt > 0u)
    {


      /* Read four samples from smaller buffer */
      VectInC= (shortV*)pIn2;
      VectInC1= (shortV*)(pIn2 + 2 );
		
      acc0 += dotpv2(*VectInA, *VectInC);
      acc2 += dotpv2(*VectInB, *VectInC);

      VectInD[0] = (*VectInA)[1]; 
      VectInD[1] = (*VectInB)[0];

      acc1 += dotpv2(VectInD, *VectInC);
      VectInA= (shortV*)pScr;
      acc0 += dotpv2(*VectInB, *VectInC1);
      acc2 += dotpv2(*VectInA, *VectInC1);
      VectInD[0] = (*VectInB)[1]; 
      VectInD[1] = (*VectInA)[0];
      acc3 += dotpv2(VectInD, *VectInC);
      acc1 += dotpv2(VectInD, *VectInC1);

      VectInB= (shortV*)(pScr+2);
      VectInD[0] = (*VectInA)[1]; 
      VectInD[1] = (*VectInB)[0];

      acc3 += dotpv2(VectInD, *VectInC1);

      pIn2 += 4u;

      pScr += 4u;


      /* Decrement the loop counter */
      tapCnt--;
    }



    /* Update scratch pointer for remaining samples of smaller length sequence */
    pScr -= 4u;


    /* apply same above for remaining samples of smaller length sequence */
    tapCnt = (srcBLen) & 3u;

    while(tapCnt > 0u)
    {

      /* accumlate the results */
      acc0 += (*pScr++ * *pIn2);
      acc1 += (*pScr++ * *pIn2);
      acc2 += (*pScr++ * *pIn2);
      acc3 += (*pScr++ * *pIn2++);

      pScr -= 3u;

      /* Decrement the loop counter */
      tapCnt--;
    }

    blkCnt--;


    /* Store the results in the accumulators in the destination buffer. */
    *pOut = (__SSAT(acc0 >> 15u, 16));
    pOut += inc;
    *pOut = (__SSAT(acc1 >> 15u, 16));
    pOut += inc;
    *pOut = (__SSAT(acc2 >> 15u, 16));
    pOut += inc;
    *pOut = (__SSAT(acc3 >> 15u, 16));
    pOut += inc;

    /* Initialization of inputB pointer */
    pIn2 = py;

    pScratch += 4u;

  }


  blkCnt = (srcALen + srcBLen - 1u) & 0x3;

  /* Calculate correlation for remaining samples of Bigger length sequence */
  while(blkCnt > 0)
  {
    /* Initialze temporary scratch pointer as scratch1 */
    pScr = pScratch;

    /* Clear Accumlators */
    acc0 = 0;

    tapCnt = (srcBLen) >> 1u;

    while(tapCnt > 0u)
    {

      acc0 += (*pScr++ * *pIn2++);
      acc0 += (*pScr++ * *pIn2++);

      /* Decrement the loop counter */
      tapCnt--;
    }

    tapCnt = (srcBLen) & 1u;

    /* apply same above for remaining samples of smaller length sequence */
    while(tapCnt > 0u)
    {

      /* accumlate the results */
      acc0 += (*pScr++ * *pIn2++);

      /* Decrement the loop counter */
      tapCnt--;
    }

    blkCnt--;

    /* Store the result in the accumulator in the destination buffer. */
    *pOut = (q15_t) (__SSAT((acc0 >> 15), 16));

    pOut += inc;

    /* Initialization of inputB pointer */
    pIn2 = py;

    pScratch += 1u;

  }


}

/**    
 * @} end of Corr group    
 */
