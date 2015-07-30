#ifndef TOF_EFF_MAKER_H
#define TOF_EFF_MAKER_H

// Rcp Maker
#include "Spectra/InclusiveSpectra.h"
#include "Spectra/ZRecentering.h"
#include "Adapter/PicoDataStore.h"

// STL
#include <algorithm> 
#include <vector>
#include <math.h>
using namespace std;


class TofEffMaker : public InclusiveSpectra
{
protected:

	//Binning
	unique_ptr<HistoBins> binsTof;
	unique_ptr<HistoBins> binsDedx;
	unique_ptr<HistoBins> binsPt;
	unique_ptr<HistoBins> binsEta;
	vector<int> charges;
	// these are made on the fly
	double tofBinWidth, dedxBinWidth;
	
	// Phase Space Recentering 
	// Only used to apply the rapidity cut
	string centerSpecies;
	string psrMethod;
	double dedxSigmaIdeal, tofSigmaIdeal;
	ZRecentering * psr;


public:
	TofEffMaker( XmlConfig* config, string np, string fl ="", string jp ="" );
	~TofEffMaker();

	/**
	 * Analyze a track in the current Event
	 * @iTrack 	- Track index 
	 */
	virtual void analyzeTrack( Int_t iTrack );
	virtual void analyzeTofTrack( Int_t iTrack );
	virtual void preEventLoop();
	virtual void postEventLoop();

protected:

	double rapidity( double pt, double eta, double m ){
		double a = sqrt( m*m + pt*pt*cosh( eta )*cosh( eta ) ) + pt * sinh( eta );
		double b = sqrt( m*m + pt*pt );
		return log( a / b );
	}
	
	
};

#endif