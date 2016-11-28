#!/bin/bash

echo "Cleaning pre-merge artifacts for /data/star1/jdb12/RcpAnalysis/products/$1/"

read -p "Are you sure? " -n 1 -r
echo    # (optional) move to a new line
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
    exit 1
fi


echo "rm /data/star1/jdb12/RcpAnalysis/products/$1/FeedDown_*.root"
rm -f /data/star1/jdb12/RcpAnalysis/products/$1/FeedDown_*.root

echo "rm /data/star1/jdb12/RcpAnalysis/products/$1/PidData_PostCorr_Pi_*.root"
echo "rm /data/star1/jdb12/RcpAnalysis/products/$1/PidData_PostCorr_K_*.root"
echo "rm /data/star1/jdb12/RcpAnalysis/products/$1/PidData_PostCorr_P_*.root"

rm -f /data/star1/jdb12/RcpAnalysis/products/$1/PidData_PostCorr_Pi_*.root
rm -f /data/star1/jdb12/RcpAnalysis/products/$1/PidData_PostCorr_K_*.root
rm -f /data/star1/jdb12/RcpAnalysis/products/$1/PidData_PostCorr_P_*.root

echo "rm /data/star1/jdb12/RcpAnalysis/products/$1/Fit_PostCorr_{Pi,K,P}_iCharge{-1,1}_iCen*.root"
rm -f /data/star1/jdb12/RcpAnalysis/products/$1/Fit_PostCorr_{Pi,K,P}_iCharge{-1,1}_iCen*.root