#ifndef INCLUSIVE_SPECTRA_H
#define INCLUSIVE_SPECTRA_H 



// JDB 
#include "TreeAnalyzer.h"
#include "XmlConfig.h"
#include "Utils.h"
#include "Logger.h"
#include "ConfigRange.h"
using namespace jdb;

// ROOT
#include "TChain.h"

// STL
#include <memory>
#include <vector>
#include <math.h>

// RcpMaker
#include "Adapter/PicoDataStore.h"
#include "Common.h"

class InclusiveSpectra : public TreeAnalyzer
{
protected:


	// Assume all particles are of given species for Rapidity determination
	//  Used to calculate the mass
	string plc = "Pi";
	// Assume mass for Rapidity cut
	float mass = Common::mass( plc );

	unique_ptr<PicoDataStore> pico;
	
	// Track cuts loaded from config
	
	// nHitsFit
	unique_ptr<ConfigRange> cut_nHitsFit;
	// Distance to closest approach [cm]
	unique_ptr<ConfigRange> cut_dca;
	// nHitsFit / nHitsPossible
	unique_ptr<ConfigRange> cut_nHitsFitOverPossible;
	// nHitDedx
	unique_ptr<ConfigRange> cut_nHitsDedx;
	// Minimum pT [GeV/c]
	unique_ptr<ConfigRange> cut_pt;
	// ptGlobal / ptPrimary
	unique_ptr<ConfigRange> cut_ptGlobalOverPrimary;
	// Rapidity based on mass assumption
	unique_ptr<ConfigRange> cut_rapidity;


	// Tof Track Cuts

	// Tof Match Flag 0 - no match, 1 - match to 1 tray, 2 or more - multi-match
	unique_ptr<ConfigRange> cut_matchFlag;
	// yLocal [cm]
	unique_ptr<ConfigRange> cut_yLocal;
	// zLocal [cm]
	unique_ptr<ConfigRange> cut_zLocal;

	

	// Centrality bins
	// maps bins from bin9 RMC into some other binning
	// used to combine bins
	map<int, int> centralityBinMap;
	// the vector of bins in the mapped space - makes it easy to loop over
	vector<int> centralityBins;

	// checking bad runs
	static vector<int> badRuns;

	// Make QA for  Track distributions
	bool makeTrackQA;

	// Make QA for Event info 
	bool makeEventQA;

	// make spectra - subclasses may not need the inclusive spectra histos
	bool makeSpectra;

	// make the tof spectra
	bool makeTofSpectra;

	// Current Event Info
	float 	refMult 		= -1;
	double 	eventWeight 	= 1.0;
	int 	cBin = -1;



public:
	virtual const char* classname() const { return "InclusiveSpectra"; }
	InclusiveSpectra() { DEBUG(classname(), "" ); }
	~InclusiveSpectra();

	virtual void initialize();


	//virtual void make()
protected:

	/**
	 * Makes pt histograms for each centrality
	 */
	virtual void makeCentralityHistos();
	virtual int centralityBin( ) {

		int bin9 = pico->b9();
		map<int, int>::iterator mit = centralityBinMap.find( bin9 );
		if ( mit != centralityBinMap.end() )
			return centralityBinMap[ bin9 ];
		return -1;
	}
	int nCentralityBins(){
		
		bool found[ 10 ];
		for ( int i = 0; i < 10; i++ )
			found[ i ] = false;
		for ( map<int, int>::iterator mit = centralityBinMap.begin(); mit != centralityBinMap.end(); mit++){
			found[ mit->second ] = true;

		}
		int n = 0;
		for ( int i = 0; i < 10; i++ ){
			if ( found[ i ] )
				n++;
		}
		return n;
	}

	virtual string histoForCentrality( string centralityCut ){
		return ("pt_" + centralityCut );
	}

	/* @inherit */
	virtual void preEventLoop();

	/* @inherit */
	virtual void postEventLoop();

	/* @inherit */
	virtual void analyzeEvent( );

	/**
	 * Analyze a track in the current Event
	 * @piTrack	- Track index 
	 */
	virtual void analyzeTrack( Int_t iTrack );

	/**
	 * Analyze a track that passes normal and TOF cuts in the current Event
	 * @iTrack 	- Track index 
	 */
	virtual void analyzeTofTrack( Int_t iTrack );

	/* @inherit */
	virtual bool keepEvent();

	/**
	 * Performs track based cuts
	 * @return 	True 	- Passes all selection cuts
	 *          False 	- Fails 1 or more selection cuts
	 */
	virtual bool keepTrack( Int_t iTrack );

	/**
	 * Performs TOF track based cuts
	 * @return 	True 	- Passes all selection cuts
	 *          False 	- Fails 1 or more selection cuts
	 */
	virtual bool keepTofTrack( Int_t iTrack );
	
	bool isRunBad( int runId ){
		if ( find( badRuns.begin(), badRuns.end(), runId ) != badRuns.end() )
			return true;
		return false;
	}


};


#endif