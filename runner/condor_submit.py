#!/usr/bin/env python2.7

import argparse
import glob
import os

parser = argparse.ArgumentParser( description="Creates the configs for Tasks" );
parser.add_argument( "config_path", default="./config", help="creates the folder and stores all configs within")
parser.add_argument( "submit_path", default="./config", help="creates the folder and stores all configs within")
parser.add_argument( "list_path", default="./config", help="creates the folder and stores all configs within")
parser.add_argument( "exe_path", default="./config", help="creates the folder and stores all configs within")



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


list_files = glob.glob( os.path.join( list_path, "list_*" ) )

for f in list_files :
	print f


