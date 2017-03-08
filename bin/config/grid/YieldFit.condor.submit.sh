
##################################################
# Create Condor Submit files for PID Rcp Analysis
##################################################
Getenv = True
Executable      = /home/jdb12/work/rcpMaker/bin/rcp
initialdir      = /home/jdb12/work/rcpMaker/bin/

Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=-1 --iCen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=-1 --iCen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=-1 --iCen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=-1 --iCen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=-1 --iCen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=-1 --iCen=5
Queue 1


Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=1 --iCen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=1 --iCen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=1 --iCen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=1 --iCen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=1 --iCen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=Pi --iCharge=1 --iCen=5
Queue 1


# K
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=-1 --iCen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=-1 --iCen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=-1 --iCen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=-1 --iCen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=-1 --iCen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=-1 --iCen=5
Queue 1


Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=1 --iCen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=1 --iCen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=1 --iCen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=1 --iCen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=1 --iCen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=K --iCharge=1 --iCen=5
Queue 1

# P
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=-1 --iCen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=-1 --iCen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=-1 --iCen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=-1 --iCen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=-1 --iCen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=-1 --iCen=5
Queue 1


Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=1 --iCen=0
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=1 --iCen=1
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=1 --iCen=2
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=1 --iCen=3
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=1 --iCen=4
Queue 1
Arguments = /home/jdb12/work/rcpMaker/bin/config/YieldFit.xml --plc=P --iCharge=1 --iCen=5
Queue 1


