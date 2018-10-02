if (USE_GDML)
  simple_testing(external_world        "--file=1_gdml_external_world.gmad --output=none"   ${OVERLAP_CHECK})
  simple_fail(external_world_fail      "--file=2_gdml_external_world_fail.gmad --output=none")
endif()
