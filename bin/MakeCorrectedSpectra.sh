#!/bin/bash

./rcp config/YieldFit_Combine.xml --plc=Pi
./rcp config/YieldFit_Combine.xml --plc=K
./rcp config/YieldFit_Combine.xml --plc=P

./rcp config/ApplyCorrections.xml --plc=Pi
./rcp config/ApplyCorrections.xml --plc=K
./rcp config/ApplyCorrections.xml --plc=P

./rcp config/Spectra_Export.xml --plc=Pi
./rcp config/Spectra_Export.xml --plc=K
./rcp config/Spectra_Export.xml --plc=P

echo "Combining into one file"
./rcp config/Spectra_Combine.xml