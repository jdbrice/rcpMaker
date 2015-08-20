#!/usr/bin/env python2.7

import argparse
import glob
import os

parser = argparse.ArgumentParser( description="Creates the configs for Tasks" );
parser.add_argument( "list_path", default="./config", help="creates the folder and stores all configs within")
parser.add_argument( "config_file", default="K.xml", help="path to config file used for this job, should end in '.xml'")
parser.add_argument( "submit_path", default="./config", help="creates the folder and stores all configs within")
parser.add_argument( "exe_path", default="./config", help="creates the folder and stores all configs within")

args = parser.parse_args()

t_submit="""
#############################################
# Create PidHisto files for PID Rcp Analysis
#############################################

Executable    = {exe}
initialdir 		= {wd}
"""

t_arg = """
Arguments = {cfg} {list} {prefix}
Queue
"""

plc = args.config_file[-4:]


list_files = glob.glob( os.path.join( args.list_path, "list_*" ) )

for f in list_files :
	print f
	prefix = f.split( '_' )[-1]
	print prefix + "_" + plc

