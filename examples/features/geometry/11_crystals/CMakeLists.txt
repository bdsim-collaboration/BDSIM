simple_testing(crystal-box-1               "--file=crystal-box1.gmad --output=none"               ${OVERLAP_CHECK})
simple_testing(crystal-box-1-strong        "--file=crystal-box1-strong.gmad --output=none"        ${OVERLAP_CHECK})
simple_testing(crystal-box-2-strong        "--file=crystal-box2-strong.gmad --output=none"        ${OVERLAP_CHECK})

simple_testing(crystal-cylinder-1          "--file=crystal-cylinder1.gmad --output=none"          ${OVERLAP_CHECK})
simple_testing(crystal-cylinder-1-strong   "--file=crystal-cylinder1-strong.gmad --output=none"   ${OVERLAP_CHECK})
simple_testing(crystal-cylinder-2-strong   "--file=crystal-cylinder2-strong.gmad --output=none"   ${OVERLAP_CHECK})

simple_testing(crystal-torus-1             "--file=crystal-torus1.gmad --output=none"             ${OVERLAP_CHECK})
simple_testing(crystal-torus-1-strong      "--file=crystal-torus1-strong.gmad --output=none"      ${OVERLAP_CHECK})
simple_testing(crystal-torus-2-strong      "--file=crystal-torus2-strong.gmad --output=none"      ${OVERLAP_CHECK})
simple_testing(crystal-torus-2-strong-negz "--file=crystal-torus2-strong-negz.gmad --output=none" ${OVERLAP_CHECK})

simple_fail(crystal-invalid-shape "--file=cyrstal-invalid-shape.gmad")
