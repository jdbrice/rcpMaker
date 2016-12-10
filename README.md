#rcpMaker
####Produces the Identified particle spectra for BES R_CP analysis
================================================================

Visible at [README.md](https://github.com/jdbrice/rcpMaker/blob/master/README.md)
The parts inside back-ticks (\`) should be executed exactly - ie copy and paste into terminal (except for $cwd - which is the path you choose). If it doesn't worh then something is wrong, let me know.

1. Obtain this code:
 - `cvs co offline/paper/psn0662/rcpMaker`
2. Prepare a data directory somewhere that all nodes can "see". Eg /star/data03/pwg/username/rcpCodeQA/. I'll refer to this as your $cwd
3. Open bin/config/Common/env.xml and modify the cwd="..." to use your $cwd (from step 2.)
4. `mkdir $cwd/img` and `mkdir $cwd/Params`
5. Submit jobs for TpcEff. These Jobs skim the data to produce the MC and RC histograms and then divides + fits. This step produces a report called "rp_TpcEff.pdf" in the $cwd
 - inside bin: `star-submit config/star/TpcEff.submit.xml`
 - This step will take about 20 minutes after it starts running
6. Submit jobs for the Yield Extraction fits.
 - inside bin: `star-submit config/star/YieldFit.submit.xml`
 - This step may take a while, wait till it is complete
7. inside bin: `./MakeCorrectedSpectra.sh`
8. inside bin: `./MakeRcpPlots.sh`
 - Make sure to run this without X11 forwarding or it will be very slow

The plots will be in $cwd/img/Rcp_*