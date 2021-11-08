#field loader test
simple_fail("field-map-bdsim-format-bad-header" "--file=bad_header.gmad")
simple_fail("field-map-bdsim-format-wrong-file" "--file=wrong_file.gmad")

#field map tests that don't require gdml (use a drift)
simple_testing(field-map-b-1d-along-z         "--file=b_field_1d_along_z.gmad --output=none" "")

if (USE_GDML AND USE_GZSTREAM)
  simple_testing(field-map-b-2d-1T-box "--file=fieldmap-box-test.gmad" "")
endif()
