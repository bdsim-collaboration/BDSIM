

simple_testing(field-map-transform-1d-b-reflectx-nearest  "--file=b_field_1d_reflect_x_nearest.gmad" "")
simple_testing(field-map-transform-1d-b-reflectx-linear   "--file=b_field_1d_reflect_x_linear.gmad"  "")
simple_testing(field-map-transform-1d-b-reflectx-cubic    "--file=b_field_1d_reflect_x_cubic.gmad"   "")
simple_testing(field-map-transform-1d-b-reflecty-nearest  "--file=b_field_1d_reflect_y_nearest.gmad" "")
simple_testing(field-map-transform-1d-b-reflecty-linear   "--file=b_field_1d_reflect_y_linear.gmad"  "")
simple_testing(field-map-transform-1d-b-reflecty-cubic    "--file=b_field_1d_reflect_y_cubic.gmad"   "")

simple_testing(field-map-transform-2d-b-reflectxydipole "--file=b_field_2d_reflectxydipole.gmad" "")

interpolator_test(interpolator-1d-transform-b-reflectx-nearest  "b_field_1d_reflect_x_nearest.gmad")
interpolator_test(interpolator-1d-transform-b-reflectx-linear   "b_field_1d_reflect_x_linear.gmad")
interpolator_test(interpolator-1d-transform-b-reflectx-cubic    "b_field_1d_reflect_x_cubic.gmad")
interpolator_test(interpolator-1d-transform-b-reflecty-nearest  "b_field_1d_reflect_y_nearest.gmad")
interpolator_test(interpolator-1d-transform-b-reflecty-linear   "b_field_1d_reflect_y_linear.gmad")
interpolator_test(interpolator-1d-transform-b-reflecty-cubic    "b_field_1d_reflect_y_cubic.gmad")
