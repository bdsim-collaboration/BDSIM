simple_testing(sampler-placement               "--file=user_placed_sampler.gmad"             "")
simple_testing(sampler-placement-far           "--file=user_placed_sampler_far.gmad"         "")
simple_testing(sampler-placement-rectangular   "--file=user_placed_sampler_rectangular.gmad" "")
simple_testing(sampler-placement-by-aperture   "--file=user_placed_sampler_ap_model.gmad"    "")
simple_testing(sampler-placement-cylindrical   "--file=sampler-cylindrical-placement.gmad"   "")
simple_testing(sampler-placement-spherical     "--file=sampler-spherical-placement.gmad"     "")

simple_testing(sampler-particles            "--file=sampler-particles.gmad"           "")
simple_testing(sampler-particles-all        "--file=sampler-particles-all.gmad"       "")
simple_testing(sampler-particles-combined   "--file=sampler-particles-combined.gmad"  "")
simple_testing(sampler-placement-particles  "--file=sampler-placement-particles.gmad" "")


simple_fail(io-user-sampler-bad-shape    "--file=user_placed_sampler_bad_shape.gmad"   "")
simple_fail(io-user-sampler-bad-name     "--file=user_placed_sampler_bad_name.gmad"    "")
simple_testing(io-user-sampler-degenerate-name  "--file=user_placed_sampler_conflicting_name.gmad" "")