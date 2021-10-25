cmake_minimum_required(VERSION 3.20)

project(
    RiscvCmsisDsp
    VERSION 0.1
    DESCRIPTION "RISCV CMSIS DSP library ported from ARM"
    LANGUAGES C
)

include(cmake/riscv.cmake)

set(SOURCES
    src/BasicMathFunctions/riscv_abs_f32.c 
    src/BasicMathFunctions/riscv_abs_q15.c
    src/BasicMathFunctions/riscv_abs_q31.c 
    src/BasicMathFunctions/riscv_abs_q7.c 
    src/BasicMathFunctions/riscv_add_f32.c 
    src/BasicMathFunctions/riscv_add_q15.c 
    src/BasicMathFunctions/riscv_add_q31.c 
    src/BasicMathFunctions/riscv_add_q7.c 
    src/BasicMathFunctions/riscv_dot_prod_f32.c
    src/BasicMathFunctions/riscv_dot_prod_q15.c
    src/BasicMathFunctions/riscv_dot_prod_q31.c
    src/BasicMathFunctions/riscv_dot_prod_q7.c
    src/BasicMathFunctions/riscv_mult_f32.c
    src/BasicMathFunctions/riscv_mult_q15.c  
    src/BasicMathFunctions/riscv_mult_q31.c 
    src/BasicMathFunctions/riscv_mult_q7.c
    src/BasicMathFunctions/riscv_negate_f32.c 
    src/BasicMathFunctions/riscv_negate_q15.c
    src/BasicMathFunctions/riscv_negate_q31.c
    src/BasicMathFunctions/riscv_negate_q7.c
    src/BasicMathFunctions/riscv_offset_f32.c
    src/BasicMathFunctions/riscv_offset_q15.c
    src/BasicMathFunctions/riscv_offset_q31.c
    src/BasicMathFunctions/riscv_offset_q7.c 
    src/BasicMathFunctions/riscv_scale_f32.c
    src/BasicMathFunctions/riscv_scale_q15.c
    src/BasicMathFunctions/riscv_scale_q31.c
    src/BasicMathFunctions/riscv_scale_q7.c
    src/BasicMathFunctions/riscv_shift_q15.c
    src/BasicMathFunctions/riscv_shift_q31.c
    src/BasicMathFunctions/riscv_shift_q7.c
    src/BasicMathFunctions/riscv_sub_f32.c
    src/BasicMathFunctions/riscv_sub_q15.c
    src/BasicMathFunctions/riscv_sub_q31.c
    src/BasicMathFunctions/riscv_sub_q7.c
    src/CommonTables/riscv_common_tables.c
    src/CommonTables/riscv_const_structs.c
    src/FastMathFunctions/riscv_cos_f32.c
    src/FastMathFunctions/riscv_cos_q15.c
    src/FastMathFunctions/riscv_cos_q31.c
    src/FastMathFunctions/riscv_sin_f32.c
    src/FastMathFunctions/riscv_sin_q15.c
    src/FastMathFunctions/riscv_sin_q31.c
    src/FastMathFunctions/riscv_sqrt_q31.c
    src/FastMathFunctions/riscv_sqrt_q15.c
    src/ComplexMathFunctions/riscv_cmplx_conj_f32.c
    src/ComplexMathFunctions/riscv_cmplx_conj_q15.c
    src/ComplexMathFunctions/riscv_cmplx_conj_q31.c
    src/ComplexMathFunctions/riscv_cmplx_dot_prod_f32.c
    src/ComplexMathFunctions/riscv_cmplx_dot_prod_q15.c
    src/ComplexMathFunctions/riscv_cmplx_dot_prod_q31.c
    src/ComplexMathFunctions/riscv_cmplx_mag_f32.c
    src/ComplexMathFunctions/riscv_cmplx_mag_q15.c
    src/ComplexMathFunctions/riscv_cmplx_mag_q31.c
    src/ComplexMathFunctions/riscv_cmplx_mag_squared_f32.c
    src/ComplexMathFunctions/riscv_cmplx_mag_squared_q15.c
    src/ComplexMathFunctions/riscv_cmplx_mag_squared_q31.c
    src/ComplexMathFunctions/riscv_cmplx_mult_cmplx_f32.c
    src/ComplexMathFunctions/riscv_cmplx_mult_cmplx_q15.c
    src/ComplexMathFunctions/riscv_cmplx_mult_cmplx_q31.c
    src/ComplexMathFunctions/riscv_cmplx_mult_real_f32.c
    src/ComplexMathFunctions/riscv_cmplx_mult_real_q15.c
    src/ComplexMathFunctions/riscv_cmplx_mult_real_q31.c
    src/MatrixFunctions/riscv_mat_add_f32.c
    src/MatrixFunctions/riscv_mat_add_q15.c
    src/MatrixFunctions/riscv_mat_add_q31.c
    src/MatrixFunctions/riscv_mat_cmplx_mult_f32.c
    src/MatrixFunctions/riscv_mat_cmplx_mult_q15.c
    src/MatrixFunctions/riscv_mat_cmplx_mult_q31.c
    src/MatrixFunctions/riscv_mat_init_f32.c 
    src/MatrixFunctions/riscv_mat_init_q15.c
    src/MatrixFunctions/riscv_mat_init_q31.c
    src/MatrixFunctions/riscv_mat_inverse_f32.c
    src/MatrixFunctions/riscv_mat_inverse_f64.c 
    src/MatrixFunctions/riscv_mat_mult_f32.c 
    src/MatrixFunctions/riscv_mat_mult_fast_q15.c
    src/MatrixFunctions/riscv_mat_mult_fast_q31.c 
    src/MatrixFunctions/riscv_mat_mult_q15.c 
    src/MatrixFunctions/riscv_mat_mult_q31.c
    src/MatrixFunctions/riscv_mat_scale_f32.c
    src/MatrixFunctions/riscv_mat_scale_q15.c
    src/MatrixFunctions/riscv_mat_scale_q31.c
    src/MatrixFunctions/riscv_mat_sub_f32.c 
    src/MatrixFunctions/riscv_mat_sub_q15.c
    src/MatrixFunctions/riscv_mat_sub_q31.c 
    src/MatrixFunctions/riscv_mat_trans_f32.c
    src/MatrixFunctions/riscv_mat_trans_q15.c
    src/MatrixFunctions/riscv_mat_trans_q31.c 
    src/StatisticsFunctions/riscv_max_f32.c
    src/StatisticsFunctions/riscv_max_q7.c
    src/StatisticsFunctions/riscv_max_q15.c
    src/StatisticsFunctions/riscv_max_q31.c
    src/StatisticsFunctions/riscv_mean_f32.c
    src/StatisticsFunctions/riscv_mean_q7.c
    src/StatisticsFunctions/riscv_mean_q15.c
    src/StatisticsFunctions/riscv_mean_q31.c
    src/StatisticsFunctions/riscv_min_f32.c
    src/StatisticsFunctions/riscv_min_q7.c
    src/StatisticsFunctions/riscv_min_q15.c
    src/StatisticsFunctions/riscv_min_q31.c
    src/StatisticsFunctions/riscv_power_f32.c
    src/StatisticsFunctions/riscv_power_q7.c
    src/StatisticsFunctions/riscv_power_q15.c
    src/StatisticsFunctions/riscv_power_q31.c
    src/StatisticsFunctions/riscv_rms_f32.c
    src/StatisticsFunctions/riscv_rms_q15.c
    src/StatisticsFunctions/riscv_rms_q31.c
    src/StatisticsFunctions/riscv_std_f32.c
    src/StatisticsFunctions/riscv_std_q15.c
    src/StatisticsFunctions/riscv_std_q31.c
    src/StatisticsFunctions/riscv_var_f32.c
    src/StatisticsFunctions/riscv_var_q15.c
    src/StatisticsFunctions/riscv_var_q31.c
    src/SupportFunctions/riscv_copy_f32.c
    src/SupportFunctions/riscv_copy_q7.c
    src/SupportFunctions/riscv_copy_q15.c
    src/SupportFunctions/riscv_copy_q31.c
    src/SupportFunctions/riscv_fill_f32.c
    src/SupportFunctions/riscv_fill_q7.c
    src/SupportFunctions/riscv_fill_q15.c
    src/SupportFunctions/riscv_fill_q31.c
    src/SupportFunctions/riscv_float_to_q7.c
    src/SupportFunctions/riscv_float_to_q15.c
    src/SupportFunctions/riscv_float_to_q31.c
    src/SupportFunctions/riscv_q7_to_float.c
    src/SupportFunctions/riscv_q7_to_q15.c
    src/SupportFunctions/riscv_q7_to_q31.c
    src/SupportFunctions/riscv_q15_to_float.c
    src/SupportFunctions/riscv_q15_to_q7.c
    src/SupportFunctions/riscv_q15_to_q31.c
    src/SupportFunctions/riscv_q31_to_float.c
    src/SupportFunctions/riscv_q31_to_q7.c
    src/SupportFunctions/riscv_q31_to_q15.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_32x64_init_q31.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_32x64_q31.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_f32.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_fast_q15.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_fast_q31.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_init_f32.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_init_q15.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_init_q31.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_q15.c
    src/FilteringFunctions/riscv_biquad_cascade_df1_q31.c
    src/FilteringFunctions/riscv_biquad_cascade_df2T_f32.c
    src/FilteringFunctions/riscv_biquad_cascade_df2T_f64.c
    src/FilteringFunctions/riscv_biquad_cascade_df2T_init_f32.c
    src/FilteringFunctions/riscv_biquad_cascade_df2T_init_f64.c
    src/FilteringFunctions/riscv_biquad_cascade_stereo_df2T_f32.c
    src/FilteringFunctions/riscv_biquad_cascade_stereo_df2T_init_f32.c
    src/FilteringFunctions/riscv_conv_f32.c
    src/FilteringFunctions/riscv_conv_fast_opt_q15.c
    src/FilteringFunctions/riscv_conv_fast_q15.c
    src/FilteringFunctions/riscv_conv_fast_q31.c
    src/FilteringFunctions/riscv_conv_opt_q7.c
    src/FilteringFunctions/riscv_conv_opt_q15.c
    src/FilteringFunctions/riscv_conv_partial_f32.c
    src/FilteringFunctions/riscv_conv_partial_fast_opt_q15.c	
    src/FilteringFunctions/riscv_conv_partial_fast_q15.c
    src/FilteringFunctions/riscv_conv_partial_fast_q31.c
    src/FilteringFunctions/riscv_conv_partial_opt_q7.c
    src/FilteringFunctions/riscv_conv_partial_opt_q15.c
    src/FilteringFunctions/riscv_conv_partial_q7.c
    src/FilteringFunctions/riscv_conv_partial_q15.c
    src/FilteringFunctions/riscv_conv_partial_q31.c
    src/FilteringFunctions/riscv_conv_q7.c
    src/FilteringFunctions/riscv_conv_q15.c
    src/FilteringFunctions/riscv_conv_q31.c
    src/FilteringFunctions/riscv_correlate_f32.c
    src/FilteringFunctions/riscv_correlate_q7.c
    src/FilteringFunctions/riscv_correlate_q15.c
    src/FilteringFunctions/riscv_correlate_q31.c
    src/FilteringFunctions/riscv_correlate_fast_opt_q15.c
    src/FilteringFunctions/riscv_correlate_fast_q15.c
    src/FilteringFunctions/riscv_correlate_fast_q31.c
    src/FilteringFunctions/riscv_correlate_opt_q7.c
    src/FilteringFunctions/riscv_correlate_opt_q15.c
    src/FilteringFunctions/riscv_fir_decimate_f32.c
    src/FilteringFunctions/riscv_fir_decimate_fast_q15.c
    src/FilteringFunctions/riscv_fir_decimate_fast_q31.c
    src/FilteringFunctions/riscv_fir_decimate_init_f32.c
    src/FilteringFunctions/riscv_fir_decimate_init_q15.c
    src/FilteringFunctions/riscv_fir_decimate_init_q31.c
    src/FilteringFunctions/riscv_fir_decimate_q15.c
    src/FilteringFunctions/riscv_fir_decimate_q31.c
    src/FilteringFunctions/riscv_fir_f32.c
    src/FilteringFunctions/riscv_fir_fast_q15.c
    src/FilteringFunctions/riscv_fir_fast_q31.c
    src/FilteringFunctions/riscv_fir_init_f32.c
    src/FilteringFunctions/riscv_fir_init_q7.c
    src/FilteringFunctions/riscv_fir_init_q15.c
    src/FilteringFunctions/riscv_fir_init_q31.c
    src/FilteringFunctions/riscv_fir_q7.c
    src/FilteringFunctions/riscv_fir_q15.c
    src/FilteringFunctions/riscv_fir_q31.c
    src/FilteringFunctions/riscv_fir_lattice_f32.c
    src/FilteringFunctions/riscv_fir_lattice_init_f32.c
    src/FilteringFunctions/riscv_fir_lattice_init_q15.c
    src/FilteringFunctions/riscv_fir_lattice_init_q31.c
    src/FilteringFunctions/riscv_fir_lattice_q15.c
    src/FilteringFunctions/riscv_fir_lattice_q31.c
    src/FilteringFunctions/riscv_fir_interpolate_f32.c
    src/FilteringFunctions/riscv_fir_interpolate_init_f32.c
    src/FilteringFunctions/riscv_fir_interpolate_init_q15.c
    src/FilteringFunctions/riscv_fir_interpolate_init_q31.c
    src/FilteringFunctions/riscv_fir_interpolate_q15.c
    src/FilteringFunctions/riscv_fir_interpolate_q31.c
    src/FilteringFunctions/riscv_iir_lattice_f32.c
    src/FilteringFunctions/riscv_iir_lattice_init_f32.c
    src/FilteringFunctions/riscv_iir_lattice_init_q15.c
    src/FilteringFunctions/riscv_iir_lattice_init_q31.c
    src/FilteringFunctions/riscv_iir_lattice_q15.c
    src/FilteringFunctions/riscv_iir_lattice_q31.c	
    src/FilteringFunctions/riscv_lms_norm_f32.c
    src/FilteringFunctions/riscv_lms_norm_init_f32.c
    src/FilteringFunctions/riscv_lms_norm_init_q15.c
    src/FilteringFunctions/riscv_lms_norm_init_q31.c
    src/FilteringFunctions/riscv_lms_norm_q15.c
    src/FilteringFunctions/riscv_lms_norm_q31.c
    src/FilteringFunctions/riscv_lms_q15.c
    src/FilteringFunctions/riscv_lms_q31.c
    src/FilteringFunctions/riscv_lms_f32.c
    src/FilteringFunctions/riscv_lms_init_f32.c
    src/FilteringFunctions/riscv_lms_init_q15.c
    src/FilteringFunctions/riscv_lms_init_q31.c
    src/FilteringFunctions/riscv_fir_sparse_f32.c
    src/FilteringFunctions/riscv_fir_sparse_init_f32.c
    src/FilteringFunctions/riscv_fir_sparse_init_q7.c
    src/FilteringFunctions/riscv_fir_sparse_init_q15.c
    src/FilteringFunctions/riscv_fir_sparse_init_q31.c
    src/FilteringFunctions/riscv_fir_sparse_q7.c
    src/FilteringFunctions/riscv_fir_sparse_q15.c
    src/FilteringFunctions/riscv_fir_sparse_q31.c
    src/TransformFunctions/riscv_bitreversal.c
    src/TransformFunctions/riscv_bitreversal2.S
    src/TransformFunctions/riscv_cfft_f32.c
    src/TransformFunctions/riscv_cfft_q15.c
    src/TransformFunctions/riscv_cfft_q31.c
    src/TransformFunctions/riscv_cfft_radix8_f32.c
    src/TransformFunctions/riscv_cfft_radix4_q15.c
    src/TransformFunctions/riscv_cfft_radix4_q31.c
    src/TransformFunctions/riscv_rfft_fast_f32.c
    src/TransformFunctions/riscv_rfft_fast_init_f32.c
    src/TransformFunctions/riscv_rfft_init_q15.c
    src/TransformFunctions/riscv_rfft_init_q31.c
    src/TransformFunctions/riscv_rfft_q15.c
    src/TransformFunctions/riscv_rfft_q31.c
    src/TransformFunctions/riscv_cfft_radix4_init_f32.c
    src/TransformFunctions/riscv_dct4_f32.c
    src/TransformFunctions/riscv_dct4_init_f32.c
    src/TransformFunctions/riscv_rfft_init_f32.c
    src/TransformFunctions/riscv_rfft_f32.c
    src/TransformFunctions/riscv_cfft_radix4_init_f32.c
    src/TransformFunctions/riscv_cfft_radix4_f32.c
    src/TransformFunctions/riscv_cfft_radix4_init_q15.c
    src/TransformFunctions/riscv_dct4_q15.c
    src/TransformFunctions/riscv_dct4_q31.c
    src/TransformFunctions/riscv_dct4_init_q31.c
    src/TransformFunctions/riscv_dct4_init_q15.c
    src/TransformFunctions/riscv_cfft_radix4_init_q31.c
    src/ControllerFunctions/riscv_pid_init_f32.c
    src/ControllerFunctions/riscv_pid_init_q15.c
    src/ControllerFunctions/riscv_pid_init_q31.c
    src/ControllerFunctions/riscv_pid_reset_f32.c
    src/ControllerFunctions/riscv_pid_reset_q15.c
    src/ControllerFunctions/riscv_pid_reset_q31.c
    src/ControllerFunctions/riscv_sin_cos_f32.c
    src/ControllerFunctions/riscv_sin_cos_q31.c
    )

set(HEADERS
    inc/riscv_math.h
    inc/riscv_common_tables.h
    inc/riscv_const_structs.h
    )

include_directories(inc/)
add_library(CMSIS_lib STATIC  ${SOURCES} ${HEADERS})

#add_library(cmsis_dsp_lib STATIC ${SOURCES})
#target_include_directories(cmsis_dsp_lib PUBLIC ../inc)
##target_link_libraries(cmsis_dsp_lib PRIVATE m)
#target_compile_features(cmsis_dsp_lib PRIVATE c_std_99)
#target_compile_options(cmsis_dsp_lib PRIVATE 
#    #Warnings
#    -Wall
#    -Wextra
#    -Wshadow
#    -Wdouble-promotion
#    -Wconversion
#    -fno-common
#    #Debug
#    -ffunction-sections
#    -fdata-sections
#    #Memory
#    -fstack-usage
#    #Compile and linker
#    -nostdlib
#    -ffreestanding
#    )
