#!/bin/bash

source /Users/jdb/.bash_profile

M1=dca_1p0

for PLC in Pi P
do
	for CEN in 0 1 2 3 4 5 
	do
		for SYS in yLocal_loose zLocal_loose nHitsFit_low nHitsDedx_low nHitsRatio_low
		do
			rbplotter config/Plots/Mod_CompareSpectra.xml --plc=$PLC --cen=$CEN --charge=n --m1=$M1 --m2=$SYS
			rbplotter config/Plots/Mod_CompareSpectra.xml --plc=$PLC --cen=$CEN --charge=p --m1=$M1 --m2=$SYS
		done	
	done
	
done

