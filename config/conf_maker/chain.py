#!/usr/bin/env python

print ( "Rcp Analysis Chain" )
import argparse
import subprocess as sp
import os

import TpcEff.make_xml_configs as effc
import EnergyLoss.make_xml_configs as elossc
import PidHisto.make_xml_configs as pidhc

parser = argparse.ArgumentParser( description="Runs the Rcp Analysis" );
parser.add_argument("exe", help="path to executable")
parser.add_argument("-config", help="Base path for config files", default="./" )

parser.add_argument("task", help="Task to run" )

args = parser.parse_args();

print args

plcs = ( "Pi", "K", "P" )
if "EnergyLoss" == args.task :
	charges = ( "p", "n" )

	for plc in plcs :
		for c in charges :
			config = os.path.join( args.config, "EnergyLoss", elossc.t_config_file.format( plc=plc, c=c, ext="xml" ) )
			logFile = config + ".log"
			os.system( args.exe + " "  + config + " >& " + logFile )

if "PidHisto" == args.task :
	for plc in plcs :
		config = os.path.join( args.config, "PidHisto", pidh.t_config_file.format( plc=plc, ext="xml" ) )
		logFile = config + ".log"
		os.system( args.exe + " "  + config + " >& " + logFile )

# """ Tpc Efficiency """
# plcs = ( "Pi", "K", "P" )
# charges = ( "p", "n" )
# track_types = ( "mc", "rc" )

# if "TpcEff" == args.task :
# 	for plc in plcs :
# 		for chg in charges :
# 			for tt in track_types :
# 				config = os.path.join( args.config, "TpcEff", effc.t_config_file.format( plc=plc, c=chg, tt=tt, ext="xml" ) )
# 				if args.log :
# 					logFile = config + ".log"
# 					print args.exe + " " + config + " >& " + logFile
# 					os.system( args.exe + " "  + config + " >& " + logFile )
# 				else :
# 					os.system( args.exe + " "  + config )

			

# 	config = os.path.join( args.config, "tpceff", "fit.xml" )
	
# 	if args.log :
# 		logFile = config + ".log"
# 		os.system( args.exe + " "  + config + " >& " + logFile )
# 	else :
# 		os.system( args.exe + " "  + config  )

# 	# TODO: Proton is not working, others are
				

