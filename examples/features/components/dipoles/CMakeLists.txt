# standard sbends
simple_testing(component-sbend               "--file=sbend.gmad"           ${OVERLAP_CHECK})
simple_testing_long(component-sbend-poleface "--file=sbend_poleface.gmad"  ${OVERLAP_CHECK})
simple_testing_long(component-sbend-fringe   "--file=sbend_fringe.gmad"    ${OVERLAP_CHECK})

# with 'bdsimmatrix' integrator set - different geometry constructed
simple_testing(component-sbend-matrix               "--file=sbend.gmad"           ${OVERLAP_CHECK})
simple_testing_long(component-sbend-matrix-poleface "--file=sbend_poleface.gmad"  ${OVERLAP_CHECK})
simple_testing_long(component-sbend-matrix-fringe   "--file=sbend_fringe.gmad"    ${OVERLAP_CHECK})

# standard rbends
simple_testing(component-rbend                   "--file=rbend.gmad"           ${OVERLAP_CHECK})
simple_testing(component-rbend-poleface          "--file=rbend_poleface.gmad"  ${OVERLAP_CHECK})
simple_testing(component-rbend-fringe            "--file=rbend_fringe.gmad"    ${OVERLAP_CHECK})

# with 'bdsimmatrix' integrator set - different geometry constructed
simple_testing(component-rbend-matrix            "--file=rbend_matrix.gmad"           ${OVERLAP_CHECK})
simple_testing(component-rbend-matrix-poleface   "--file=rbend_matrix_poleface.gmad"  ${OVERLAP_CHECK})
simple_testing(component-rbend-matrix-fringe     "--file=rbend_matrix_fringe.gmad"    ${OVERLAP_CHECK})

# bends with quadrupole components
simple_testing(component-sbend-k1     "--file=sbend_k1.gmad"  ${OVERLAP_CHECK})
simple_testing(component-rbend-k1     "--file=rbend_k1.gmad"  ${OVERLAP_CHECK})

simple_testing(component-rbend-strong-single-e-neg-B   "--file=rbend_strong_single_e_neg_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-e-neg-ang "--file=rbend_strong_single_e_neg_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-e-pos-B   "--file=rbend_strong_single_e_pos_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-e-pos-ang "--file=rbend_strong_single_e_pos_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-p-neg-B   "--file=rbend_strong_single_p_neg_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-p-neg-ang "--file=rbend_strong_single_p_neg_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-p-pos-B   "--file=rbend_strong_single_p_pos_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-p-pos-ang "--file=rbend_strong_single_p_pos_ang.gmad" ${OVERLAP_CHECK})

simple_testing(component-sbend-strong-single-e-neg-B   "--file=sbend_strong_single_e_neg_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-e-neg-ang "--file=sbend_strong_single_e_neg_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-e-pos-B   "--file=sbend_strong_single_e_pos_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-e-pos-ang "--file=sbend_strong_single_e_pos_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-p-neg-B   "--file=sbend_strong_single_p_neg_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-p-neg-ang "--file=sbend_strong_single_p_neg_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-p-pos-B   "--file=sbend_strong_single_p_pos_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-p-pos-ang "--file=sbend_strong_single_p_pos_ang.gmad" ${OVERLAP_CHECK})
