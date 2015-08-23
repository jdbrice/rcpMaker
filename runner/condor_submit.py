#!/usr/bin/env python2.7

import argparse
import glob
import os

parser = argparse.ArgumentParser( description="Creates the configs for Tasks" );
parser.add_argument( "project", default="nominal", help="nominal, test, systematic etc.")
parser.add_argument( "job", default="PidHisto_PostCorr_K", help="name")
parser.add_argument( "list_path", default="./config", help="creates the folder and stores all configs within")
parser.add_argument( "config_path", default="K.xml", help="path to config file used for this job, should end in '.xml'")
parser.add_argument( "working_dir", default="./config", help="working directory - must contain dedxBischel.root")
parser.add_argument( "product_path", default="./config", help="working directory - must contain dedxBischel.root")

args = parser.parse_args()

t_header="""
##################################################
# Create Condor Submit files for PID Rcp Analysis
##################################################
Getenv = True
Executable    = {exe}
initialdir 		= {wd}
"""

t_arg = """Arguments = {cfg} {list} {prefix}
Queue
"""
t_job = """
JOB {id} {file}"""
t_parent = """
PARENT {p_id} CHILD {c_id} """


plcs = ( "Pi", "K", "P" )
states = ( "PostCorr", "Corr" )

if not os.path.exists("grid"):
    os.makedirs("grid")
if not os.path.exists(os.path.join( "grid", args.project ) ):
    os.makedirs(os.path.join( "grid", args.project ) )


hadd = os.path.join( "grid", args.project, args.job + ".hadd" )
dag = os.path.join( "grid", args.project, args.job + ".dag" )
wd = os.path.dirname(os.path.realpath(__file__))


for plc in plcs :
	for state in states :
		name = os.path.join( "grid", ( args.job  + "_" + state + "_" + plc + ".submit" ) )
		print "writing to", name
		with open( name, 'w' ) as of :
			list_files = glob.glob( os.path.join( args.list_path, "list_*" ) )
			print "Found", len(list_files), "list files"
			
			of.write( t_header.format( exe=os.path.join( args.working_dir, "rcp" ), wd=args.working_dir ) )

			for f in list_files :
				prefix = f.split( '_' )[-1]
				of.write( t_arg.format( cfg=os.path.join( args.config_path, state + "_" + plc + ".xml"), list = f, prefix = state + "_" + plc + "_" + prefix +"_" ) )

# with open( hadd, 'w' ) as of :
# 	of.write( t_header.format( exe="hadd", wd=args.product_path ) )
# 	of.write( t_arg.format( cfg="output.root", list=(plc + "_*.root"), prefix="" ) )


# with open( dag, 'w' ) as of :
# 	of.write( t_job.format( id="A", file=os.path.join( wd, name ) ) )
# 	of.write( t_job.format( id="B", file=os.path.join( wd, hadd ) ) )
# 	of.write( t_parent.format( p_id="A", c_id="B" ) )

