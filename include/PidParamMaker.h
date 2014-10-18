#ifndef PID_PARAM_MAKER_H
#define PID_PARAM_MAKER_H

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

#include "PhaseSpaceRecentering.h"

#include "TH1D.h"


class PidParamMaker
{
protected:
	Logger * lg;
	XmlConfig * cfg;
	string nodePath;

	HistoBook * book;
	Reporter * reporter;

	TFile * inFile;

	HistoBins* binsPt;
	HistoBins* binsEta;
	HistoBins* binsCharge;

	double muRoi, sigmaRoi, roi;

	string centerSpecies;
	string psrMethod;
	double dedxSigmaIdeal, tofSigmaIdeal;
	PhaseSpaceRecentering * psr;
	vector<string> species;

	

public:

	PidParamMaker( XmlConfig * config, string np);
	~PidParamMaker();

	void make();

	class GaussianFitResult{
	public:
		double mu, muError;		
		double sigma, sigmaError;
	};

protected:

	GaussianFitResult fitSingleSpecies( TH1D* h, double x1, double x2 );
	
};


#endif