/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:	    arm_mat_mult_q15.c    
*    
* Description:	 Q15 matrix multiplication.    
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
 * @ingroup groupMatrix    
 */

/**    
 * @addtogroup MatrixMult    
 * @{    
 */


/**    
 * @brief Q15 matrix multiplication    
 * @param[in]       *pSrcA points to the first input matrix structure    
 * @param[in]       *pSrcB points to the second input matrix structure    
 * @param[out]      *pDst points to output matrix structure    
 * @param[in]		*pState points to the array for storing intermediate results (Unused)  
 * @return     		The function returns either    
 * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.    
 *    
 * @details    
 * <b>Scaling and Overflow Behavior:</b>    
 *    
 * \par    
 * The function is implemented using a 64-bit internal accumulator. The inputs to the    
 * multiplications are in 1.15 format and multiplications yield a 2.30 result.    
 * The 2.30 intermediate    
 * results are accumulated in a 64-bit accumulator in 34.30 format. This approach    
 * provides 33 guard bits and there is no risk of overflow. The 34.30 result is then    
 * truncated to 34.15 format by discarding the low 15 bits and then saturated to    
 * 1.15 format.    
 *    
 * \par    
 * Refer to <code>riscv_mat_mult_fast_q15()</code> for a faster but less precise version of this function for Cortex-M3 and Cortex-M4.    
 *    
 */

riscv_status riscv_mat_mult_q15(
  const riscv_matrix_instance_q15 * pSrcA,
  const riscv_matrix_instance_q15 * pSrcB,
  riscv_matrix_instance_q15 * pDst,
  q15_t * pState)
{
  q63_t sum;                                     /* accumulator */

#if defined (USE_DSP_RISCV)


  q15_t *pSrcBT = pState;                        /* input data matrix pointer for transpose */
  q15_t *pInA = pSrcA->pData;                    /* input data matrix pointer A of Q15 type */
  q15_t *pInB = pSrcB->pData;                    /* input data matrix pointer B of Q15 type */
  q15_t *px;                                     /* Temporary output data matrix pointer */
  uint16_t numRowsA = pSrcA->numRows;            /* number of rows of input matrix A    */
  uint16_t numColsB = pSrcB->numCols;            /* number of columns of input matrix B */
  uint16_t numColsA = pSrcA->numCols;            /* number of columns of input matrix A */
  uint16_t numRowsB = pSrcB->numRows;            /* number of rows of input matrix A    */
  uint16_t col, i = 0u, row = numRowsB, colCnt;  /* loop counters */
  riscv_status status;                             /* status of matrix multiplication */



  q15_t in;                                      /* Temporary variable to hold the input value */
  q15_t inA1, inB1, inA2, inB2;

  shortV *VectInA;
  shortV *VectInB;


#ifdef RISCV_MATH_MATRIX_CHECK
  /* Check for matrix mismatch condition */
  if((pSrcA->numCols != pSrcB->numRows) ||
     (pSrcA->numRows != pDst->numRows) || (pSrcB->numCols != pDst->numCols))
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else
#endif /*    #ifdef RISCV_MATH_MATRIX_CHECK    */
  {
    /* Matrix transpose */
    do
    {
      /* Apply loop unrolling and exchange the columns with row elements */
      col = numColsB >> 2;

      /* The pointer px is set to starting address of the column being processed */
      px = pSrcBT + i;

      /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.        
       ** a second loop below computes the remaining 1 to 3 samples. */
      while(col > 0u)
      {

        /* Read one element from the row */
        in = *pInB++;

        /* Store one element in the destination */
        *px = in;
 
        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

        /* Read one element from the row */
        in = *pInB++;

        /* Store one element in the destination */
        *px = in;
 
        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

        /* Read one element from the row */
        in = *pInB++;

        /* Store one element in the destination */
        *px = in;
 
        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

        /* Read one element from the row */
        in = *pInB++;

        /* Store one element in the destination */
        *px = in;
 
        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

       /* Decrement the column loop counter */
        col--;
      }

      /* If the columns of pSrcB is not a multiple of 4, compute any remaining output samples here.        
       ** No loop unrolling is used. */
      col = numColsB % 0x4u;

      while(col > 0u)
      {
        /* Read and store the input element in the destination */
        *px = *pInB++;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB;

        /* Decrement the column loop counter */
        col--;
      }

      i++;

      /* Decrement the row loop counter */
      row--;

    } while(row > 0u);

    /* Reset the variables for the usage in the following multiplication process */
    row = numRowsA;
    i = 0u;
    px = pDst->pData;

    /* The following loop performs the dot-product of each row in pSrcA with each column in pSrcB */
    /* row loop */
    do
    {
      /* For every row wise process, the column loop counter is to be initiated */
      col = numColsB;

      /* For every row wise process, the pIn2 pointer is set        
       ** to the starting address of the transposed pSrcB data */
      pInB = pSrcBT;

      /* column loop */
      do
      {
        /* Set the variable sum, that acts as accumulator, to zero */
        sum = 0;

        /* Apply loop unrolling and compute 2 MACs simultaneously. */
        colCnt = numColsA >> 2;

        /* Initiate the pointer pIn1 to point to the starting address of the column being processed */
        pInA = pSrcA->pData + i;


        /* matrix multiplication */
        while(colCnt > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
          /*inA1 = *pInA++;
          inB1 = *pInB++;
          inA2 = *pInA++;
          inB2 = *pInB++;*/

          VectInA = (shortV*)pInA;
          VectInB = (shortV*)pInB;


          sum += dotpv2(*VectInA, *VectInB);
          VectInA = (shortV*)(pInA+2);
          VectInB = (shortV*)(pInB+2);

          pInA+=4;
          pInB+=4;
          sum += dotpv2(*VectInA, *VectInB);

          /* Decrement the loop counter */
          colCnt--;
        }

        /* process remaining column samples */
        colCnt = numColsA & 3u;

        while(colCnt > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
          sum += *pInA++ * *pInB++;

          /* Decrement the loop counter */
          colCnt--;
        }

        /* Saturate and store the result in the destination buffer */
        *px = (q15_t) (clip((sum >> 15), -32768,32767));
        px++;

        /* Decrement the column loop counter */
        col--;

      } while(col > 0u);

      i = i + numColsA;

      /* Decrement the row loop counter */
      row--;

    } while(row > 0u);

#else


  q15_t *pIn1 = pSrcA->pData;                    /* input data matrix pointer A */
  q15_t *pIn2 = pSrcB->pData;                    /* input data matrix pointer B */
  q15_t *pInA = pSrcA->pData;                    /* input data matrix pointer A of Q15 type */
  q15_t *pInB = pSrcB->pData;                    /* input data matrix pointer B of Q15 type */
  q15_t *pOut = pDst->pData;                     /* output data matrix pointer */
  q15_t *px;                                     /* Temporary output data matrix pointer */
  uint16_t numColsB = pSrcB->numCols;            /* number of columns of input matrix B */
  uint16_t numColsA = pSrcA->numCols;            /* number of columns of input matrix A */
  uint16_t numRowsA = pSrcA->numRows;            /* number of rows of input matrix A    */
  uint16_t col, i = 0u, row = numRowsA, colCnt;  /* loop counters */
  riscv_status status;                             /* status of matrix multiplication */

#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if((pSrcA->numCols != pSrcB->numRows) ||
     (pSrcA->numRows != pDst->numRows) || (pSrcB->numCols != pDst->numCols))
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else
#endif /*    #ifdef RISCV_MATH_MATRIX_CHECK    */

  {
    /* The following loop performs the dot-product of each row in pSrcA with each column in pSrcB */
    /* row loop */
    do
    {
      /* Output pointer is set to starting address of the row being processed */
      px = pOut + i;

      /* For every row wise process, the column loop counter is to be initiated */
      col = numColsB;

      /* For every row wise process, the pIn2 pointer is set          
       ** to the starting address of the pSrcB data */
      pIn2 = pSrcB->pData;

      /* column loop */
      do
      {
        /* Set the variable sum, that acts as accumulator, to zero */
        sum = 0;

        /* Initiate the pointer pIn1 to point to the starting address of pSrcA */
        pIn1 = pInA;

        /* Matrix A columns number of MAC operations are to be performed */
        colCnt = numColsA;

        /* matrix multiplication */
        while(colCnt > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */
          /* Perform the multiply-accumulates */
          sum += (q31_t) * pIn1++ * *pIn2;
          pIn2 += numColsB;

          /* Decrement the loop counter */
          colCnt--;
        }

        /* Convert the result from 34.30 to 1.15 format and store the saturated value in destination buffer */
        /* Saturate and store the result in the destination buffer */
        *px++ = (q15_t) __SSAT((sum >> 15), 16);

        /* Decrement the column loop counter */
        col--;

        /* Update the pointer pIn2 to point to the  starting address of the next column */
        pIn2 = pInB + (numColsB - col);

      } while(col > 0u);

      /* Update the pointer pSrcA to point to the  starting address of the next row */
      i = i + numColsB;
      pInA = pInA + numColsA;

      /* Decrement the row loop counter */
      row--;

    } while(row > 0u);

#endif
    /* set status as RISCV_MATH_SUCCESS */
    status = RISCV_MATH_SUCCESS;
  }

  /* Return to application */
  return (status);
}

/**        
 * @} end of MatrixMult group        
 */
