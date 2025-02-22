/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:        arm_fir_q15.c    
*    
* Description:  Q15 FIR filter processing function.    
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
 * @addtogroup FIR       
 * @{       
 */

/**       
 * @brief Processing function for the Q15 FIR filter.       
 * @param[in] *S points to an instance of the Q15 FIR structure.       
 * @param[in] *pSrc points to the block of input data.       
 * @param[out] *pDst points to the block of output data.       
 * @param[in]  blockSize number of samples to process per call.       
 * @return none.       
 *   
 *   
 * \par Restrictions   
 *  If the silicon does not support unaligned memory access enable the macro UNALIGNED_SUPPORT_DISABLE   
 *	In this case input, output, state buffers should be aligned by 32-bit   
 *   
 * <b>Scaling and Overflow Behavior:</b>       
 * \par       
 * The function is implemented using a 64-bit internal accumulator.       
 * Both coefficients and state variables are represented in 1.15 format and multiplications yield a 2.30 result.       
 * The 2.30 intermediate results are accumulated in a 64-bit accumulator in 34.30 format.       
 * There is no risk of internal overflow with this approach and the full precision of intermediate multiplications is preserved.       
 * After all additions have been performed, the accumulator is truncated to 34.15 format by discarding low 15 bits.       
 * Lastly, the accumulator is saturated to yield a result in 1.15 format.       
 *       
 * \par       
 * Refer to the function <code>riscv_fir_fast_q15()</code> for a faster but less precise implementation of this function.       
 */


void riscv_fir_q15(
  const riscv_fir_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize)
{

#if defined (USE_DSP_RISCV)

  q15_t *pState = S->pState;                     /* State pointer */
  q15_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
  q15_t *pStateCurnt;                            /* Points to the current sample of the state */
  q15_t *px1;                                    /* Temporary q15 pointer for state buffer */
  q15_t *pb;                                     /* Temporary pointer for coefficient buffer */
  q31_t x0, x1, x2, x3, c0;                      /* Temporary variables to hold SIMD state and coefficient values */
  q63_t acc0, acc1, acc2, acc3;                  /* Accumulators */
  uint32_t numTaps = S->numTaps;                 /* Number of taps in the filter */
  uint32_t tapCnt, blkCnt;                       /* Loop counters */
  shortV *VectInA = NULL; /*compiler generates unreasonable  warning here, although it is initialized later*/
  shortV *VectInB;
  shortV *VectInC;
  shortV *VectInD;
  shortV *VectInE;
  /* S->pState points to state array which contains previous frame (numTaps - 1) samples */
  /* pStateCurnt points to the location where the new input data should be written */
  pStateCurnt = &(S->pState[(numTaps - 1u)]);

  /* Apply loop unrolling and compute 4 output values simultaneously.       
   * The variables acc0 ... acc3 hold output values that are being computed:       
   *       
   *    acc0 =  b[numTaps-1] * x[n-numTaps-1] + b[numTaps-2] * x[n-numTaps-2] + b[numTaps-3] * x[n-numTaps-3] +...+ b[0] * x[0]       
   *    acc1 =  b[numTaps-1] * x[n-numTaps] +   b[numTaps-2] * x[n-numTaps-1] + b[numTaps-3] * x[n-numTaps-2] +...+ b[0] * x[1]       
   *    acc2 =  b[numTaps-1] * x[n-numTaps+1] + b[numTaps-2] * x[n-numTaps] +   b[numTaps-3] * x[n-numTaps-1] +...+ b[0] * x[2]       
   *    acc3 =  b[numTaps-1] * x[n-numTaps+2] + b[numTaps-2] * x[n-numTaps+1] + b[numTaps-3] * x[n-numTaps]   +...+ b[0] * x[3]       
   */

  blkCnt = blockSize >> 2;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.       
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* Copy four new input samples into the state buffer.       
     ** Use 32-bit SIMD to move the 16-bit data.  Only requires two copies. */
    *(shortV*)pStateCurnt =*(shortV*)pSrc;
    pStateCurnt+=2;
    pSrc+=2;
    *(shortV*)pStateCurnt =*(shortV*)pSrc;
    pStateCurnt+=2;
    pSrc+=2;

    /* Set all accumulators to zero */
    acc0 = 0;
    acc1 = 0;
    acc2 = 0;
    acc3 = 0;

    /* Initialize state pointer of type q15 */
    px1 = pState;

    /* Initialize coeff pointer of type q31 */
    pb = pCoeffs;


    /* Read the first two samples from the state buffer:  x[n-N], x[n-N-1] */
    VectInA = (shortV*)(px1);
    /* Read the third and forth samples from the state buffer: x[n-N-1], x[n-N-2] */
    VectInB = (shortV*)(px1+1u);
    px1 += 2u;

    /* Loop over the number of taps.  Unroll by a factor of 4.       
     ** Repeat until we've computed numTaps-4 coefficients. */
    tapCnt = numTaps >> 2;

    while(tapCnt > 0u)
    {
      /* Read the first two coefficients using SIMD:  b[N] and b[N-1] coefficients */
      VectInC = (shortV*)pb;
      pb+=2;
      /* acc0 +=  b[N] * x[n-N] + b[N-1] * x[n-N-1] */
      acc0 += dotpv2(*VectInA,*VectInC);
      /* acc1 +=  b[N] * x[n-N-1] + b[N-1] * x[n-N-2] */
      acc1 += dotpv2(*VectInB,*VectInC);
      /* Read state x[n-N-2], x[n-N-3] */
      VectInD = (shortV*)px1; 
      /* Read state x[n-N-3], x[n-N-4] */
      VectInE = (shortV*)(px1+1u);
      /* acc2 +=  b[N] * x[n-N-2] + b[N-1] * x[n-N-3] */
      acc2 += dotpv2(*VectInD,*VectInC);
      /* acc3 +=  b[N] * x[n-N-3] + b[N-1] * x[n-N-4] */
      acc3 += dotpv2(*VectInE,*VectInC);
      /* Read coefficients b[N-2], b[N-3] */
      VectInC = (shortV*)pb;
      pb+=2;
      /* acc0 +=  b[N-2] * x[n-N-2] + b[N-3] * x[n-N-3] */
      acc0 += dotpv2(*VectInD,*VectInC);
      /* acc1 +=  b[N-2] * x[n-N-3] + b[N-3] * x[n-N-4] */
      acc1 += dotpv2(*VectInE,*VectInC);
      /* Read state x[n-N-4], x[n-N-5] */
      VectInA = (shortV*)(px1+2u);
      /* Read state x[n-N-5], x[n-N-6] */
      VectInB = (shortV*)(px1+3u);
      /* acc2 +=  b[N-2] * x[n-N-4] + b[N-3] * x[n-N-5] */
      acc2 += dotpv2(*VectInA,*VectInC);
      /* acc3 +=  b[N-2] * x[n-N-5] + b[N-3] * x[n-N-6] */
      acc3 += dotpv2(*VectInB,*VectInC);
      px1 += 4u;

      tapCnt--;

    }


    /* If the filter length is not a multiple of 4, compute the remaining filter taps.       
     ** This is always be 2 taps since the filter length is even. */
    if((numTaps & 0x3u) != 0u)
    {
      /* Read 2 coefficients */
      VectInC = (shortV*)pb;
      pb+=2;
      /* Fetch 4 state variables */
      VectInD = (shortV*)px1;
      VectInE = (shortV*)(px1+1u);
      /* Perform the multiply-accumulates */
      acc0 += dotpv2(*VectInA,*VectInC);
      px1 += 2u;

      acc1 += dotpv2(*VectInB,*VectInC);
      acc2 += dotpv2(*VectInD,*VectInC);
      acc3 += dotpv2(*VectInE,*VectInC);
    }

    /* The results in the 4 accumulators are in 2.30 format.  Convert to 1.15 with saturation.       
     ** Then store the 4 outputs in the destination buffer. */
   

    *pDst++ = (q15_t) (clip((acc0 >> 15), -32768,32767));
    *pDst++ = (q15_t) (clip((acc1 >> 15), -32768,32767));
    *pDst++ = (q15_t) (clip((acc2 >> 15), -32768,32767));
    *pDst++ = (q15_t) (clip((acc3 >> 15), -32768,32767));

    /* Advance the state pointer by 4 to process the next group of 4 samples */
    pState = pState + 4;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.       
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;
  while(blkCnt > 0u)
  {
    /* Copy two samples into state buffer */
    *pStateCurnt++ = *pSrc++;

    /* Set the accumulator to zero */
    acc0 = 0;

    /* Initialize state pointer of type q15 */
    px1 = pState;

    /* Initialize coeff pointer of type q31 */
    pb = pCoeffs;

    tapCnt = numTaps >> 1;

    do
    {
      VectInC = (shortV*)pb;
      pb+=2;
      VectInD = (shortV*)px1;
      px1+=2;
      acc0 += dotpv2(*VectInA,*VectInC);
      tapCnt--;
    }
    while(tapCnt > 0u);

    /* The result is in 2.30 format.  Convert to 1.15 with saturation.       
     ** Then store the output in the destination buffer. */
    *pDst++ = (q15_t) (__SSAT((acc0 >> 15), 16));

    /* Advance state pointer by 1 for the next sample */
    pState = pState + 1;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Processing is complete.       
   ** Now copy the last numTaps - 1 samples to the satrt of the state buffer.       
   ** This prepares the state buffer for the next function call. */

  /* Points to the start of the state buffer */
  pStateCurnt = S->pState;

  /* Calculation of count for copying integer writes */
  tapCnt = (numTaps - 1u) >> 2;

  while(tapCnt > 0u)
  {

    /* Copy state values to start of state buffer */
    *(shortV*)pStateCurnt =*(shortV*)pState;
    pStateCurnt+=2;
    pSrc+=2;
    *(shortV*)pStateCurnt =*(shortV*)pState;
    pStateCurnt+=2;
    pSrc+=2;
    tapCnt--;

  }

  /* Calculation of count for remaining q15_t data */
  tapCnt = (numTaps - 1u) % 0x4u;

  /* copy remaining data */
  while(tapCnt > 0u)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement the loop counter */
    tapCnt--;
  }

#else

  q15_t *pState = S->pState;                     /* State pointer */
  q15_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
  q15_t *pStateCurnt;                            /* Points to the current sample of the state */



  q15_t *px;                                     /* Temporary pointer for state buffer */
  q15_t *pb;                                     /* Temporary pointer for coefficient buffer */
  q63_t acc;                                     /* Accumulator */
  uint32_t numTaps = S->numTaps;                 /* Number of nTaps in the filter */
  uint32_t tapCnt, blkCnt;                       /* Loop counters */

  /* S->pState buffer contains previous frame (numTaps - 1) samples */
  /* pStateCurnt points to the location where the new input data should be written */
  pStateCurnt = &(S->pState[(numTaps - 1u)]);

  /* Initialize blkCnt with blockSize */
  blkCnt = blockSize;

  while(blkCnt > 0u)
  {
    /* Copy one sample at a time into state buffer */
    *pStateCurnt++ = *pSrc++;

    /* Set the accumulator to zero */
    acc = 0;

    /* Initialize state pointer */
    px = pState;

    /* Initialize Coefficient pointer */
    pb = pCoeffs;

    tapCnt = numTaps;

    /* Perform the multiply-accumulates */
    do
    {
      /* acc =  b[numTaps-1] * x[n-numTaps-1] + b[numTaps-2] * x[n-numTaps-2] + b[numTaps-3] * x[n-numTaps-3] +...+ b[0] * x[0] */
      acc += (q31_t) * px++ * *pb++;
      tapCnt--;
    } while(tapCnt > 0u);

    /* The result is in 2.30 format.  Convert to 1.15         
     ** Then store the output in the destination buffer. */
    *pDst++ = (q15_t) __SSAT((acc >> 15u), 16);

    /* Advance state pointer by 1 for the next sample */
    pState = pState + 1;

    /* Decrement the samples loop counter */
    blkCnt--;
  }

  /* Processing is complete.         
   ** Now copy the last numTaps - 1 samples to the satrt of the state buffer.       
   ** This prepares the state buffer for the next function call. */

  /* Points to the start of the state buffer */
  pStateCurnt = S->pState;

  /* Copy numTaps number of values */
  tapCnt = (numTaps - 1u);

  /* copy data */
  while(tapCnt > 0u)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement the loop counter */
    tapCnt--;
  }

#endif
}





/**       
 * @} end of FIR group       
 */
