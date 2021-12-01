#set +x

# for mac, must run once
# defaults write org.xquartz.X11 enable_iglx -bool true
# (restart xquartz)

# maybe required to run this to allow incoming network connections
# xhost +
DV=`ipconfig getifaddr en0`
docker run -t -i -e DISPLAY=$DV:0 --rm bdsim bash
