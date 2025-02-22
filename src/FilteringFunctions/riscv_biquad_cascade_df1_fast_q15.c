/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:	    arm_biquad_cascade_df1_fast_q15.c    
*    
* Description:	Fast processing function for the    
*				Q15 Biquad cascade filter.    
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
 * @addtogroup BiquadCascadeDF1    
 * @{    
 */

/**    
 * @details    
 * @param[in]  *S points to an instance of the Q15 Biquad cascade structure.    
 * @param[in]  *pSrc points to the block of input data.    
 * @param[out] *pDst points to the block of output data.    
 * @param[in]  blockSize number of samples to process per call.    
 * @return none.    
 *    
 * <b>Scaling and Overflow Behavior:</b>    
 * \par    
 * This fast version uses a 32-bit accumulator with 2.30 format.    
 * The accumulator maintains full precision of the intermediate multiplication results but provides only a single guard bit.    
 * Thus, if the accumulator result overflows it wraps around and distorts the result.    
 * In order to avoid overflows completely the input signal must be scaled down by two bits and lie in the range [-0.25 +0.25).    
 * The 2.30 accumulator is then shifted by <code>postShift</code> bits and the result truncated to 1.15 format by discarding the low 16 bits.    
 *    
 * \par    
 * Refer to the function <code>riscv_biquad_cascade_df1_q15()</code> for a slower implementation of this function which uses 64-bit accumulation to avoid wrap around distortion.  Both the slow and the fast versions use the same instance structure.    
 * Use the function <code>riscv_biquad_cascade_df1_init_q15()</code> to initialize the filter structure.    
 *    
 */

void riscv_biquad_cascade_df1_fast_q15(
  const riscv_biquad_casd_df1_inst_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize)
{
  q15_t *pIn = pSrc;                             /*  Source pointer                               */
  q15_t *pOut = pDst;                            /*  Destination pointer                          */
  q31_t in;                                      /*  Temporary variable to hold input value       */
  q31_t out;                                     /*  Temporary variable to hold output value      */
  q31_t b0;                                      /*  Temporary variable to hold bo value          */
  q31_t b1, a1;                                  /*  Filter coefficients                          */
  q31_t state_in, state_out;                     /*  Filter state variables                       */
  q31_t acc = 0;                                     /*  Accumulator                                  */
  int32_t shift = (int32_t) (15 - S->postShift); /*  Post shift                                   */
  q15_t *pState = S->pState;                     /*  State pointer                                */
  q15_t *pCoeffs = S->pCoeffs;                   /*  Coefficient pointer                          */
  uint32_t sample, stage = S->numStages;         /*  Stage loop counter                           */
  shortV *VectInA;
  shortV *VectInB;
  shortV *VectInC;
  shortV *VectInD;
  q15_t Xn;   

  do
  {
    /* Reading the coefficients */
    b0 = *pCoeffs++;
    pCoeffs++;  // skip the 0 coefficient
    VectInA = (shortV*)pCoeffs; /*b1 b2*/
    pCoeffs+=2;
    VectInB = (shortV*)pCoeffs; /*a1 a2*/
    pCoeffs+=2;
    /* Reading the state values */
    VectInC = (shortV*)pState; /*Xn1 Xn2*/
    VectInD = (shortV*)(pState+2); /*Yn1 Yn2*/

    /*      The variables acc holds the output value that is computed:         
     *    acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]         
     */

    sample = blockSize;

    while(sample > 0u)
    {
      /* Read the input */
      Xn = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      /* acc =  b0 * x[n] */
      acc = (q31_t) b0 *Xn;
      acc = sumdotpv2(*VectInA,*VectInC,acc);
      acc = sumdotpv2(*VectInB,*VectInD,acc);
      acc = clip((acc >> shift), -32768,32767 );

      /* Every time after the output is computed state should be updated. */
      /* The states should be updated as:  */
      /* Xn2 = Xn1    */
      /* Xn1 = Xn     */
      /* Yn2 = Yn1    */
      /* Yn1 = acc    */
      (*VectInC) = pack2(Xn,(*VectInC)[0]);
      (*VectInD) = pack2(acc,(*VectInD)[0]);

      /* Store the output in the destination buffer. */
      *pOut++ = (q15_t) acc;

      /* decrement the loop counter */
      sample--;
    }

    /*  The first stage goes from the input buffer to the output buffer. */
    /*  Subsequent stages occur in-place in the output buffer */
    pIn = pDst;

    /* Reset to destination pointer */
    pOut = pDst;

    /*  Store the updated state variables back into the pState array */
    *(shortV*)pState = *VectInC;
    pState+=2;
    *(shortV*)pState = *VectInD;
    pState+=2;

  } while(--stage);



}


/**    
 * @} end of BiquadCascadeDF1 group    
 */
