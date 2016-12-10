#!/usr/bin/env python
import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("jobIndex")
args = parser.parse_args()

id = int(args.jobIndex)

plcMap = {
	0 : "Pi",
	1 : "K",
	2 : "P"
}

chargeMap = {
	0 : "p",
	1 : "n"
}

iPlc = (id % 6 )
iCharge = (iPlc / 3)
iPlc = iPlc % 3
iCen = (id / 6) % 7
# iCharge = (id / 3 ) 

print "Plc", iPlc
print "iCharge", iCharge
print "iCen", iCen

command = "--iPlc=" + str(iPlc) + " --iCharge=" + str(iCharge) + " --iCen=" + str(iCen)
print command