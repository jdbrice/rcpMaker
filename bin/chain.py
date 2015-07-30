#!/usr/bin/env python

print ( "Rcp Analysis Chain" )
import argparse
import subprocess as sp
import os

parser = argparse.ArgumentParser( description="Runs the Rcp Analysis" );
parser.add_argument("-config", help="Base path for config files", default="./" )
parser.add_argument("--log", help="output to stdout or to a log file", default=False, action='store_const', const=True )

args = parser.parse_args();

""" Energy Loss """
plcs = ( "Pi", "K", "P" )
charges = ( "p", "n" )



for plc in plcs :
	for chg in charges :
		config = os.path.join( args.config, "eloss", (plc + "_" + chg ) )
		if args.log :
			logFile = config + ".log"
			print [ "./rcp", config, ">&", logFile ]
		#sp.call( [ "./rcp", "" ] )
		print config

