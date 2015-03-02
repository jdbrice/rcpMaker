#ifndef SIMULTANEOUS_PID_H
#define SIMULTANEOUS_PID_H

#include <iostream>
#include <string>
using namespace std;

/**
 * JDB 
 */
#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "HistoBook.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"
#include "Reporter.h"
using namespace jdb;

#include "HistoAnalyzer.h"
#include "TofPidParams.h"
#include "DedxPidParams.h"
#include "PhaseSpaceRecentering.h"

#include "TH1D.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooExtendPdf.h"

class SimultaneousPid : public HistoAnalyzer
{
protected:

	HistoBins* binsPt;
	HistoBins* binsEta;
	HistoBins* binsCharge;

	double muRoi, sigmaRoi, roi;

	string centerSpecies;
	string psrMethod;
	double dedxSigmaIdeal, tofSigmaIdeal;
	PhaseSpaceRecentering * psr;
	vector<string> species;

	vector<TofPidParams*> tofParams;
	vector<DedxPidParams*> dedxParams;

	

public:

	SimultaneousPid( XmlConfig * config, string np);
	~SimultaneousPid();

	virtual void make();

	RooAbsPdf* gaussModel( int nGauss, RooRealVar * x, string prefix ="" );
	
protected:
	

};


#endif