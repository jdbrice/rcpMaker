#!/usr/bin/env python

print ( "Rcp Analysis Chain" )
import argparse
import subprocess as sp
import os

import TpcEff.make_xml_configs as effc
import EnergyLoss.make_xml_configs as elossc
import PidHisto.make_xml_configs as pidhc
import TofEff.make_xml_configs as tofc
import FeedDown.make_xml_configs as fdc

parser = argparse.ArgumentParser( description="Runs the Rcp Analysis" );
parser.add_argument("exe", help="path to executable")
parser.add_argument("-config", help="Base path for config files", default="./" )

parser.add_argument("task", help="Task to run" )

args = parser.parse_args();

print args

plcs = ( "Pi", "K", "P" )
charges = ( "p", "n" )
track_types = ( "mc", "rc" )



if "EnergyLoss" == args.task :
	for plc in plcs :
		for c in charges :
			config = os.path.join( args.config, "EnergyLoss", elossc.t_config_file.format( plc=plc, c=c, ext="xml" ) )
			logFile = config + ".log"
			cmd = args.exe + " "  + config + " >& " + logFile
			print "Running : "
			print cmd
			os.system( cmd )

if "TofEff" == args.task :
	for plc in plcs :
		config = os.path.join( args.config, "TofEff", tofc.t_config_file.format( plc=plc, ext="xml" ) )
		logFile = config + ".log"
		print( "Running : " )
		cmd = args.exe + " "  + config + " >& " + logFile
		print( cmd )
		os.system( cmd )

""" Tpc Efficiency """
if "TpcEff" == args.task :
	for plc in plcs :
		for chg in charges :
			for tt in track_types :
				config = os.path.join( args.config, "TpcEff", effc.t_config_file.format( plc=plc, c=chg, tt=tt, ext="xml" ) )
				logFile = config + ".log"	
				cmd = args.exe + " "  + config + " >& " + logFile
				print "Running :"
				print cmd
				os.system( cmd )
				
	config = os.path.join( args.config, "TpcEff", "fit.xml" )
	logFile = config + ".log"
	cmd = args.exe + " "  + config + " >& " + logFile
	print "Running : "
	print cmd
	os.system( cmd )
				

if "FeedDown" == args.task :
	config = os.path.join( args.config, "FeedDown", fdc.t_config_file.format( ext="xml" ) )
	logFile = config + ".log"
	cmd = args.exe + " "  + config + " >& " + logFile
	print "Running : "
	print cmd
	os.system( cmd )


if "PidHisto" == args.task :
	for plc in plcs :
		config = os.path.join( args.config, "PidHisto", pidhc.t_config_file.format( plc=plc, ext="xml" ) )
		logFile = config + ".log"
		os.system( args.exe + " "  + config + " >& " + logFile )



if "All" == args.task :
	# not very clean but it will do
	cmd = "./chain " + args.exe + " EnergyLoss -config " + args.config
	os.system( cmd )
	cmd = "./chain " + args.exe + " TofEff -config " + args.config
	os.system( cmd )
	cmd = "./chain " + args.exe + " TpcEff -config " + args.config
	os.system( cmd )
	cmd = "./chain " + args.exe + " FeedDown -config " + args.config
	os.system( cmd )

	cmd = "./chain " + args.exe + " PidHisto -config " + args.config
	os.system( cmd )
