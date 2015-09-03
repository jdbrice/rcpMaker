#!/usr/bin/env python2.7


""" 
root's hadd gets really inefficiency for large numbers of files - it becomes much more efficienct to do it in small batches. 
This tool hadds some collection of files in smaller batches and gives you the result. maybe in the future ill make it parallelized
"""
import argparse
import os
import glob



def chunks(l, n):
    """Yield successive n-sized chunks from l. - thank you Ned Batchelder : http://stackoverflow.com/users/14343"""
    for i in xrange(0, len(l), n):
        yield l[i:i+n]

def hadd( files, n, final_name="merged.root", prefix="tmp_" ) :

	chunky = list( chunks( files, n ) )

	n_to_make = len(chunky)
	print "Length :", n_to_make

	index = 0
	new_files = []
	for c in chunky :
		
		if n_to_make > 1 :
			new_name = prefix + str(index) + "_" + final_name
		else :
			new_name = final_name

		new_files.append( new_name )
		cmd = "hadd " + new_name + " " + " ".join(c)
		print cmd
		os.system( cmd )
		index = index+1
	print " created :", " ".join( new_files )

	if len( new_files ) > 1 :
		hadd( new_files, n, final_name, "tmp_" + prefix )

	# remove our tmp files
	if len( new_files ) > 1 :
		for tmp_f in new_files :
			print "Removing temp file", tmp_f
			os.remove( tmp_f )




parser = argparse.ArgumentParser( description="Creates the configs for Tasks" );
parser.add_argument( "needle", default="*.root", help="selects all of the files that match the glob" )
parser.add_argument( "N", default=50, nargs='?', type=int, help="split the hadd into N files per batch" )
parser.add_argument( "name", default="merged.root", nargs='?', type=str, help="name of final output file" )


args = parser.parse_args()
files = glob.glob( args.needle )

hadd( files, args.N, args.name )