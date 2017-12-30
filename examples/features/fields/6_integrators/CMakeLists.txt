simple_testing(integrators_bdsim2  "--file=one_of_each_base.gmad"  "")

simple_testing(integrators_g4classicalrk4      "--file=g4classicalrk4.gmad" "")

if (G4_MINOR_VERSION GREATER 2)
simple_testing(integrators_g4dormandprince745  "--file=g4dormandprince745.gmad" "")
endif()