#ifndef PARAM_SPECTRA
#define PARAM_SPECTRA

#include "InclusiveSpectra.h"
#include "TofPidParams.h"
#include "PhaseSpaceRecentering.h"
#include "HistoBins.h"

#include <vector>
#include <string>

class ParamSpectra : public InclusiveSpectra
{
protected:

	vector<string> species;
	vector<TofPidParams*> tofParams;

	string centerSpecies;
	PhaseSpaceRecentering * psr;

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
	ParamSpectra( XmlConfig * config, string np);
	~ParamSpectra(){}

protected:

	virtual void analyzeTrack( Int_t iTrack );
	virtual void preLoop();

	string pid( string type, double p, double dedx, double tof );
	string pidTof( double p, double tof );
	string pidDedx( double p, double dedx );
	string pidSquare( double p, double dedx, double tof );
	string pidEllipse( double p, double dedx, double tof );

	
};

#endif