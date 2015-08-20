#!/usr/bin/env python2.7

import argparse
import glob
import os

parser = argparse.ArgumentParser( description="Creates the configs for Tasks" );
parser.add_argument( "name", default="PidHisto_PostCorr_K", help="name")
parser.add_argument( "list_path", default="./config", help="creates the folder and stores all configs within")
parser.add_argument( "config_file", default="K.xml", help="path to config file used for this job, should end in '.xml'")
parser.add_argument( "submit_path", default="./config", help="creates the folder and stores all configs within")
parser.add_argument( "exe_path", default="./config", help="creates the folder and stores all configs within")

args = parser.parse_args()

t_header="""
##################################################
# Create Condor Submit files for PID Rcp Analysis
##################################################

Executable    = {exe}
initialdir 		= {wd}
"""

t_arg = """
Arguments = {cfg} {list} {prefix}
Queue
"""


if args.name.endswith( ".submit" ):
	name = args.name
else :
	name = args.name + ".submit"

print "writing to", name

plc = os.path.basename( args.config_file ).split( "." )[ 0 ]


with open( name, 'w' ) as of :
	list_files = glob.glob( os.path.join( args.list_path, "list_*" ) )
	print "Found", len(list_files), "list files"
	
	of.write( t_header.format( exe="./rcp", wd="./" ) )

	for f in list_files :

		

		print f
		prefix = f.split( '_' )[-1]
		print plc + "_" + prefix 

		of.write( t_arg.format( cfg=args.config_file, list = f, prefix = plc + "_" + prefix  ) )

