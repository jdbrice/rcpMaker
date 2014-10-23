#ifndef SIMULTANEOUS_GAUSSIAN_H
#define SIMULTANEOUS_GAUSSIAN_H

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


#include "RooExtendPdf.h"

class SimultaneousGaussians
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

	SimultaneousGaussians( XmlConfig * config, string np);
	~SimultaneousGaussians();

	void make();

	class GaussianFitResult{
	public:
		double mu, muError;		
		double sigma, sigmaError;
	};

protected:

	GaussianFitResult fitThreeSpecies( TH1D* h, vector<double> x1, vector<double> x2 );
	RooExtendPdf fitSingleSpecies( TH1D* h, double x1, double x2, string dropt, int c );
	
};


#endif