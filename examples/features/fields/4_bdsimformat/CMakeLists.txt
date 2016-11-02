#field tests

interpolator_test("interpolator-1d-nearest" "1d_nearest.gmad")
interpolator_test("interpolator-1d-linear"  "1d_linear.gmad")
interpolator_test("interpolator-1d-cubic"   "1d_cubic.gmad")

interpolator_test("interpolator-2d-nearest" "2d_nearest.gmad")
interpolator_test("interpolator-2d-linear"  "2d_linear.gmad")
interpolator_test("interpolator-2d-cubic"   "2d_cubic.gmad")

interpolator_test("interpolator-3d-nearest" "3d_nearest.gmad")
interpolator_test("interpolator-3d-linear"  "3d_linear.gmad")
interpolator_test("interpolator-3d-cubic"   "3d_cubic.gmad")
