#ifndef TOF_EFF_SPECTRA_H
#define TOF_EFF_SPECTRA_H

#include "Spectra/InclusiveSpectra.h"
#include "Spectra/ZRecentering.h"

class TofEffSpectra : public InclusiveSpectra {

public:
	TofEffSpectra( XmlConfig config, string nodePath, string fileList ="", string jobPrefix ="" );
	~TofEffSpectra();


	// Z Recentering 
	// Which cpecies are we centering around
	string centerSpecies;
	// traditional or nonlinear 
	string zrMethod;
	// ideal sigma for 1/beta and dEdx
	double dedxSigmaIdeal, tofSigmaIdeal;
	// recentering object
	ZRecentering * zr;

	double nSigmaDedxCut;

protected:
	virtual void makeCentralityHistos();

	virtual void analyzeTrack( Int_t iTrack );
	
	virtual void analyzeTofTrack( Int_t iTrack );

};

#endif