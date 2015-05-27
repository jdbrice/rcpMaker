#ifndef PID_SPECTRA_MAKER_H
#define PID_SPECTRA_MAKER_H


// RcpMaker
#include "InclusiveSpectra.h"
#include "PhaseSpaceRecentering.h"
#include "Adapter/PicoDataStore.h"
#include "PidPhaseSpace.h"
#include "PidProbabilityMapper.h"

// STL
#include <math.h>
#include <algorithm>    // std::find
#include <vector>
using namespace std;



class PidSpectraMaker : public InclusiveSpectra
{
protected:

	/**
	 * Binning
	 */
	unique_ptr<HistoBins> binsPt;
	unique_ptr<HistoBins> binsEta;
	unique_ptr<HistoBins> binsCharge;

	/**
	 * Phase Space Recentering
	 */
	string centerSpecies;
	string psrMethod;
	double dedxSigmaIdeal, tofSigmaIdeal;
	PhaseSpaceRecentering * psr;
	
	double tofCut, dedxCut;

	double nSigBelow, nSigAbove;

	PidProbabilityMapper * ppm;

public:
	PidSpectraMaker( XmlConfig* config, string np, string fl ="", string jp ="" );
	~PidSpectraMaker();

	/**
	 * Analyze a track in the current Event
	 * @iTrack 	- Track index 
	 */
	virtual void analyzeTrack( Int_t iTrack );
	virtual void preEventLoop();
	virtual void postEventLoop();
	
protected:

	void prepareHistograms( );

	double averageP( int ptBin, int etaBin ){
		if ( ptBin < 0 || ptBin > binsPt->nBins() ){
			return 0;
		}
		if ( etaBin < 0 || etaBin > binsEta->nBins() ){
			return 0;
		} 

		double avgPt = ((*binsPt)[ ptBin ] + (*binsPt)[ ptBin + 1 ]) / 2.0;
		double avgEta = ((*binsEta)[ etaBin ] + (*binsEta)[ etaBin + 1 ]) / 2.0;

		return PidPhaseSpace::p( avgPt, avgEta );

	}
	
};

#endif