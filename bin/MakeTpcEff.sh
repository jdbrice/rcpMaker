#!/bin/bash

echo "Pions"
./rcp config/TpcEff_Make.xml --plc=Pi --charge=p --type=mc
./rcp config/TpcEff_Make.xml --plc=Pi --charge=p --type=rc

./rcp config/TpcEff_Make.xml --plc=Pi --charge=n --type=mc
./rcp config/TpcEff_Make.xml --plc=Pi --charge=n --type=rc

echo "Kaons"
./rcp config/TpcEff_Make.xml --plc=K --charge=p --type=mc
./rcp config/TpcEff_Make.xml --plc=K --charge=p --type=rc

./rcp config/TpcEff_Make.xml --plc=K --charge=n --type=mc
./rcp config/TpcEff_Make.xml --plc=K --charge=n --type=rc


echo "Protons"
./rcp config/TpcEff_Make.xml --plc=P --charge=p --type=mc
./rcp config/TpcEff_Make.xml --plc=P --charge=p --type=rc

./rcp config/TpcEff_Make.xml --plc=P --charge=n --type=mc
./rcp config/TpcEff_Make.xml --plc=P --charge=n --type=rc


echo "Fit"
./rcp config/TpcEff_Fit.xml