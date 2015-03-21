#ifndef PID_PARAM_MAKER_H
#define PID_PARAM_MAKER_H

/**
 * STL
 */
#include <memory>

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
	Logger * lg = nullptr;
	XmlConfig * cfg = nullptr;
	string nodePath;

	HistoBook * book = nullptr;
	vector< unique_ptr<Reporter> > tofReport;
	vector< unique_ptr<Reporter> > dedxReport;

	TFile * inFile;

	HistoBins* binsPt;
	HistoBins* binsEta;
	HistoBins* binsCharge;

	// 
	double 	muRoi 		= 2.5, 
			sigmaRoi 	= 3.5, 
			roi 		= 2.5, 
			window 		= 1.5;


	string centerSpecies;
	string psrMethod;
	double dedxSigmaIdeal, tofSigmaIdeal;
	PhaseSpaceRecentering * psr;
	vector<string> species;

	vector<double> mmtms;
	


	

public:

	PidParamMaker( XmlConfig * config, string np);
	~PidParamMaker();

	void make();

	class GaussianFitResult{
	public:
		double mu, muError;		
		double sigma, sigmaError;
	};

	static double meanFunction( double * x, double * pars );
	static double sigmaFunction( double * x, double * pars );

protected:

	GaussianFitResult fitSingleSpecies( TH1D* h, double x1, double x2, Reporter &rp, string title = "" );
	void reportTofFitResult( string s, Reporter &rp );
	void reportDedxFitResult( string s, Reporter &rp );
	map< string, vector<GaussianFitResult> > tofFit;
	map< string, vector<GaussianFitResult> > dedxFit;



};


#endif