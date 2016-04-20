

import os
from shutil import copyfile


plcs = [ "Pi", "K", "P" ];
charges = [ "p", "n" ];
centralities = [ "0", "1", "2", "3", "4", "5", "6" ];

plcMap = {
	"Pi" : 0,
	"K" : 1,
	"P" : 2
}

chargeMap = {
	"p" : 1,
	"n" : 0
}

cenMap = {
	"0" : "6",
	"1" : "5",
	"2" : "4",
	"3" : "3",
	"4" : "2",
	"5" : "1",
	"6" : "0"
}


for plc in plcs :
	for charge in charges :
		for cen in centralities :
			copyfile( "effin/TpcEff_{plc}_{charge}_{bin}.png".format( plc=plc, charge=charge, bin=cen ), 
				"effout/efficiency_c{ec}_p{eplc}_b{ebin}.png".format( ec=chargeMap[ charge ], eplc=plcMap[ plc ], ebin=cenMap[ cen ] ) );