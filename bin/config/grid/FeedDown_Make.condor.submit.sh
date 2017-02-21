
##################################################
# Create Condor Submit files for PID Rcp Analysis
##################################################
Getenv = True
Executable    = /home/jdb12/work/rcpMaker/bin/rcp
initialdir    = /home/jdb12/work/rcpMaker/bin/

log           = /home/jdb12/work/rcpMaker/bin/config/grid/log_FeedDown_Make.condor.submit

Arguments = /home/jdb12/work/rcpMaker/bin/config/FeedDown/Make.xml --jobIndex=$(Process) --maxJobIndex=119
Queue 119