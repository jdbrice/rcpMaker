
##################################################
# Create Condor Submit files for PID Rcp Analysis
##################################################
Getenv = True
Executable    = /home/jdb12/work/rcpMaker/bin/rcp
initialdir    = /home/jdb12/work/rcpMaker/bin/

log           = /home/jdb12/work/rcpMaker/bin/config/grid/log_ZdData_Make.condor.submit

Arguments = /home/jdb12/work/rcpMaker/bin/config/ZdData/MakeHistos.xml --jobIndex=$(Process) --maxJobIndex=120 --plc=Pi
Queue 120

Arguments = /home/jdb12/work/rcpMaker/bin/config/ZdData/MakeHistos.xml --jobIndex=$(Process) --maxJobIndex=120 --plc=K
Queue 120

Arguments = /home/jdb12/work/rcpMaker/bin/config/ZdData/MakeHistos.xml --jobIndex=$(Process) --maxJobIndex=120 --plc=P
Queue 120