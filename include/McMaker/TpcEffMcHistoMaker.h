#ifndef TPC_EFF_MC_HISTO_MAKER_H
#define TPC_EFF_MC_HISTO_MAKER_H


// ROOBARB
#include "TreeAnalyzer.h"
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

class TpcEffMcHistoMaker : public TreeAnalyzer {

protected:
	unique_ptr<PicoDataStore> pico;
	unique_ptr<ConfigRange> cut_rapidity;

	string plc;
	double mass;

public:
	TpcEffMcHistoMaker( XmlConfig * config, string nodePath, string fileList ="", string jobPrefix ="" );
	~TpcEffMcHistoMaker();

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