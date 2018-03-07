#field loader test
simple_fail("field-map-bdsim-format-bad-header" "--file=bad_header.gmad")
simple_fail("field-map-bdsim-format-wrong-file" "--file=wrong_file.gmad")

#interpolator tests

interpolator_test("interpolator-1d-nearest" "1d_nearest.gmad")
interpolator_test("interpolator-1d-linear"  "1d_linear.gmad")
interpolator_test("interpolator-1d-cubic"   "1d_cubic.gmad")

interpolator_test("interpolator-2d-nearest" "2d_nearest.gmad")
interpolator_test("interpolator-2d-linear"  "2d_linear.gmad")
interpolator_test("interpolator-2d-cubic"   "2d_cubic.gmad")

if (USE_GZSTREAM)
  interpolator_test("interpolator-3d-nearest-gz" "3d_nearest.gmad")
  interpolator_test("interpolator-3d-linear-gz"  "3d_linear.gmad")
  interpolator_test("interpolator-3d-cubic-gz"   "3d_cubic.gmad")
  interpolator_test("field-map-bdsim-format-loop-order" "3d_cubic_zyx.gmad")
  
  interpolator_test("interpolator-4d-nearest-gz" "4d_nearest.gmad")
  interpolator_test("interpolator-4d-linear-gz"  "4d_linear.gmad")
  interpolator_test("interpolator-4d-cubic-gz"   "4d_cubic.gmad")
  
  interpolator_test("interpolator-1d-nearest-gz" "1d_nearest_gz.gmad")

  #field tests
  simple_testing(field-map-b-1d "--file=b_field_1d.gmad" "")
  simple_testing(field-map-b-2d "--file=b_field_2d.gmad" "")
  simple_testing(field-map-b-3d "--file=b_field_3d.gmad" "")
  simple_testing(field-map-b-4d "--file=b_field_4d.gmad" "")

  simple_testing(field-map-e-1d "--file=e_field_1d.gmad" "")
  simple_testing(field-map-e-2d "--file=e_field_2d.gmad" "")
  simple_testing(field-map-e-3d "--file=e_field_3d.gmad" "")
  simple_testing(field-map-e-4d "--file=e_field_4d.gmad" "")

  simple_testing(field-map-em-1d "--file=em_field_1d.gmad" "")
  simple_testing(field-map-em-2d "--file=em_field_2d.gmad" "")
  simple_testing(field-map-em-3d "--file=em_field_3d.gmad" "")
  simple_testing(field-map-em-4d "--file=em_field_4d.gmad" "")

  simple_testing(field-map-step-size "--file=field_max_step.gmad" "")
endif()
