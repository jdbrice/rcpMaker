#ifndef TPC_EFF_RC_HISTO_MAKER_H
#define TPC_EFF_RC_HISTO_MAKER_H


// ROOBARB
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
#include "InclusiveSpectra.h"

// TODO : should inherit from Inclusive Spectra since it needs track cuts and QA generation
class TpcEffRcHistoMaker : public InclusiveSpectra {

protected:
	
	unique_ptr<ConfigRange> cut_rapidity;

	string plc;
	double mass;

public:
	TpcEffRcHistoMaker( XmlConfig * config, string nodePath, string fileList ="", string jobPrefix ="" );
	~TpcEffRcHistoMaker();

	/**
	 * Analyze an Event
	 */
	virtual void analyzeEvent( );

	/**
	 * Performs event based cuts
	 * @return 	True 	- Keep Event 
	 *          False 	- Reject Event
	 */
	virtual bool keepEvent();


	bool keepTrack( int iTrack );
	void analyzeTrack( int iTrack );

};



#endif