
##################################################
# Create Condor Submit files for PID Rcp Analysis
##################################################
Getenv = True
Executable    = /home/jdb12/work/rcpMaker/bin/rcp
initialdir 		= /home/jdb12/work/rcpMaker/bin/

Arguments = /home/jdb12/work/rcpMaker/bin/config/PidData_Make.xml --jobIndex=$(Process) --maxJobIndex=119 --plc=Pi
Queue 119

Arguments = /home/jdb12/work/rcpMaker/bin/config/PidData_Make.xml --jobIndex=$(Process) --maxJobIndex=119 --plc=K
Queue 119

Arguments = /home/jdb12/work/rcpMaker/bin/config/PidData_Make.xml --jobIndex=$(Process) --maxJobIndex=119 --plc=P
Queue 119