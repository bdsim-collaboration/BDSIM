simple_testing(io-rootevent "--file=fodoRing.gmad --output=rootevent --circular" "")

# seed state
simple_testing(io-recreate "--file=sc_recreate.gmad" "")

# write the seed out per event
simple_testing(io-write-ascii-seed-state "--file=sc.gmad --writeSeedState" "")

# use ascii seed state
simple_testing(io-load-ascii-seed-state "--file=sc.gmad --seedStateFileName=../../data/output.seedstate.txt --ngenerate=1" "")

# write optional sampler data
simple_testing(io-sampler-extras        "--file=sc_samplerextras.gmad --ngenerate=10" "")