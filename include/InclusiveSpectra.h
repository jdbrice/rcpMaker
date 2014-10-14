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
#include "TofPicoDst.h"

class InclusiveSpectra
{
protected:

	Logger * lg;
	XmlConfig * cfg;
	string nodePath;

	HistoBook * book;


	TChain * chain;
	PicoDataStore * pico;

	/**
	 * Analysis cut Alias
	 */
	ConfigRange *cutVertexZ;


public:
	InclusiveSpectra( XmlConfig * config, string nodePath );
	~InclusiveSpectra();

	void make() {
		eventLoop();
	}

protected:

	/**
	 * Loops the tree events and calculates the non-linear
	 * recentering for use with unbinned methods
	 */
	void eventLoop();

	/**
	 * Before the event loop starts - for subclass init
	 */
	virtual void preLoop(){}

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