simple_testing(io-rootevent "--file=fodoRing.gmad --output=rootevent --circular" "")

# seed state
simple_testing(io-recreate "--file=sc_recreate.gmad" "")

# write the seed out per event
simple_testing(io-write-ascii-seed-state "--file=sc.gmad --writeSeedState" "")

# use ascii seed state
simple_testing(io-load-ascii-seed-state "--file=sc.gmad --seedStateFileName=../../data/output.seedstate.txt --ngenerate=1" "")

# write optional sampler data
simple_testing(io-sampler-extras        "--file=sc_samplerextras.gmad --ngenerate=10" "")

# write optional eloss data
simple_testing(io-eloss-extras          "--file=sc_elossextras.gmad  --ngenerate=10" "")
simple_testing(io-eloss-extras2         "--file=sc_elossextras2.gmad --ngenerate=10" "")

simple_testing(io-no-model    "--file=sc_no_model.gmad --ngenerate=1" "")
simple_testing(io-no-eloss    "--file=sc_no_eloss.gmad --ngenerate=1" "")