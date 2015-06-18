#ifndef INCLUSIVE_SPECTRA_H
#define INCLUSIVE_SPECTRA_H 

/**
 * JDB 
 */
#include "TreeAnalyzer.h"
 #include "RefMultCorrection.h"
 using namespace jdb;

/**
 * ROOT
 */
#include "TChain.h"

/**
 * STD
 */
#include <memory>
#include <vector>
#include <math.h>

/**
 * Local
 */
#include "Adapter/PicoDataStore.h"

class InclusiveSpectra : public TreeAnalyzer
{
protected:

	unique_ptr<PicoDataStore> pico;

	/**
	 * Common configs
	 */
	unique_ptr<XmlConfig> qaCfg;
	unique_ptr<XmlConfig> cutsCfg;
	
	/**
	 * Event cuts
	 */
	unique_ptr<ConfigRange> cutVertexZ;
	unique_ptr<ConfigRange> cutVertexR;
	unique_ptr<ConfigPoint> cutVertexROffset;
	unique_ptr<ConfigRange> cutNTofMatchedTracks;
	Int_t triggerMask;
	
	/**
	 * Track Cuts
	 */
	unique_ptr<ConfigRange> cutNHitsFit;
	unique_ptr<ConfigRange> cutDca;
	unique_ptr<ConfigRange> cutNHitsFitOverPossible;
	unique_ptr<ConfigRange> cutNHitsDedx;
	unique_ptr<ConfigRange> cutPt;
	unique_ptr<ConfigRange> cutPtGlobalOverPrimary;
	unique_ptr<ConfigRange> cutYLocal;
	unique_ptr<ConfigRange> cutZLocal;
	unique_ptr<ConfigRange> cutRapidity;


	map<int, int> centralityBinMap;

	/**
	 * Make QA
	 */
	bool makeEventQA, makeTrackQA;

	/**
	 * RefMult Correction
	 */	
	unique_ptr<RefMultCorrection> rmc;

	/**
	 * Current Event Info
	 */
	float refMult = -1;
	double eventWeight = 1.0;
	int cBin;

	Int_t nTofMatchedTracks;



public:
	InclusiveSpectra( XmlConfig * config, string nodePath, string fileList ="", string jobPrefix ="" );
	~InclusiveSpectra();

	//virtual void make()
protected:
	/**
	 * Makes pt histograms for each centrality
	 */
	virtual void makeCentralityHistos();
	virtual int centralityBin( double refMult ) {
		if ( rmc ){
			int bin9 = rmc->bin9( refMult );
			//if ( bin9 > 8 || bin9 < 0 )
			//	return -1;
			map<int, int>::iterator mit = centralityBinMap.find( bin9 );
			if ( mit != centralityBinMap.end() )
				return centralityBinMap[ bin9 ];
		}
		
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

	/**
	 * Before the event loop starts - for subclass init
	 */
	virtual void preEventLoop();

	/**
	 * After the event loop starts - for subclass reporting
	 */
	virtual void postEventLoop();

	/**
	 * Analyze an Event
	 */
	virtual void analyzeEvent( );

	/**
	 * Analyze a track in the current Event
	 * @param	iTrack 	- Track index 
	 */
	virtual void analyzeTrack( Int_t iTrack );

	/**
	 * Performs event based cuts
	 * @return 	True 	- Keep Event 
	 *          False 	- Reject Event
	 */
	virtual bool keepEvent();

	/**
	 * Performs track based cuts
	 * @return 	True 	- Keep Track 
	 *          False 	- Reject Track
	 */
	virtual bool keepTrack( Int_t iTrack );
	


};


#endif