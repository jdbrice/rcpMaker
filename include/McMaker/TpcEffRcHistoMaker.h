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
#include "Spectra/InclusiveSpectra.h"


class TpcEffRcHistoMaker : public InclusiveSpectra {

protected:
	
	unique_ptr<ConfigRange> cut_rapidity;

	string plc;
	double mass;

public:
	TpcEffRcHistoMaker( XmlConfig * config, string nodePath, string fileList ="", string jobPrefix ="" );
	~TpcEffRcHistoMaker();


	virtual void analyzeTrack( int iTrack );

};



#endif