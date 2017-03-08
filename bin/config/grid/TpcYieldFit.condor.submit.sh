
##################################################
# Create Condor Submit files for PID Rcp Analysis
##################################################
Getenv = True
Executable      = /home/jdb12/work/rcpMaker/bin/rcp
initialdir      = /home/jdb12/work/rcpMaker/bin/

Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=-1 --cen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=-1 --cen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=-1 --cen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=-1 --cen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=-1 --cen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=-1 --cen=5
Queue 1


Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=1 --cen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=1 --cen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=1 --cen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=1 --cen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=1 --cen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=Pi --charge=1 --cen=5
Queue 1


# K
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=-1 --cen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=-1 --cen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=-1 --cen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=-1 --cen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=-1 --cen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=-1 --cen=5
Queue 1


Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=1 --cen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=1 --cen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=1 --cen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=1 --cen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=1 --cen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=K --charge=1 --cen=5
Queue 1

# P
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=-1 --cen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=-1 --cen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=-1 --cen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=-1 --cen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=-1 --cen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=-1 --cen=5
Queue 1


Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=1 --cen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=1 --cen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=1 --cen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=1 --cen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=1 --cen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/TpcYieldFit.xml --plc=P --charge=1 --cen=5
Queue 1


