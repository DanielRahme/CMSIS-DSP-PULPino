/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015 
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:	    arm_biquad_cascade_df2T_f64.c    
*    
* Description:  Processing function for the floating-point transposed    
*               direct form II Biquad cascade filter.   
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
* @defgroup BiquadCascadeDF2T Biquad Cascade IIR Filters Using a Direct Form II Transposed Structure       
*       
* This set of functions implements arbitrary order recursive (IIR) filters using a transposed direct form II structure.       
* The filters are implemented as a cascade of second order Biquad sections.       
* These functions provide a slight memory savings as compared to the direct form I Biquad filter functions.      
* Only floating-point data is supported.       
*       
* This function operate on blocks of input and output data and each call to the function       
* processes <code>blockSize</code> samples through the filter.       
* <code>pSrc</code> points to the array of input data and       
* <code>pDst</code> points to the array of output data.       
* Both arrays contain <code>blockSize</code> values.       
*       
* \par Algorithm       
* Each Biquad stage implements a second order filter using the difference equation:       
* <pre>       
*    y[n] = b0 * x[n] + d1       
*    d1 = b1 * x[n] + a1 * y[n] + d2       
*    d2 = b2 * x[n] + a2 * y[n]       
* </pre>       
* where d1 and d2 represent the two state values.       
*       
* \par       
* A Biquad filter using a transposed Direct Form II structure is shown below.       
* \image html BiquadDF2Transposed.gif "Single transposed Direct Form II Biquad"       
* Coefficients <code>b0, b1, and b2 </code> multiply the input signal <code>x[n]</code> and are referred to as the feedforward coefficients.       
* Coefficients <code>a1</code> and <code>a2</code> multiply the output signal <code>y[n]</code> and are referred to as the feedback coefficients.       
* Pay careful attention to the sign of the feedback coefficients.       
* Some design tools flip the sign of the feedback coefficients:       
* <pre>       
*    y[n] = b0 * x[n] + d1;       
*    d1 = b1 * x[n] - a1 * y[n] + d2;       
*    d2 = b2 * x[n] - a2 * y[n];       
* </pre>       
* In this case the feedback coefficients <code>a1</code> and <code>a2</code> must be negated when used with the CMSIS DSP Library.       
*       
* \par       
* Higher order filters are realized as a cascade of second order sections.       
* <code>numStages</code> refers to the number of second order stages used.       
* For example, an 8th order filter would be realized with <code>numStages=4</code> second order stages.       
* A 9th order filter would be realized with <code>numStages=5</code> second order stages with the       
* coefficients for one of the stages configured as a first order filter (<code>b2=0</code> and <code>a2=0</code>).       
*       
* \par       
* <code>pState</code> points to the state variable array.       
* Each Biquad stage has 2 state variables <code>d1</code> and <code>d2</code>.       
* The state variables are arranged in the <code>pState</code> array as:       
* <pre>       
*     {d11, d12, d21, d22, ...}       
* </pre>       
* where <code>d1x</code> refers to the state variables for the first Biquad and       
* <code>d2x</code> refers to the state variables for the second Biquad.       
* The state array has a total length of <code>2*numStages</code> values.       
* The state variables are updated after each block of data is processed; the coefficients are untouched.       
*       
* \par       
* The CMSIS library contains Biquad filters in both Direct Form I and transposed Direct Form II.    
* The advantage of the Direct Form I structure is that it is numerically more robust for fixed-point data types.    
* That is why the Direct Form I structure supports Q15 and Q31 data types.    
* The transposed Direct Form II structure, on the other hand, requires a wide dynamic range for the state variables <code>d1</code> and <code>d2</code>.    
* Because of this, the CMSIS library only has a floating-point version of the Direct Form II Biquad.    
* The advantage of the Direct Form II Biquad is that it requires half the number of state variables, 2 rather than 4, per Biquad stage.    
*       
* \par Instance Structure       
* The coefficients and state variables for a filter are stored together in an instance data structure.       
* A separate instance structure must be defined for each filter.       
* Coefficient arrays may be shared among several instances while state variable arrays cannot be shared.       
*       
* \par Init Functions       
* There is also an associated initialization function.      
* The initialization function performs following operations:       
* - Sets the values of the internal structure fields.       
* - Zeros out the values in the state buffer.       
* To do this manually without calling the init function, assign the follow subfields of the instance structure:
* numStages, pCoeffs, pState. Also set all of the values in pState to zero. 
*       
* \par       
* Use of the initialization function is optional.       
* However, if the initialization function is used, then the instance structure cannot be placed into a const data section.       
* To place an instance structure into a const data section, the instance structure must be manually initialized.       
* Set the values in the state buffer to zeros before static initialization.       
* For example, to statically initialize the instance structure use       
* <pre>       
*     riscv_biquad_cascade_df2T_instance_f64 S1 = {numStages, pState, pCoeffs};       
* </pre>       
* where <code>numStages</code> is the number of Biquad stages in the filter; <code>pState</code> is the address of the state buffer.       
* <code>pCoeffs</code> is the address of the coefficient buffer;        
*       
*/

/**       
* @addtogroup BiquadCascadeDF2T       
* @{       
*/

/**      
* @brief Processing function for the floating-point transposed direct form II Biquad cascade filter.      
* @param[in]  *S        points to an instance of the filter data structure.      
* @param[in]  *pSrc     points to the block of input data.      
* @param[out] *pDst     points to the block of output data      
* @param[in]  blockSize number of samples to process.      
* @return none.      
*/


void riscv_biquad_cascade_df2T_f64(
const riscv_biquad_cascade_df2T_instance_f64 * S,
float64_t * pSrc,
float64_t * pDst,
uint32_t blockSize)
{

   float64_t *pIn = pSrc;                         /*  source pointer            */
   float64_t *pOut = pDst;                        /*  destination pointer       */
   float64_t *pState = S->pState;                 /*  State pointer             */
   float64_t *pCoeffs = S->pCoeffs;               /*  coefficient pointer       */
   float64_t acc1;                                /*  accumulator               */
   float64_t b0, b1, b2, a1, a2;                  /*  Filter coefficients       */
   float64_t Xn1;                                 /*  temporary input           */
   float64_t d1, d2;                              /*  state variables           */
   uint32_t sample, stage = S->numStages;         /*  loop counters             */

   /* Run the below code for Cortex-M0 */

   do
   {
      /* Reading the coefficients */
      b0 = *pCoeffs++;
      b1 = *pCoeffs++;
      b2 = *pCoeffs++;
      a1 = *pCoeffs++;
      a2 = *pCoeffs++;

      /*Reading the state values */
      d1 = pState[0];
      d2 = pState[1];


      sample = blockSize;

      while(sample > 0u)
      {
         /* Read the input */
         Xn1 = *pIn++;

         /* y[n] = b0 * x[n] + d1 */
         acc1 = (b0 * Xn1) + d1;

         /* Store the result in the accumulator in the destination buffer. */
         *pOut++ = acc1;

         /* Every time after the output is computed state should be updated. */
         /* d1 = b1 * x[n] + a1 * y[n] + d2 */
         d1 = ((b1 * Xn1) + (a1 * acc1)) + d2;

         /* d2 = b2 * x[n] + a2 * y[n] */
         d2 = (b2 * Xn1) + (a2 * acc1);

         /* decrement the loop counter */
         sample--;
      }

      /* Store the updated state variables back into the state array */
      *pState++ = d1;
      *pState++ = d2;

      /* The current stage input is given as the output to the next stage */
      pIn = pDst;

      /*Reset the output working pointer */
      pOut = pDst;

      /* decrement the loop counter */
      stage--;

   } while(stage > 0u);
	 


}
//LOW_OPTIMIZATION_EXIT

/**       
   * @} end of BiquadCascadeDF2T group       
   */
