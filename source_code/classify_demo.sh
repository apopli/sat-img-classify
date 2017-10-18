#! /bin/sh

# Set parameters
red="RED_BAND.tif"
green="GREEN_BAND.tif"
blue="BLUE_BAND.tif"
infrared="INFRARED_BAND.tif"

# Compile
make rebuild

# Run executable CLASSIFY_TEST
./CLASSIFY_TEST $red $green $blue $infrared

