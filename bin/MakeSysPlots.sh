#!/bin/bash

source /home/jdb12/.bash_profile

M1=dca_1p0

for PLC in P
do
	for CEN in 0 1 2 4 5 
	do
		for SYS in yLocal_loose zLocal_loose nHitsFit_low nHitsDedx_low nHitsRatio_low qm_nominal
		do
			if [ 4 = $CEN ] || [ 5 = $CEN ] ; then
				CEN2=$((CEN+1))
			else 
				CEN2=$CEN
			fi

			rbplotter config/Plots/Mod_CompareSpectra.xml --plc=$PLC --cen=$CEN --charge=n --m1=$M1 --m2=$SYS --cen2=$CEN2
			rbplotter config/Plots/Mod_CompareSpectra.xml --plc=$PLC --cen=$CEN --charge=p --m1=$M1 --m2=$SYS --cen2=$CEN2
		done	
	done
	
done

