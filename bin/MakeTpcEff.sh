#!/bin/bash

echo "Pions"
./rcp config/TpcEff/Make.xml --plc=Pi --charge=p --type=mc &> /dev/null &
./rcp config/TpcEff/Make.xml --plc=Pi --charge=p --type=rc &> /dev/null &

./rcp config/TpcEff/Make.xml --plc=Pi --charge=n --type=mc &> /dev/null &
./rcp config/TpcEff/Make.xml --plc=Pi --charge=n --type=rc &> /dev/null &

echo "Kaons"
./rcp config/TpcEff/Make.xml --plc=K --charge=p --type=mc &> /dev/null &
./rcp config/TpcEff/Make.xml --plc=K --charge=p --type=rc &> /dev/null &

./rcp config/TpcEff/Make.xml --plc=K --charge=n --type=mc &> /dev/null &
./rcp config/TpcEff/Make.xml --plc=K --charge=n --type=rc &> /dev/null &

echo "Protons"
./rcp config/TpcEff/Make.xml --plc=P --charge=p --type=mc &> /dev/null &
./rcp config/TpcEff/Make.xml --plc=P --charge=p --type=rc &> /dev/null &

./rcp config/TpcEff/Make.xml --plc=P --charge=n --type=mc &> /dev/null &
./rcp config/TpcEff/Make.xml --plc=P --charge=n --type=rc &> /dev/null &


wait $(jobs -p)

echo "Fit"
./rcp config/TpcEff/Fit.xml



# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0
# --dca=0p25 --TrackCuts.dca:max=30.0