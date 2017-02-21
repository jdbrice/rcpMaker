#!/bin/bash

echo "Pions"
./rcp config/TpcEff/EnergyLoss.xml --plc=Pi --charge=p &> /dev/null &
./rcp config/TpcEff/EnergyLoss.xml --plc=Pi --charge=n &> /dev/null &

echo "Kaons"
./rcp config/TpcEff/EnergyLoss.xml --plc=K --charge=p &> /dev/null &
./rcp config/TpcEff/EnergyLoss.xml --plc=K --charge=n &> /dev/null &

echo "Protons"
./rcp config/TpcEff/EnergyLoss.xml --plc=P --charge=p &> /dev/null &
./rcp config/TpcEff/EnergyLoss.xml --plc=P --charge=n &> /dev/null &


wait $(jobs -p)
echo "Energy Loss Complete"