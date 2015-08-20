#!/usr/bin/env python2.7


import os
import subprocess as sub
import time


wd = os.path.dirname(os.path.realpath(__file__))
print os.path.dirname(os.path.realpath(__file__))

cmd = os.path.join( wd, 'n_jobs.sh' )
res = sub.check_output( [cmd] )

print int(res)


while int(sub.check_output( [cmd] )) >= 1 :
	time.sleep( 60 )

