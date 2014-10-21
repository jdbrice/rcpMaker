#ifndef SPECTRA_MAKER_H
#define SPECTRA_MAKER_H 

/**
 * JDB 
 */

#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "HistoBook.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"
#include "Reporter.h"
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

class InclusiveSpectra
{
protected:

	Logger * lg;
	XmlConfig * cfg;
	string nodePath;

	HistoBook * book;
	Reporter * reporter;

	TChain * chain;
	PicoDataStore * pico;

	/**
	 * Event cuts
	 */
	ConfigRange *cutVertexZ;
	ConfigRange *cutVertexR;
	ConfigPoint *cutVertexROffset;
	ConfigRange *cutNTZero;
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


public:
	InclusiveSpectra( XmlConfig * config, string nodePath );
	~InclusiveSpectra();

	void make() {
		eventLoop();
	}

protected:

	/**
	 * Makes pt histograms for each centrality
	 */
	virtual void makeCentralityHistos();
	virtual string histoForCentrality( string centralityCut ){
		return ("pt_" + centralityCut );
	}
	/**
	 * Loops the tree events and calculates the non-linear
	 * recentering for use with unbinned methods
	 */
	void eventLoop();

	/**
	 * Before the event loop starts - for subclass init
	 */
	virtual void preLoop();

	/**
	 * After the event loop starts - for subclass reporting
	 */
	virtual void postLoop(){}

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
	virtual bool eventCut();

	/**
	 * Performs track based cuts
	 * @return 	True 	- Keep Track 
	 *          False 	- Reject Track
	 */
	virtual bool trackCut( Int_t iTrack );
	


};


#endif