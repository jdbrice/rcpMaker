#!/usr/bin/env python2.7
import TpcEff.make_xml_configs as effc
import EnergyLoss.make_xml_configs as elossc
import PidHisto.make_xml_configs as pidhc
import PidData.make_xml_configs as pidtc
import TofEff.make_xml_configs as tofc
import FeedDown.make_xml_configs as fdc
import Fitter.make_xml_configs as fitc
import PostCorr.make_xml_configs as postc
import Present.make_xml_configs as presc
import argparse
import os
import glob
import shutil


parser = argparse.ArgumentParser( description="Creates the configs for Tasks" );
parser.add_argument( "config_base_path", default="./config", help="creates the folder and stores all configs within")
parser.add_argument( "-data_path", required=True, help="Path to folder containg embedding data in rcpPicoDst format. Files should be named {plc}_{charge_str}_{mc,rc}.root" )
parser.add_argument( "-output_path", required=True, help="Path to folder to output products" )
parser.add_argument( "-output_config_path", required=True, help="Path to folder to output config products" )
parser.add_argument( "-common_path", required=True, help="Path to folder to containing common configs" )

args = parser.parse_args()

# if we got the arguments we need
# first create the config_base_path
if not os.path.exists(args.config_base_path):
    os.makedirs(args.config_base_path)

"""Correction Makers"""
#	Tpc Efficiency
effc.write( os.path.join( args.data_path, "embedding" ) , args.output_path, args.output_config_path, os.path.join( args.config_base_path, "TpcEff/") )
#	Energy Loss
elossc.write( os.path.join( args.data_path, "embedding" ), args.output_path, args.output_config_path, os.path.join( args.config_base_path, "EnergyLoss/") )
#	Tof Efficiency
tofc.write( os.path.join( args.data_path, "data", "RcpPicoDst" ), args.output_path, args.output_config_path, os.path.join( args.config_base_path, "TofEff/") )
#	FeedDown Maker
fdc.write( os.path.join( args.data_path, "urqmd" ), args.output_path, args.output_config_path, os.path.join( args.config_base_path, "FeedDown/") )



"""Pid Makers"""
#PidHisto
#pidhc.write( os.path.join( args.data_path, "data", "RcpPicoDst" ), args.output_path, args.output_config_path, os.path.join( args.config_base_path, "PidHisto/") )
#PidTree
pidtc.write( os.path.join( args.data_path, "data", "RcpPicoDst" ), args.output_path, args.output_config_path, os.path.join( args.config_base_path, "PidData/") )
# Pif Fitter
fitc.write( args.output_path, os.path.join( args.config_base_path, "Fitter/") )


"""Post Fit Corrections"""
postc.write( args.output_path, os.path.join( args.config_base_path, "PostCorr/") )

"""Presentation"""
presc.write(args.output_path, os.path.join( args.config_base_path, "Present/") )

#Install the common configs - you may need to edit these by hand

common_path = os.path.join( args.config_base_path, "Common" )
if not os.path.exists( common_path  ):
    os.makedirs(common_path)
for f in glob.glob( os.path.join( args.common_path, "*" ) ) :
	shutil.copy( f, common_path )

#create the dir for output params
params_path = os.path.join( args.config_base_path, "Params" )
if not os.path.exists( params_path  ):
    os.makedirs(params_path)