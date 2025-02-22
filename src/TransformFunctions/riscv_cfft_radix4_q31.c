/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015 
* $Revision: 	V.1.4.5  
*    
* Project: 	    CMSIS DSP Library    
* Title:	    arm_cfft_radix4_q31.c    
*    
* Description:	This file has function definition of Radix-4 FFT & IFFT function and    
*				In-place bit reversal using bit reversal table    
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

void riscv_radix4_butterfly_inverse_q31(
q31_t * pSrc,
uint32_t fftLen,
q31_t * pCoef,
uint32_t twidCoefModifier);

void riscv_radix4_butterfly_q31(
q31_t * pSrc,
uint32_t fftLen,
q31_t * pCoef,
uint32_t twidCoefModifier);

void riscv_bitreversal_q31(
q31_t * pSrc,
uint32_t fftLen,
uint16_t bitRevFactor,
uint16_t * pBitRevTab);

/**    
 * @ingroup groupTransforms    
 */

/**    
 * @addtogroup ComplexFFT    
 * @{    
 */

/**    
 * @details    
 * @brief Processing function for the Q31 CFFT/CIFFT.    
 * @deprecated Do not use this function.  It has been superseded by \ref riscv_cfft_q31 and will be removed
 * @param[in]      *S    points to an instance of the Q31 CFFT/CIFFT structure.   
 * @param[in, out] *pSrc points to the complex data buffer of size <code>2*fftLen</code>. Processing occurs in-place.   
 * @return none.    
 *     
 * \par Input and output formats:    
 * \par    
 * Internally input is downscaled by 2 for every stage to avoid saturations inside CFFT/CIFFT process.   
 * Hence the output format is different for different FFT sizes.    
 * The input and output formats for different FFT sizes and number of bits to upscale are mentioned in the tables below for CFFT and CIFFT:   
 * \par   
 * \image html CFFTQ31.gif "Input and Output Formats for Q31 CFFT"    
 * \image html CIFFTQ31.gif "Input and Output Formats for Q31 CIFFT"    
 *    
 */

void riscv_cfft_radix4_q31(
  const riscv_cfft_radix4_instance_q31 * S,
  q31_t * pSrc)
{
  if(S->ifftFlag == 1u)
  {
    /* Complex IFFT radix-4 */
    riscv_radix4_butterfly_inverse_q31(pSrc, S->fftLen, S->pTwiddle,
                                     S->twidCoefModifier);
  }
  else
  {
    /* Complex FFT radix-4 */
    riscv_radix4_butterfly_q31(pSrc, S->fftLen, S->pTwiddle,
                             S->twidCoefModifier);
  }


  if(S->bitReverseFlag == 1u)
  {
    /*  Bit Reversal */
    riscv_bitreversal_q31(pSrc, S->fftLen, S->bitRevFactor, S->pBitRevTable);
  }

}

/**    
 * @} end of ComplexFFT group    
 */

/*    
* Radix-4 FFT algorithm used is :    
*    
* Input real and imaginary data:    
* x(n) = xa + j * ya    
* x(n+N/4 ) = xb + j * yb    
* x(n+N/2 ) = xc + j * yc    
* x(n+3N 4) = xd + j * yd    
*    
*    
* Output real and imaginary data:    
* x(4r) = xa'+ j * ya'    
* x(4r+1) = xb'+ j * yb'    
* x(4r+2) = xc'+ j * yc'    
* x(4r+3) = xd'+ j * yd'    
*    
*    
* Twiddle factors for radix-4 FFT:    
* Wn = co1 + j * (- si1)    
* W2n = co2 + j * (- si2)    
* W3n = co3 + j * (- si3)    
*    
*  Butterfly implementation:    
* xa' = xa + xb + xc + xd    
* ya' = ya + yb + yc + yd    
* xb' = (xa+yb-xc-yd)* co1 + (ya-xb-yc+xd)* (si1)    
* yb' = (ya-xb-yc+xd)* co1 - (xa+yb-xc-yd)* (si1)    
* xc' = (xa-xb+xc-xd)* co2 + (ya-yb+yc-yd)* (si2)    
* yc' = (ya-yb+yc-yd)* co2 - (xa-xb+xc-xd)* (si2)    
* xd' = (xa-yb-xc+yd)* co3 + (ya+xb-yc-xd)* (si3)    
* yd' = (ya+xb-yc-xd)* co3 - (xa-yb-xc+yd)* (si3)    
*    
*/

/**    
 * @brief  Core function for the Q31 CFFT butterfly process.   
 * @param[in, out] *pSrc            points to the in-place buffer of Q31 data type.   
 * @param[in]      fftLen           length of the FFT.   
 * @param[in]      *pCoef           points to twiddle coefficient buffer.   
 * @param[in]      twidCoefModifier twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table.   
 * @return none.   
 */

void riscv_radix4_butterfly_q31(
  q31_t * pSrc,
  uint32_t fftLen,
  q31_t * pCoef,
  uint32_t twidCoefModifier)
{
  uint32_t n1, n2, ia1, ia2, ia3, i0, j, k;
  q31_t t1, t2, r1, r2, s1, s2, co1, co2, co3, si1, si2, si3;

  q31_t xa, xb, xc, xd;
  q31_t ya, yb, yc, yd;
  q31_t xa_out, xb_out, xc_out, xd_out;
  q31_t ya_out, yb_out, yc_out, yd_out;

  q31_t *ptr1;
  q31_t *pSi0;
  q31_t *pSi1;
  q31_t *pSi2;
  q31_t *pSi3;
  q63_t xaya, xbyb, xcyc, xdyd;
  /* Total process is divided into three stages */

  /* process first stage, middle stages, & last stage */


  /* start of first stage process */

  /*  Initializations for the first stage */
  n2 = fftLen;
  n1 = n2;
  /* n2 = fftLen/4 */
  n2 >>= 2u;

  ia1 = 0u;

  j = n2;
  
  pSi0 = pSrc;
  pSi1 = pSi0 + 2 * n2;
  pSi2 = pSi1 + 2 * n2;
  pSi3 = pSi2 + 2 * n2;

  /*  Calculation of first stage */
  do
  {
    /* input is in 1.31(q31) format and provide 4 guard bits for the input */

    /*  Butterfly implementation */
    /* xa + xc */
    r1 = (pSi0[0] >> 4u) + (pSi2[0] >> 4u);
    /* xa - xc */
    r2 = (pSi0[0] >> 4u) - (pSi2[0] >> 4u);

    /* xb + xd */
    t1 = (pSi1[0] >> 4u) + (pSi3[0] >> 4u);

    /* ya + yc */
    s1 = (pSi0[1] >> 4u) + (pSi2[1] >> 4u);
    /* ya - yc */
    s2 = (pSi0[1] >> 4u) - (pSi2[1] >> 4u);

    /* xa' = xa + xb + xc + xd */
    *pSi0++ = (r1 + t1);
    /* (xa + xc) - (xb + xd) */
    r1 = r1 - t1;
    /* yb + yd */
    t2 = (pSi1[1] >> 4u) + (pSi3[1] >> 4u);

    /* ya' = ya + yb + yc + yd */
    *pSi0++ = (s1 + t2);

    /* (ya + yc) - (yb + yd) */
    s1 = s1 - t2;

    /* yb - yd */
    t1 = (pSi1[1] >> 4u) - (pSi3[1] >> 4u);
    /* xb - xd */
    t2 = (pSi1[0] >> 4u) - (pSi3[0] >> 4u);

    /*  index calculation for the coefficients */
    ia2 = 2u * ia1;
    co2 = pCoef[ia2 * 2u];
    si2 = pCoef[(ia2 * 2u) + 1u];

    /* xc' = (xa-xb+xc-xd)co2 + (ya-yb+yc-yd)(si2) */
    *pSi1++ = (((int32_t) (((q63_t) r1 * co2) >> 32)) +
                     ((int32_t) (((q63_t) s1 * si2) >> 32))) << 1u;

    /* yc' = (ya-yb+yc-yd)co2 - (xa-xb+xc-xd)(si2) */
    *pSi1++ = (((int32_t) (((q63_t) s1 * co2) >> 32)) -
                            ((int32_t) (((q63_t) r1 * si2) >> 32))) << 1u;

    /* (xa - xc) + (yb - yd) */
    r1 = r2 + t1;
    /* (xa - xc) - (yb - yd) */
    r2 = r2 - t1;

    /* (ya - yc) - (xb - xd) */
    s1 = s2 - t2;
    /* (ya - yc) + (xb - xd) */
    s2 = s2 + t2;

    co1 = pCoef[ia1 * 2u];
    si1 = pCoef[(ia1 * 2u) + 1u];

    /* xb' = (xa+yb-xc-yd)co1 + (ya-xb-yc+xd)(si1) */
    *pSi2++ = (((int32_t) (((q63_t) r1 * co1) >> 32)) +
                     ((int32_t) (((q63_t) s1 * si1) >> 32))) << 1u;

    /* yb' = (ya-xb-yc+xd)co1 - (xa+yb-xc-yd)(si1) */
    *pSi2++ = (((int32_t) (((q63_t) s1 * co1) >> 32)) -
                            ((int32_t) (((q63_t) r1 * si1) >> 32))) << 1u;

    /*  index calculation for the coefficients */
    ia3 = 3u * ia1;
    co3 = pCoef[ia3 * 2u];
    si3 = pCoef[(ia3 * 2u) + 1u];

    /* xd' = (xa-yb-xc+yd)co3 + (ya+xb-yc-xd)(si3) */
    *pSi3++ = (((int32_t) (((q63_t) r2 * co3) >> 32)) +
                     ((int32_t) (((q63_t) s2 * si3) >> 32))) << 1u;

    /* yd' = (ya+xb-yc-xd)co3 - (xa-yb-xc+yd)(si3) */
    *pSi3++ = (((int32_t) (((q63_t) s2 * co3) >> 32)) -
                            ((int32_t) (((q63_t) r2 * si3) >> 32))) << 1u;

    /*  Twiddle coefficients index modifier */
    ia1 = ia1 + twidCoefModifier;

  } while(--j);

  /* end of first stage process */

  /* data is in 5.27(q27) format */


  /* start of Middle stages process */


  /* each stage in middle stages provides two down scaling of the input */

  twidCoefModifier <<= 2u;


  for (k = fftLen / 4u; k > 4u; k >>= 2u)
  {
    /*  Initializations for the first stage */
    n1 = n2;
    n2 >>= 2u;
    ia1 = 0u;

    /*  Calculation of first stage */
    for (j = 0u; j <= (n2 - 1u); j++)
    {
      /*  index calculation for the coefficients */
      ia2 = ia1 + ia1;
      ia3 = ia2 + ia1;
      co1 = pCoef[ia1 * 2u];
      si1 = pCoef[(ia1 * 2u) + 1u];
      co2 = pCoef[ia2 * 2u];
      si2 = pCoef[(ia2 * 2u) + 1u];
      co3 = pCoef[ia3 * 2u];
      si3 = pCoef[(ia3 * 2u) + 1u];
      /*  Twiddle coefficients index modifier */
      ia1 = ia1 + twidCoefModifier;
      
      pSi0 = pSrc + 2 * j;
      pSi1 = pSi0 + 2 * n2;
      pSi2 = pSi1 + 2 * n2;
      pSi3 = pSi2 + 2 * n2;

      for (i0 = j; i0 < fftLen; i0 += n1)
      {
        /*  Butterfly implementation */
        /* xa + xc */
        r1 = pSi0[0] + pSi2[0];

        /* xa - xc */
        r2 = pSi0[0] - pSi2[0];


        /* ya + yc */
        s1 = pSi0[1] + pSi2[1];

        /* ya - yc */
        s2 = pSi0[1] - pSi2[1];


        /* xb + xd */
        t1 = pSi1[0] + pSi3[0];


        /* xa' = xa + xb + xc + xd */
        pSi0[0] = (r1 + t1) >> 2u;
        /* xa + xc -(xb + xd) */
        r1 = r1 - t1;

        /* yb + yd */
        t2 = pSi1[1] + pSi3[1];

        /* ya' = ya + yb + yc + yd */
        pSi0[1] = (s1 + t2) >> 2u;
        pSi0 += 2 * n1;

        /* (ya + yc) - (yb + yd) */
        s1 = s1 - t2;

        /* (yb - yd) */
        t1 = pSi1[1] - pSi3[1];

        /* (xb - xd) */
        t2 = pSi1[0] - pSi3[0];


        /* xc' = (xa-xb+xc-xd)co2 + (ya-yb+yc-yd)(si2) */
        pSi1[0] = (((int32_t) (((q63_t) r1 * co2) >> 32)) +
                         ((int32_t) (((q63_t) s1 * si2) >> 32))) >> 1u;

        /* yc' = (ya-yb+yc-yd)co2 - (xa-xb+xc-xd)(si2) */
        pSi1[1] = (((int32_t) (((q63_t) s1 * co2) >> 32)) -
                                ((int32_t) (((q63_t) r1 * si2) >> 32))) >> 1u;
        pSi1 += 2 * n1;

        /* (xa - xc) + (yb - yd) */
        r1 = r2 + t1;
        /* (xa - xc) - (yb - yd) */
        r2 = r2 - t1;

        /* (ya - yc) -  (xb - xd) */
        s1 = s2 - t2;
        /* (ya - yc) +  (xb - xd) */
        s2 = s2 + t2;

        /* xb' = (xa+yb-xc-yd)co1 + (ya-xb-yc+xd)(si1) */
        pSi2[0] = (((int32_t) (((q63_t) r1 * co1) >> 32)) +
                         ((int32_t) (((q63_t) s1 * si1) >> 32))) >> 1u;

        /* yb' = (ya-xb-yc+xd)co1 - (xa+yb-xc-yd)(si1) */
        pSi2[1] = (((int32_t) (((q63_t) s1 * co1) >> 32)) -
                                ((int32_t) (((q63_t) r1 * si1) >> 32))) >> 1u;
        pSi2 += 2 * n1;

        /* xd' = (xa-yb-xc+yd)co3 + (ya+xb-yc-xd)(si3) */
        pSi3[0] = (((int32_t) (((q63_t) r2 * co3) >> 32)) +
                         ((int32_t) (((q63_t) s2 * si3) >> 32))) >> 1u;

        /* yd' = (ya+xb-yc-xd)co3 - (xa-yb-xc+yd)(si3) */
        pSi3[1] = (((int32_t) (((q63_t) s2 * co3) >> 32)) -
                                ((int32_t) (((q63_t) r2 * si3) >> 32))) >> 1u;
        pSi3 += 2 * n1;
      }
    }
    twidCoefModifier <<= 2u;
  }


  /* End of Middle stages process */

  /* data is in 11.21(q21) format for the 1024 point as there are 3 middle stages */
  /* data is in 9.23(q23) format for the 256 point as there are 2 middle stages */
  /* data is in 7.25(q25) format for the 64 point as there are 1 middle stage */
  /* data is in 5.27(q27) format for the 16 point as there are no middle stages */


  /* start of Last stage process */
  /*  Initializations for the last stage */
  j = fftLen >> 2;
  ptr1 = &pSrc[0];

  /*  Calculations of last stage */
  do
  {
    xa = *ptr1++;
    ya = *ptr1++;
    xb = *ptr1++;
    yb = *ptr1++;
    xc = *ptr1++;
    yc = *ptr1++;
    xd = *ptr1++;
    yd = *ptr1++;

    /* xa' = xa + xb + xc + xd */
    xa_out = xa + xb + xc + xd;

    /* ya' = ya + yb + yc + yd */
    ya_out = ya + yb + yc + yd;

    /* pointer updation for writing */
    ptr1 = ptr1 - 8u;

    /* writing xa' and ya' */
    *ptr1++ = xa_out;
    *ptr1++ = ya_out;

    xc_out = (xa - xb + xc - xd);
    yc_out = (ya - yb + yc - yd);

    /* writing xc' and yc' */
    *ptr1++ = xc_out;
    *ptr1++ = yc_out;

    xb_out = (xa + yb - xc - yd);
    yb_out = (ya - xb - yc + xd);

    /* writing xb' and yb' */
    *ptr1++ = xb_out;
    *ptr1++ = yb_out;

    xd_out = (xa - yb - xc + yd);
    yd_out = (ya + xb - yc - xd);

    /* writing xd' and yd' */
    *ptr1++ = xd_out;
    *ptr1++ = yd_out;


  } while(--j);

  /* output is in 11.21(q21) format for the 1024 point */
  /* output is in 9.23(q23) format for the 256 point */
  /* output is in 7.25(q25) format for the 64 point */
  /* output is in 5.27(q27) format for the 16 point */

  /* End of last stage process */

}


/**    
 * @brief  Core function for the Q31 CIFFT butterfly process.   
 * @param[in, out] *pSrc            points to the in-place buffer of Q31 data type.   
 * @param[in]      fftLen           length of the FFT.   
 * @param[in]      *pCoef           points to twiddle coefficient buffer.   
 * @param[in]      twidCoefModifier twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table.   
 * @return none.   
 */


/*    
* Radix-4 IFFT algorithm used is :    
*    
* CIFFT uses same twiddle coefficients as CFFT Function    
*  x[k] = x[n] + (j)k * x[n + fftLen/4] + (-1)k * x[n+fftLen/2] + (-j)k * x[n+3*fftLen/4]    
*    
*    
* IFFT is implemented with following changes in equations from FFT    
*    
* Input real and imaginary data:    
* x(n) = xa + j * ya    
* x(n+N/4 ) = xb + j * yb    
* x(n+N/2 ) = xc + j * yc    
* x(n+3N 4) = xd + j * yd    
*    
*    
* Output real and imaginary data:    
* x(4r) = xa'+ j * ya'    
* x(4r+1) = xb'+ j * yb'    
* x(4r+2) = xc'+ j * yc'    
* x(4r+3) = xd'+ j * yd'    
*    
*    
* Twiddle factors for radix-4 IFFT:    
* Wn = co1 + j * (si1)    
* W2n = co2 + j * (si2)    
* W3n = co3 + j * (si3)    
    
* The real and imaginary output values for the radix-4 butterfly are    
* xa' = xa + xb + xc + xd    
* ya' = ya + yb + yc + yd    
* xb' = (xa-yb-xc+yd)* co1 - (ya+xb-yc-xd)* (si1)    
* yb' = (ya+xb-yc-xd)* co1 + (xa-yb-xc+yd)* (si1)    
* xc' = (xa-xb+xc-xd)* co2 - (ya-yb+yc-yd)* (si2)    
* yc' = (ya-yb+yc-yd)* co2 + (xa-xb+xc-xd)* (si2)    
* xd' = (xa+yb-xc-yd)* co3 - (ya-xb-yc+xd)* (si3)    
* yd' = (ya-xb-yc+xd)* co3 + (xa+yb-xc-yd)* (si3)    
*    
*/

void riscv_radix4_butterfly_inverse_q31(
  q31_t * pSrc,
  uint32_t fftLen,
  q31_t * pCoef,
  uint32_t twidCoefModifier)
{
  uint32_t n1, n2, ia1, ia2, ia3, i0, j, k;
  q31_t t1, t2, r1, r2, s1, s2, co1, co2, co3, si1, si2, si3;
  q31_t xa, xb, xc, xd;
  q31_t ya, yb, yc, yd;
  q31_t xa_out, xb_out, xc_out, xd_out;
  q31_t ya_out, yb_out, yc_out, yd_out;

  q31_t *ptr1;
  q31_t *pSi0;
  q31_t *pSi1;
  q31_t *pSi2;
  q31_t *pSi3;
  q63_t xaya, xbyb, xcyc, xdyd;

  /* input is be 1.31(q31) format for all FFT sizes */
  /* Total process is divided into three stages */
  /* process first stage, middle stages, & last stage */

  /* Start of first stage process */

  /* Initializations for the first stage */
  n2 = fftLen;
  n1 = n2;
  /* n2 = fftLen/4 */
  n2 >>= 2u;

  ia1 = 0u;

  j = n2;
  
  pSi0 = pSrc;
  pSi1 = pSi0 + 2 * n2;
  pSi2 = pSi1 + 2 * n2;
  pSi3 = pSi2 + 2 * n2;

  do
  {
    /*  Butterfly implementation */
    /* xa + xc */
    r1 = (pSi0[0] >> 4u) + (pSi2[0] >> 4u);
    /* xa - xc */
    r2 = (pSi0[0] >> 4u) - (pSi2[0] >> 4u);

    /* xb + xd */
    t1 = (pSi1[0] >> 4u) + (pSi3[0] >> 4u);

    /* ya + yc */
    s1 = (pSi0[1] >> 4u) + (pSi2[1] >> 4u);
    /* ya - yc */
    s2 = (pSi0[1] >> 4u) - (pSi2[1] >> 4u);

    /* xa' = xa + xb + xc + xd */
    *pSi0++ = (r1 + t1);
    /* (xa + xc) - (xb + xd) */
    r1 = r1 - t1;
    /* yb + yd */
    t2 = (pSi1[1] >> 4u) + (pSi3[1] >> 4u);
    /* ya' = ya + yb + yc + yd */
    *pSi0++ = (s1 + t2);

    /* (ya + yc) - (yb + yd) */
    s1 = s1 - t2;

    /* yb - yd */
    t1 = (pSi1[1] >> 4u) - (pSi3[1] >> 4u);
    /* xb - xd */
    t2 = (pSi1[0] >> 4u) - (pSi3[0] >> 4u);

    /*  index calculation for the coefficients */
    ia2 = 2u * ia1;
    co2 = pCoef[ia2 * 2u];
    si2 = pCoef[(ia2 * 2u) + 1u];

    /* xc' = (xa-xb+xc-xd)co2 - (ya-yb+yc-yd)(si2) */
    *pSi1++ = (((int32_t) (((q63_t) r1 * co2) >> 32)) -
                     ((int32_t) (((q63_t) s1 * si2) >> 32))) << 1u;

    /* yc' = (ya-yb+yc-yd)co2 + (xa-xb+xc-xd)(si2) */
    *pSi1++ = (((int32_t) (((q63_t) s1 * co2) >> 32)) +
                          ((int32_t) (((q63_t) r1 * si2) >> 32))) << 1u;

    /* (xa - xc) - (yb - yd) */
    r1 = r2 - t1;
    /* (xa - xc) + (yb - yd) */
    r2 = r2 + t1;

    /* (ya - yc) + (xb - xd) */
    s1 = s2 + t2;
    /* (ya - yc) - (xb - xd) */
    s2 = s2 - t2;

    co1 = pCoef[ia1 * 2u];
    si1 = pCoef[(ia1 * 2u) + 1u];

    /* xb' = (xa+yb-xc-yd)co1 - (ya-xb-yc+xd)(si1) */
    *pSi2++ = (((int32_t) (((q63_t) r1 * co1) >> 32)) -
                     ((int32_t) (((q63_t) s1 * si1) >> 32))) << 1u;

    /* yb' = (ya-xb-yc+xd)co1 + (xa+yb-xc-yd)(si1) */
    *pSi2++ = (((int32_t) (((q63_t) s1 * co1) >> 32)) +
                            ((int32_t) (((q63_t) r1 * si1) >> 32))) << 1u;

    /*  index calculation for the coefficients */
    ia3 = 3u * ia1;
    co3 = pCoef[ia3 * 2u];
    si3 = pCoef[(ia3 * 2u) + 1u];

    /* xd' = (xa-yb-xc+yd)co3 - (ya+xb-yc-xd)(si3) */
    *pSi3++ = (((int32_t) (((q63_t) r2 * co3) >> 32)) -
                     ((int32_t) (((q63_t) s2 * si3) >> 32))) << 1u;

    /* yd' = (ya+xb-yc-xd)co3 + (xa-yb-xc+yd)(si3) */
    *pSi3++ = (((int32_t) (((q63_t) s2 * co3) >> 32)) +
                            ((int32_t) (((q63_t) r2 * si3) >> 32))) << 1u;

    /*  Twiddle coefficients index modifier */
    ia1 = ia1 + twidCoefModifier;

  } while(--j);

  /* data is in 5.27(q27) format */
  /* each stage provides two down scaling of the input */


  /* Start of Middle stages process */

  twidCoefModifier <<= 2u;

  /*  Calculation of second stage to excluding last stage */
  for (k = fftLen / 4u; k > 4u; k >>= 2u)
  {
    /*  Initializations for the first stage */
    n1 = n2;
    n2 >>= 2u;
    ia1 = 0u;

    for (j = 0; j <= (n2 - 1u); j++)
    {
      /*  index calculation for the coefficients */
      ia2 = ia1 + ia1;
      ia3 = ia2 + ia1;
      co1 = pCoef[ia1 * 2u];
      si1 = pCoef[(ia1 * 2u) + 1u];
      co2 = pCoef[ia2 * 2u];
      si2 = pCoef[(ia2 * 2u) + 1u];
      co3 = pCoef[ia3 * 2u];
      si3 = pCoef[(ia3 * 2u) + 1u];
      /*  Twiddle coefficients index modifier */
      ia1 = ia1 + twidCoefModifier;
      
      pSi0 = pSrc + 2 * j;
      pSi1 = pSi0 + 2 * n2;
      pSi2 = pSi1 + 2 * n2;
      pSi3 = pSi2 + 2 * n2;

      for (i0 = j; i0 < fftLen; i0 += n1)
      {
        /*  Butterfly implementation */
        /* xa + xc */
        r1 = pSi0[0] + pSi2[0];

        /* xa - xc */
        r2 = pSi0[0] - pSi2[0];


        /* ya + yc */
        s1 = pSi0[1] + pSi2[1];

        /* ya - yc */
        s2 = pSi0[1] - pSi2[1];


        /* xb + xd */
        t1 = pSi1[0] + pSi3[0];


        /* xa' = xa + xb + xc + xd */
        pSi0[0] = (r1 + t1) >> 2u;
        /* xa + xc -(xb + xd) */
        r1 = r1 - t1;
        /* yb + yd */
        t2 = pSi1[1] + pSi3[1];

        /* ya' = ya + yb + yc + yd */
        pSi0[1] = (s1 + t2) >> 2u;
        pSi0 += 2 * n1;

        /* (ya + yc) - (yb + yd) */
        s1 = s1 - t2;

        /* (yb - yd) */
        t1 = pSi1[1] - pSi3[1];

        /* (xb - xd) */
        t2 = pSi1[0] - pSi3[0];


        /* xc' = (xa-xb+xc-xd)co2 - (ya-yb+yc-yd)(si2) */
        pSi1[0] = (((int32_t) (((q63_t) r1 * co2) >> 32u)) -
                         ((int32_t) (((q63_t) s1 * si2) >> 32u))) >> 1u;

        /* yc' = (ya-yb+yc-yd)co2 + (xa-xb+xc-xd)(si2) */
        pSi1[1] =

          (((int32_t) (((q63_t) s1 * co2) >> 32u)) +
           ((int32_t) (((q63_t) r1 * si2) >> 32u))) >> 1u;
        pSi1 += 2 * n1;

        /* (xa - xc) - (yb - yd) */
        r1 = r2 - t1;
        /* (xa - xc) + (yb - yd) */
        r2 = r2 + t1;

        /* (ya - yc) +  (xb - xd) */
        s1 = s2 + t2;
        /* (ya - yc) -  (xb - xd) */
        s2 = s2 - t2;

        /* xb' = (xa+yb-xc-yd)co1 - (ya-xb-yc+xd)(si1) */
        pSi2[0] = (((int32_t) (((q63_t) r1 * co1) >> 32)) -
                         ((int32_t) (((q63_t) s1 * si1) >> 32))) >> 1u;

        /* yb' = (ya-xb-yc+xd)co1 + (xa+yb-xc-yd)(si1) */
        pSi2[1] = (((int32_t) (((q63_t) s1 * co1) >> 32)) +
                                ((int32_t) (((q63_t) r1 * si1) >> 32))) >> 1u;
        pSi2 += 2 * n1;

        /* xd' = (xa-yb-xc+yd)co3 - (ya+xb-yc-xd)(si3) */
        pSi3[0] = (((int32_t) (((q63_t) r2 * co3) >> 32)) -
                           ((int32_t) (((q63_t) s2 * si3) >> 32))) >> 1u;

        /* yd' = (ya+xb-yc-xd)co3 + (xa-yb-xc+yd)(si3) */
        pSi3[1] = (((int32_t) (((q63_t) s2 * co3) >> 32)) +
                                ((int32_t) (((q63_t) r2 * si3) >> 32))) >> 1u;
        pSi3 += 2 * n1;
      }
    }
    twidCoefModifier <<= 2u;
  }
  /* End of Middle stages process */

  /* data is in 11.21(q21) format for the 1024 point as there are 3 middle stages */
  /* data is in 9.23(q23) format for the 256 point as there are 2 middle stages */
  /* data is in 7.25(q25) format for the 64 point as there are 1 middle stage */
  /* data is in 5.27(q27) format for the 16 point as there are no middle stages */


  /* Start of last stage process */


  /*  Initializations for the last stage */
  j = fftLen >> 2;
  ptr1 = &pSrc[0];

  /*  Calculations of last stage */
  do
  {
    xa = *ptr1++;
    ya = *ptr1++;
    xb = *ptr1++;
    yb = *ptr1++;
    xc = *ptr1++;
    yc = *ptr1++;
    xd = *ptr1++;
    yd = *ptr1++;

    /* xa' = xa + xb + xc + xd */
    xa_out = xa + xb + xc + xd;

    /* ya' = ya + yb + yc + yd */
    ya_out = ya + yb + yc + yd;

    /* pointer updation for writing */
    ptr1 = ptr1 - 8u;

    /* writing xa' and ya' */
    *ptr1++ = xa_out;
    *ptr1++ = ya_out;

    xc_out = (xa - xb + xc - xd);
    yc_out = (ya - yb + yc - yd);

    /* writing xc' and yc' */
    *ptr1++ = xc_out;
    *ptr1++ = yc_out;

    xb_out = (xa - yb - xc + yd);
    yb_out = (ya + xb - yc - xd);

    /* writing xb' and yb' */
    *ptr1++ = xb_out;
    *ptr1++ = yb_out;

    xd_out = (xa + yb - xc - yd);
    yd_out = (ya - xb - yc + xd);

    /* writing xd' and yd' */
    *ptr1++ = xd_out;
    *ptr1++ = yd_out;

  } while(--j);

  /* output is in 11.21(q21) format for the 1024 point */
  /* output is in 9.23(q23) format for the 256 point */
  /* output is in 7.25(q25) format for the 64 point */
  /* output is in 5.27(q27) format for the 16 point */

  /* End of last stage process */
}
