#ifndef PID_DATA_MAKER_H
#define PID_DATA_MAKER_H

// Rcp Maker
#include "Spectra/InclusiveSpectra.h"
#include "Spectra/ZRecentering.h"
#include "Adapter/PicoDataStore.h"
#include "Params/EnergyLossParams.h"
#include "Params/FeedDownParams.h"
class SpectraCorrecter;

// STL
#include <algorithm> 
#include <vector>
#include <math.h>
using namespace std;

// ROOT
#include "TNtuple.h"


class PidDataMaker : public InclusiveSpectra
{
protected:


	//Configs for corrections
	XmlConfig * cfgEnergyLoss;

	//Binning
	unique_ptr<HistoBins> binsPt;
	vector<int> charges;

	
	// Z Recentering 
	// Which cpecies are we centering around
	string centerSpecies;
	// traditional or nonlinear 
	string zrMethod;
	// ideal sigma for 1/beta and dEdx
	double dedxSigmaIdeal, tofSigmaIdeal;
	// recentering object
	ZRecentering * zr;
	
	unique_ptr<SpectraCorrecter> sc;
	float corrTrackPt = 0;
	float trackPt = 0;
	map< string, unique_ptr<EnergyLossParams> > elParams;
	
	double tpcSysNSigma = 0;

	map<string, unique_ptr<TNtuple>> pidPoints;


public:
	virtual const char* classname() const { return "PidDataMaker"; }
	PidDataMaker( XmlConfig config, string np, string fl ="", string jp ="" );
	~PidDataMaker();

	/**
	 * Analyze a track in the current Event
	 * @iTrack 	- Track index 
	 */
	//virtual void analyzeTrack( Int_t iTrack );
	virtual void analyzeTofTrack( Int_t iTrack );
	virtual void preEventLoop();
	virtual void postEventLoop();
	
	/* Produces the 1D PID plots
	 *
	 * @return 		True / False : pass tof acceptance
	 */
	bool enhanceDistributions( double avgP, int pBin, int charge, double dedx, double tof );

	bool rejectElectron( double avgP, double dedx, double tof );

	/* Average P in a bin range assuming a flat distribution
	 * The distribution is really an exp, but we just need to be consistent
	 * @param  bin 0 indexed bit 
	 * @return     average value in the given bin range
	 */
	double binAverageP( int bin ){
		if ( bin < 0 || bin > binsPt->nBins() ){
			return -1;
		}
		return ((*binsPt)[ bin ] + (*binsPt)[ bin + 1 ]) / 2.0;
	}
	
};

#endif