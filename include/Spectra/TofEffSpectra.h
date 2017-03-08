#ifndef TOF_EFF_SPECTRA_H
#define TOF_EFF_SPECTRA_H

#include "Spectra/InclusiveSpectra.h"
#include "Spectra/ZRecentering.h"

// #define LOGURU_IMPLEMENTATION 1
// #include "loguru.h"

class TofEffSpectra : public InclusiveSpectra {

public:
	virtual const char* classname() const { return "TofEffSpectra"; }
	TofEffSpectra() {}
	~TofEffSpectra();

	virtual void initialize();

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

	string toString(){
		stringstream ss;

		ss << "<" << classname() << ">";
		ss << "\n";
		ss << "\tcenterSpecies=" << quote( centerSpecies ) << "\n";
		ss << "\tzrMethod=" << quote( zrMethod ) << "\n";
		ss << "\tdedxSigmaIdeal=" << dedxSigmaIdeal << "\n";
		ss << "\ttofSigmaIdeal=" << tofSigmaIdeal << "\n";
		ss << "\tnSigmaDedxCut=" << nSigmaDedxCut << "\n";

		return ss.str();
	}

protected:
	virtual void makeCentralityHistos();

	virtual void analyzeTrack( Int_t iTrack, bool isTofTrack );
	
	virtual void analyzeTofTrack( Int_t iTrack );

};

#endif