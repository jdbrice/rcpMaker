#ifndef TPC_EFF_MAKER_H
#define TPC_EFF_MAKER_H

#include "Spectra/InclusiveSpectra.h"
#include "Spectra/ZRecentering.h"

class TpcEffMaker : public InclusiveSpectra {

public:
	TpcEffMaker( XmlConfig * config, string nodePath, string fileList ="", string jobPrefix ="" );
	~TpcEffMaker();

protected:

	virtual void analyzeTrack( Int_t iTrack );
	
	virtual void analyzeTofTrack( Int_t iTrack );

};

#endif