#ifndef PID_PHASE_SPACE_H
#define PID_PHASE_SPACE_H

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


class PidHistoMaker : public InclusiveSpectra
{
protected:


	//Configs for corrections
	XmlConfig * cfgEnergyLoss;
	XmlConfig * cfgFeedDown;


	//Binning
	unique_ptr<HistoBins> binsTof;
	unique_ptr<HistoBins> binsDedx;
	unique_ptr<HistoBins> binsPt;
	vector<int> charges;
	// these are made on the fly based on the config options
	double tofBinWidth, dedxBinWidth;

	// Plot Ranges 
	double tofPadding, dedxPadding, tofScalePadding, dedxScalePadding;

	
	// Z Recentering 
	// Which cpecies are we centering around
	string centerSpecies;
	// traditional or nonlinear 
	string zrMethod;
	// ideal sigma for 1/beta and dEdx
	double dedxSigmaIdeal, tofSigmaIdeal;
	// recentering object
	ZRecentering * zr;
	
	// cuts used for enhancing distributions
	double tofCut, dedxCut;

	// options for histogram creation
	bool make2D, makeEnhanced;

	// # of sigma to cut above and below
	double nSigBelow, nSigAbove;

	bool makeCombinedCharge = false;

	unique_ptr<SpectraCorrecter> sc;
	float trackPt = 0;
	map< string, unique_ptr<EnergyLossParams> > elParams;
	


	// apply feed down correction
	bool correctFeedDown = false;
	// apply tpc eff correction
	bool correctTpcEff = false;
	// apply tof eff correction
	bool correctTofEff = false;

public:
	PidHistoMaker( XmlConfig* config, string np, string fl ="", string jp ="" );
	~PidHistoMaker();

	/**
	 * Analyze a track in the current Event
	 * @iTrack 	- Track index 
	 */
	//virtual void analyzeTrack( Int_t iTrack );
	virtual void analyzeTofTrack( Int_t iTrack );
	virtual void preEventLoop();
	virtual void postEventLoop();
	

	void enhanceDistributions( double avgP, int pBin, int charge, double dedx, double tof );


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


	/**
	 * Computes the upper and lower limits in tof and dedx histogram space
	 * @pType             centering plc
	 * @p                 momentum transvers [GeV/c]
	 * @tofLow            double* receiving tofLow limit
	 * @tofHigh           double* receiving tofHigh limit
	 * @dedxLow           double* receiving dedxLow limit
	 * @dedxHigh          double* receiving dedxHigh limit
	 * @tofPadding        fixed value of padding applied to tof range
	 * @dedxPadding       fixed value of padding applied to dedx range
	 * @tofScaledPadding  percent padding added to tof range
	 * @dedxScaledPadding percent padding added to dedx range
	 */
	static void autoViewport( 	string pType, double p, ZRecentering * lpsr, double * tofLow, double* tofHigh, double * dedxLow, double * dedxHigh, 
								double tofPadding = 1, double dedxPadding = 1, double tofScaledPadding = 0, double dedxScaledPadding = 0 );


	void reportAll( string n );
	// void reportAllTof(  );
	// void reportAllDedx(  );

protected:

	void prepareHistograms( string plc );
	
};

#endif