#ifndef PARAM_SPECTRA
#define PARAM_SPECTRA

#include "InclusiveSpectra.h"
#include "TofPidParams.h"
#include "DedxPidParams.h"
#include "PhaseSpaceRecentering.h"
#include "HistoBins.h"
#include "Reporter.h"
#include <vector>
#include <string>

class ParamSpectra : public InclusiveSpectra
{
protected:

	vector<string> species;
	vector<TofPidParams*> tofParams;
	vector<DedxPidParams*> dedxParams;

	string centerSpecies;
	double tofSigmaIdeal, dedxSigmaIdeal;
	PhaseSpaceRecentering * psr;
	string psrMethod;

	// Bins
	HistoBins * binsPt;
	HistoBins * binsEta;

	// Cutting
	double nSigmaTof;
	double nSigmaDedx;
	string nSigmaType;
	
	static string DEDX_CUT;
	static string TOF_CUT;
	static string SQUARE_CUT;
	static string ELLIPSE_CUT;
	vector<string> cuts;

public:
	ParamSpectra( XmlConfig * config, string np, string fileList ="", string jobPrefix ="");
	~ParamSpectra();

	virtual void analyzeEvent();
	virtual void analyzeTrack( Int_t iTrack );
	virtual void preEventLoop();
	virtual void postEventLoop();
	string chargeName( int charge ) {
		if ( 0 > charge  )
			return "n";
		else if ( 0 < charge  )
			return "p";
		return "a";
	}
	virtual string histoForCentrality( string cent, string pType, string pidCut, int charge = 0 )
			{ return "pt_" + pidCut + "_" + pType + "_" + chargeName( charge ) + "_" + cent; }
	virtual void makeCentralityHistos();

	vector<string> pid( string type, double p, double dedx, double tof, double avgP );
	vector<string> pidTof( double p, double tof );
	vector<string> pidDedx( double p, double dedx );
	vector<string> pidSquare( double p, double dedx, double tof );
	vector<string> pidEllipse( double p, double dedx, double tof, double avgP );

	
};

#endif