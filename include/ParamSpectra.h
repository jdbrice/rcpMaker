#ifndef PARAM_SPECTRA
#define PARAM_SPECTRA

#include "InclusiveSpectra.h"
#include "PidParams.h"
#include "PhaseSpaceRecentering.h"
#include "HistogramBins.h"

#include <vector>
#include <string>

class ParamSpectra : public InclusiveSpectra
{
protected:

	vector<string> species;
	vector<PidParams*> pidParams;

	string centerSpecies;
	PhaseSpaceRecentering * psr;

	// Bins
	HistogramBins * binsPt;
	HistogramBins * binsEta;

public:
	ParamSpectra( XmlConfig * config, string np);
	~ParamSpectra(){}

protected:

	virtual void analyzeTrack( Int_t iTrack );
	
};

#endif