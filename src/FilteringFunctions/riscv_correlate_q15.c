/* ----------------------------------------------------------------------   
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.   
*   
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*   
* Project: 	    CMSIS DSP Library   
* Title:		arm_correlate_q15.c   
*   
* Description:	Correlation of Q15 sequences. 
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
 * @return none.   
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
 * \par    
 * Refer the function <code>riscv_correlate_opt_q15()</code> for a faster implementation of this function using scratch buffers.
 * 
 */

void riscv_correlate_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst)
{
#if defined (USE_DSP_RISCV)
  q15_t *pIn1;                                   /* inputA pointer               */
  q15_t *pIn2;                                   /* inputB pointer               */
  q15_t *pOut = pDst;                            /* output pointer               */
  q63_t sum, acc0, acc1, acc2, acc3;             /* Accumulators                  */
  q15_t *px;                                     /* Intermediate inputA pointer  */
  q15_t *py;                                     /* Intermediate inputB pointer  */
  q15_t *pSrc1;                                  /* Intermediate pointers        */
  q31_t x0, x1, x2, x3, c0;                      /* temporary variables for holding input and coefficient values */
  uint32_t j, k = 0u, count, blkCnt, outBlockSize, blockSize1, blockSize2, blockSize3;  /* loop counter                 */
  int32_t inc = 1;                               /* Destination address modifier */
  q15_t a, b;
  shortV VectInA;
  shortV VectInB;
  shortV VectInC;
  shortV VectInD;
  shortV VectInE;
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

  /* The function is internally   
   * divided into three parts according to the number of multiplications that has to be   
   * taken place between inputA samples and inputB samples. In the first part of the   
   * algorithm, the multiplications increase by one for every iteration.   
   * In the second part of the algorithm, srcBLen number of multiplications are done.   
   * In the third part of the algorithm, the multiplications decrease by one   
   * for every iteration.*/
  /* The algorithm is implemented in three stages.   
   * The loop counters of each stage is initiated here. */
  blockSize1 = srcBLen - 1u;
  blockSize2 = srcALen - (srcBLen - 1u);
  blockSize3 = blockSize1;

  /* --------------------------   
   * Initializations of stage1   
   * -------------------------*/

  /* sum = x[0] * y[srcBlen - 1]   
   * sum = x[0] * y[srcBlen - 2] + x[1] * y[srcBlen - 1]   
   * ....   
   * sum = x[0] * y[0] + x[1] * y[1] +...+ x[srcBLen - 1] * y[srcBLen - 1]   
   */

  /* In this stage the MAC operations are increased by 1 for every iteration.   
     The count variable holds the number of MAC operations performed */
  count = 1u;

  /* Working pointer of inputA */
  px = pIn1;

  /* Working pointer of inputB */
  pSrc1 = pIn2 + (srcBLen - 1u);
  py = pSrc1;

  /* ------------------------   
   * Stage1 process   
   * ----------------------*/

  /* The first loop starts here */
  while(blockSize1 > 0u)
  {
    /* Accumulator is made zero for every iteration */
    sum = 0;

    /* Apply loop unrolling and compute 4 MACs simultaneously. */
    k = count >> 2;

    /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
     ** a second loop below computes MACs for the remaining 1 to 3 samples. */
    while(k > 0u)
    {
      /* x[0] * y[srcBLen - 4] , x[1] * y[srcBLen - 3] */
        sum += ((q31_t) * px++ * *py++);
        sum += ((q31_t) * px++ * *py++);
        sum += ((q31_t) * px++ * *py++);
        sum += ((q31_t) * px++ * *py++);

      /* Decrement the loop counter */
      k--;
    }

    /* If the count is not a multiple of 4, compute any remaining MACs here.   
     ** No loop unrolling is used. */
    k = count % 0x4u;

    while(k > 0u)
    {
      /* Perform the multiply-accumulates */
      /* x[0] * y[srcBLen - 1] */
        sum += ((q31_t) * px++ * *py++);

      /* Decrement the loop counter */
      k--;
    }

    /* Store the result in the accumulator in the destination buffer. */
   /**pOut = (q15_t) (sum >> 15);*/
    *pOut = (q15_t) __SSAT((sum >> 15u), 16u);
    /* Destination pointer is updated according to the address modifier, inc */
    pOut += inc;

    /* Update the inputA and inputB pointers for next MAC calculation */
    py = pSrc1 - count;
    px = pIn1;

    /* Increment the MAC count */
    count++;

    /* Decrement the loop counter */
    blockSize1--;
  }

  /* --------------------------   
   * Initializations of stage2   
   * ------------------------*/

  /* sum = x[0] * y[0] + x[1] * y[1] +...+ x[srcBLen-1] * y[srcBLen-1]   
   * sum = x[1] * y[0] + x[2] * y[1] +...+ x[srcBLen] * y[srcBLen-1]   
   * ....   
   * sum = x[srcALen-srcBLen-2] * y[0] + x[srcALen-srcBLen-1] * y[1] +...+ x[srcALen-1] * y[srcBLen-1]   
   */

  /* Working pointer of inputA */
  px = pIn1;

  /* Working pointer of inputB */
  py = pIn2;

  /* count is index by which the pointer pIn1 to be incremented */
  count = 0u;

  /* -------------------   
   * Stage2 process   
   * ------------------*/

  /* Stage2 depends on srcBLen as in this stage srcBLen number of MACS are performed.   
   * So, to loop unroll over blockSize2,   
   * srcBLen should be greater than or equal to 4, to loop unroll the srcBLen loop */
  if(srcBLen >= 4u)
  {
    /* Loop unroll over blockSize2, by 4 */
    blkCnt = blockSize2 >> 2u;

    while(blkCnt > 0u)
    {
      /* Set all accumulators to zero */
      acc0 = 0;
      acc1 = 0;
      acc2 = 0;
      acc3 = 0;

      /* read x[0], x[1], x[2] samples */
      a = *px;
      b = *(px + 1);

      VectInA[0] = a; /*x0*/
      VectInA[1] = b; /*x1*/
      a = *px;
      VectInB[0] = b; /*x1*/
      VectInB[1] = a; /*x2*/

      px += 2u;

      /* Apply loop unrolling and compute 4 MACs simultaneously. */
      k = srcBLen >> 2u;

      /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
       ** a second loop below computes MACs for the remaining 1 to 3 samples. */
      do
      {
        /* Read the first two inputB samples using SIMD:   
         * y[0] and y[1] */
        a = *py;
        b = *(py + 1);

        VectInC[0] = a; /*y[srcBLen - 1]*/
        VectInC[1] = b; /*y[srcBLen - 2] */

        /* acc0 +=  x[0] * y[0] + x[1] * y[1] */
        acc0 += dotpv2(VectInA, VectInC);

        /* acc1 +=  x[1] * y[0] + x[2] * y[1] */
        acc1 += dotpv2(VectInB, VectInC);

        /* Read x[2], x[3], x[4] */
        a = *px;
        b = *(px + 1);

        VectInD[0] = a; /*x2*/
        VectInD[1] = b; /*x3*/
        a = *(px + 2);
        VectInE[0] = b; /*x3*/
        VectInE[1] = a; /*x4*/
        /* acc2 +=  x[2] * y[0] + x[3] * y[1] */
        acc2 += dotpv2(VectInD, VectInC);

        /* acc3 +=  x[3] * y[0] + x[4] * y[1] */
        acc3 += dotpv2(VectInE, VectInC);

        /* Read y[2] and y[3] */
        a = *(py + 2);
        b = *(py + 3);
        VectInC[0] = a; /*y[srcBLen - 3]*/
        VectInC[1] = b; /*y[srcBLen - 4] */
        py += 4u;
	
        /* acc0 +=  x[2] * y[2] + x[3] * y[3] */
        acc0 += dotpv2(VectInD, VectInC);

        /* acc1 +=  x[3] * y[2] + x[4] * y[3] */
        acc1 += dotpv2(VectInE, VectInC);

        /* Read x[4], x[5], x[6] */
        a = *(px + 2);
        b = *(px + 3);

        VectInA[0] = a; /*x4*/
        VectInA[1] = b; /*x5*/
        a = *(px + 4);
        VectInB[0] = b; /*x5*/
        VectInB[1] = a; /*x6*/
        px += 4u;

        /* acc2 +=  x[4] * y[2] + x[5] * y[3] */
        acc2 += dotpv2(VectInA, VectInC);

        /* acc3 +=  x[5] * y[2] + x[6] * y[3] */
        acc3 += dotpv2(VectInB, VectInC);

      } while(--k);

      /* For the next MAC operations, SIMD is not used   
       * So, the 16 bit pointer if inputB, py is updated */

      /* If the srcBLen is not a multiple of 4, compute any remaining MACs here.   
       ** No loop unrolling is used. */
      k = srcBLen % 0x4u;

      if(k == 1u)
      {
        /* Read y[4] */
        VectInC[0] = VectInC[1]; 
        VectInC[1] = 0;
        /* Read x[7] */
        a = *px;
        b = *(px + 1);

        px++;;

        VectInD[0] = a; /*x6*/
        VectInD[1] = b; /*7*/
        px++;

        /* Perform the multiply-accumulates */
        acc0 += dotpv2(VectInA, VectInC);
        acc1 += dotpv2(VectInB, VectInC);
        acc2 += dotpv2(VectInB, VectInC);
        acc3 += dotpv2(VectInD, VectInC);
      }

      if(k == 2u)
      {
        /* Read y[4], y[5] */
        a = *py;
        b = *(py + 1);
        VectInC[0] = a; 
        VectInC[1] = b;
        /* Read x[7], x[8], x[9] */
        a = *px;
        b = *(px + 1);

        VectInE[0] = a; /*x7*/
        VectInE[1] = b; /*x8*/
        a = *(px + 2);
        VectInD[0] = b; /*x8*/
        VectInD[1] = a; /*x9*/
        px += 2u;

        /* Perform the multiply-accumulates */
        acc0 += dotpv2(VectInA, VectInC);
        acc1 += dotpv2(VectInB, VectInC);
        acc2 += dotpv2(VectInE, VectInC);
        acc3 += dotpv2(VectInD, VectInC);
      }

      if(k == 3u)
      {
        /* Read y[4], y[5] */
        a = *py;
        b = *(py + 1);

        VectInC[0] = a; 
        VectInC[1] = b;
        py += 2u;

        /* Read x[7], x[8], x[9] */
        a = *px;
        b = *(px + 1);

        VectInE[0] = a; /*x7*/
        VectInE[1] = b; /*x8*/
        a = *(px + 2);
        VectInD[0] = b; /*x8*/
        VectInD[1] = a; /*x9*/
        /* Perform the multiply-accumulates */
        acc0 += dotpv2(VectInA, VectInC);
        acc1 += dotpv2(VectInB, VectInC);
        acc2 += dotpv2(VectInE, VectInC);
        acc3 += dotpv2(VectInD, VectInC);

        VectInC[0] = VectInC[1]; 
        VectInC[1] = 0;

        /* Read x[10] */
        b = *(px + 3);

        VectInE[0] = a; /*x9*/
        VectInE[1] = b; /*x10*/
        px += 3u;

        /* Perform the multiply-accumulates */
        acc0 += dotpv2(VectInB, VectInC);
        acc1 += dotpv2(VectInD, VectInC);
        acc2 += dotpv2(VectInD, VectInC);
        acc3 += dotpv2(VectInE, VectInC);
      }

      /* Store the result in the accumulator in the destination buffer. */
      /**pOut = (q15_t) (acc0 >> 15);*/
      *pOut = (q15_t) __SSAT((acc0 >> 15u), 16u);
      /* Destination pointer is updated according to the address modifier, inc */
      pOut += inc;

     /**pOut = (q15_t) (acc1 >> 15);*/
      *pOut = (q15_t) __SSAT((acc1 >> 15u), 16u);
      pOut += inc;

     /* *pOut = (q15_t) (acc2 >> 15);*/
      *pOut = (q15_t) __SSAT((acc2 >> 15u), 16u);
      pOut += inc;

    /*  *pOut = (q15_t) (acc3 >> 15);*/
      *pOut = (q15_t) __SSAT((acc3 >> 15u), 16u);
      pOut += inc;

      /* Increment the pointer pIn1 index, count by 1 */
      count += 4u;

      /* Update the inputA and inputB pointers for next MAC calculation */
      px = pIn1 + count;
      py = pIn2;


      /* Decrement the loop counter */
      blkCnt--;
    }

    /* If the blockSize2 is not a multiple of 4, compute any remaining output samples here.   
     ** No loop unrolling is used. */
    blkCnt = blockSize2 % 0x4u;

    while(blkCnt > 0u)
    {
      /* Accumulator is made zero for every iteration */
      sum = 0;

      /* Apply loop unrolling and compute 4 MACs simultaneously. */
      k = srcBLen >> 2u;

      /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
       ** a second loop below computes MACs for the remaining 1 to 3 samples. */
      while(k > 0u)
      {
        /* Perform the multiply-accumulates */
        sum += ((q31_t) * px++ * *py++);
        sum += ((q31_t) * px++ * *py++);
        sum += ((q31_t) * px++ * *py++);
        sum += ((q31_t) * px++ * *py++);

        /* Decrement the loop counter */
        k--;
      }

      /* If the srcBLen is not a multiple of 4, compute any remaining MACs here.   
       ** No loop unrolling is used. */
      k = srcBLen % 0x4u;

      while(k > 0u)
      {
        /* Perform the multiply-accumulates */
        sum += ((q31_t) * px++ * *py++);

        /* Decrement the loop counter */
        k--;
      }

      /* Store the result in the accumulator in the destination buffer. */
      /**pOut = (q15_t) (sum >> 15);*/
      *pOut = (q15_t) __SSAT((sum >> 15u), 16u);
      /* Destination pointer is updated according to the address modifier, inc */
      pOut += inc;

      /* Increment the pointer pIn1 index, count by 1 */
      count++;

      /* Update the inputA and inputB pointers for next MAC calculation */
      px = pIn1 + count;
      py = pIn2;

      /* Decrement the loop counter */
      blkCnt--;
    }
  }
  else
  {
    /* If the srcBLen is not a multiple of 4,   
     * the blockSize2 loop cannot be unrolled by 4 */
    blkCnt = blockSize2;

    while(blkCnt > 0u)
    {
      /* Accumulator is made zero for every iteration */
      sum = 0;

      /* Loop over srcBLen */
      k = srcBLen;

      while(k > 0u)
      {
        /* Perform the multiply-accumulate */
        sum += ((q31_t) * px++ * *py++);

        /* Decrement the loop counter */
        k--;
      }

      /* Store the result in the accumulator in the destination buffer. */
      /**pOut = (q15_t) (sum >> 15);*/
      *pOut = (q15_t) __SSAT((sum >> 15u), 16u);
      /* Destination pointer is updated according to the address modifier, inc */
      pOut += inc;

      /* Increment the MAC count */
      count++;

      /* Update the inputA and inputB pointers for next MAC calculation */
      px = pIn1 + count;
      py = pIn2;

      /* Decrement the loop counter */
      blkCnt--;
    }
  }

  /* --------------------------   
   * Initializations of stage3   
   * -------------------------*/

  /* sum += x[srcALen-srcBLen+1] * y[0] + x[srcALen-srcBLen+2] * y[1] +...+ x[srcALen-1] * y[srcBLen-1]   
   * sum += x[srcALen-srcBLen+2] * y[0] + x[srcALen-srcBLen+3] * y[1] +...+ x[srcALen-1] * y[srcBLen-1]   
   * ....   
   * sum +=  x[srcALen-2] * y[0] + x[srcALen-1] * y[1]   
   * sum +=  x[srcALen-1] * y[0]   
   */

  /* In this stage the MAC operations are decreased by 1 for every iteration.   
     The count variable holds the number of MAC operations performed */
  count = srcBLen - 1u;

  /* Working pointer of inputA */
  pSrc1 = (pIn1 + srcALen) - (srcBLen - 1u);
  px = pSrc1;

  /* Working pointer of inputB */
  py = pIn2;

  /* -------------------   
   * Stage3 process   
   * ------------------*/

  while(blockSize3 > 0u)
  {
    /* Accumulator is made zero for every iteration */
    sum = 0;

    /* Apply loop unrolling and compute 4 MACs simultaneously. */
    k = count >> 2u;

    /* First part of the processing with loop unrolling.  Compute 4 MACs at a time.   
     ** a second loop below computes MACs for the remaining 1 to 3 samples. */
    while(k > 0u)
    {
      /* Perform the multiply-accumulates */
        sum += ((q31_t) * px++ * *py++);
        sum += ((q31_t) * px++ * *py++);
        sum += ((q31_t) * px++ * *py++);
        sum += ((q31_t) * px++ * *py++);

      /* Decrement the loop counter */
      k--;
    }

    /* If the count is not a multiple of 4, compute any remaining MACs here.   
     ** No loop unrolling is used. */
    k = count % 0x4u;

    while(k > 0u)
    {
      /* Perform the multiply-accumulates */
        sum += ((q31_t) * px++ * *py++);

      /* Decrement the loop counter */
      k--;
    }

    /* Store the result in the accumulator in the destination buffer. */
    /**pOut = (q15_t) (sum >> 15);*/
    *pOut = (q15_t) __SSAT((sum >> 15u), 16u);
    /* Destination pointer is updated according to the address modifier, inc */
    pOut += inc;

    /* Update the inputA and inputB pointers for next MAC calculation */
    px = ++pSrc1;
    py = pIn2;

    /* Decrement the MAC count */
    count--;

    /* Decrement the loop counter */
    blockSize3--;
  }

#else

  q15_t *pIn1 = pSrcA;                           /* inputA pointer               */
  q15_t *pIn2 = pSrcB + (srcBLen - 1u);          /* inputB pointer               */
  q63_t sum;                                     /* Accumulators                  */
  uint32_t i = 0u, j;                            /* loop counters */
  uint32_t inv = 0u;                             /* Reverse order flag */
  uint32_t tot = 0u;                             /* Length */

  /* The algorithm implementation is based on the lengths of the inputs. */
  /* srcB is always made to slide across srcA. */
  /* So srcBLen is always considered as shorter or equal to srcALen */
  /* But CORR(x, y) is reverse of CORR(y, x) */
  /* So, when srcBLen > srcALen, output pointer is made to point to the end of the output buffer */
  /* and a varaible, inv is set to 1 */
  /* If lengths are not equal then zero pad has to be done to  make the two   
   * inputs of same length. But to improve the performance, we include zeroes   
   * in the output instead of zero padding either of the the inputs*/
  /* If srcALen > srcBLen, (srcALen - srcBLen) zeroes has to included in the   
   * starting of the output buffer */
  /* If srcALen < srcBLen, (srcALen - srcBLen) zeroes has to included in the  
   * ending of the output buffer */
  /* Once the zero padding is done the remaining of the output is calcualted  
   * using convolution but with the shorter signal time shifted. */

  /* Calculate the length of the remaining sequence */
  tot = ((srcALen + srcBLen) - 2u);

  if(srcALen > srcBLen)
  {
    /* Calculating the number of zeros to be padded to the output */
    j = srcALen - srcBLen;

    /* Initialise the pointer after zero padding */
    pDst += j;
  }

  else if(srcALen < srcBLen)
  {
    /* Initialization to inputB pointer */
    pIn1 = pSrcB;

    /* Initialization to the end of inputA pointer */
    pIn2 = pSrcA + (srcALen - 1u);

    /* Initialisation of the pointer after zero padding */
    pDst = pDst + tot;

    /* Swapping the lengths */
    j = srcALen;
    srcALen = srcBLen;
    srcBLen = j;

    /* Setting the reverse flag */
    inv = 1;

  }

  /* Loop to calculate convolution for output length number of times */
  for (i = 0u; i <= tot; i++)
  {
    /* Initialize sum with zero to carry on MAC operations */
    sum = 0;

    /* Loop to perform MAC operations according to convolution equation */
    for (j = 0u; j <= i; j++)
    {
      /* Check the array limitations */
      if((((i - j) < srcBLen) && (j < srcALen)))
      {
        /* z[i] += x[i-j] * y[j] */
        sum += ((q31_t) pIn1[j] * pIn2[-((int32_t) i - j)]);
      }
    }
    /* Store the output in the destination buffer */
    if(inv == 1)
      *pDst-- = (q15_t) __SSAT((sum >> 15u), 16u);
    else
      *pDst++ = (q15_t) __SSAT((sum >> 15u), 16u);
  }
#endif

}

/**   
 * @} end of Corr group   
 */
