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

/**
 * Local
 */
#include "PicoDataStore.h"

class InclusiveSpectra : public TreeAnalyzer
{
protected:

	PicoDataStore * pico;
	
	/**
	 * Event cuts
	 */
	ConfigRange *cutVertexZ;
	ConfigRange *cutVertexR;
	ConfigPoint *cutVertexROffset;
	ConfigRange *cutTofMatch;
	vector<int> cutTriggers;
	vector<string> centrals;
	map< string, ConfigRange* > cutCentrality;

	/**
	 * Track Cuts
	 */
	ConfigRange *cutYLocal;
	ConfigRange *cutNHits;
	ConfigRange *cutNHitsDedx;
	ConfigRange *cutNHitsFitOverPossible;
	ConfigRange *cutDca;

	bool makeEventQA, makeTrackQA;

	int correctedRefMult = -1;
	RefMultCorrection *rmc;


public:
	InclusiveSpectra( XmlConfig * config, string nodePath, string fileList ="", string jobPrefix ="" );
	~InclusiveSpectra();

	//virtual void make()
protected:
	/**
	 * Makes pt histograms for each centrality
	 */
	virtual void makeCentralityHistos();
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
	virtual void postEventLoop(){}

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