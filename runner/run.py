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
import Fitter.make_xml_configs as fitc
import PostCorr.make_xml_configs as postc
import Present.make_xml_configs as presc

parser = argparse.ArgumentParser( description="Runs the Rcp Analysis" );
parser.add_argument("exe", help="path to executable")
parser.add_argument("-config", help="Base path for config files", default="./" )

parser.add_argument("task", help="Task to run. Can be one of : \n 'All', 'EnergyLoss', 'TpcEff', 'TofEff', 'FeedDown', 'PidHisto', 'Fit', 'PostCorr', 'Present', 'PostHisto', 'Corrections'" )

args = parser.parse_args();

exeMe = "./run.py "

print args

plcs = ( "Pi", "K", "P" )
charges = ( "p", "n" )
track_types = ( "mc", "rc" )
states = ( "Corr", "PostCorr" )

t_cmd = "{exe} {args} >& {log}"


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

	config = os.path.join( args.config, "TofEff", "fit.xml" )
	logFile = config + ".log"
	cmd = args.exe + " "  + config + " >& " + logFile
	print "Running : "
	print cmd
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
	cmd = t_cmd.format( exe=args.exe, args=config, log=logFile )
	print "Running : "
	print cmd
	os.system( cmd )
				

if "FeedDown" == args.task :
	config = os.path.join( args.config, "FeedDown", fdc.t_config_file.format( ext="xml" ) )
	logFile = config + ".log"
	cmd = t_cmd.format( exe=args.exe, args=config, log=logFile )
	print "Running : "
	print cmd
	os.system( cmd )


if "PidHisto" == args.task :
	for state in states :
		for plc in plcs :
			config = os.path.join( args.config, "PidHisto", pidhc.t_config_file.format( state=state, plc=plc, ext="xml" ) )
			logFile = config + ".log"
			if not os.path.isfile( config ) :
				print "Skipping :", config
				continue
			cmd = t_cmd.format( exe=args.exe, args=config, log=logFile )
			print "Running : "
			print cmd
			os.system( cmd )

if "Fit" == args.task :
	for state in states :
		for plc in plcs :
			config = os.path.join( args.config, "Fitter", fitc.t_config_file.format( state=state, plc=plc, ext="xml" ) )
			if not os.path.isfile( config ) :
				print "Skipping :", config
				continue
			logFile = config + ".log"
			cmd = t_cmd.format( exe=args.exe, args=config, log=logFile )
			print "Running : "
			print cmd
			os.system( cmd )

if "PostCorr" == args.task :
	for plc in plcs :
		config = os.path.join( args.config, "PostCorr", postc.t_config_file.format( plc=plc, ext="xml" ) )
		logFile = config + ".log"
		cmd = t_cmd.format( exe=args.exe, args=config, log=logFile )
		print "Running : "
		print cmd
		os.system( cmd )

if "Present" == args.task :
	for state in ( "Corr", "PostCorr" ) :
		for plc in plcs :
			config = os.path.join( args.config, "Present", presc.t_config_file.format( state=state, plc=plc, ext="xml" ) )
			logFile = config + ".log"
			if not os.path.isfile( config ) :
				print "Skipping :", config
				continue
			cmd = t_cmd.format( exe=args.exe, args=config, log=logFile )
			print "Running : "
			print cmd
			os.system( cmd )


if "PostHisto" == args.task :
	# not very clean but it will do
	cmd = exeMe + args.exe + " Fit -config " + args.config
	os.system( cmd )

	cmd = exeMe + args.exe + " PostCorr -config " + args.config
	os.system( cmd )
	cmd = exeMe + args.exe + " Present -config " + args.config
	os.system( cmd )


if "Corrections" == args.task :
	# not very clean but it will do
	cmd = exeMe + args.exe + " EnergyLoss -config " + args.config
	os.system( cmd )
	cmd = exeMe + args.exe + " TofEff -config " + args.config
	os.system( cmd )
	cmd = exeMe + args.exe + " TpcEff -config " + args.config
	os.system( cmd )
	cmd = exeMe + args.exe + " FeedDown -config " + args.config
	os.system( cmd )


if "All" == args.task :
	# not very clean but it will do
	cmd = exeMe + args.exe + " Corrections -config " + args.config
	os.system( cmd )

	cmd = exeMe + args.exe + " PidHisto -config " + args.config
	os.system( cmd )
	
	cmd = exeMe + args.exe + " PostHisto -config " + args.config
	os.system( cmd )

	