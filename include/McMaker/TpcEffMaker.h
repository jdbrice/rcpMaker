#ifndef TPC_EFF_MAKER_H
#define TPC_EFF_MAKER_H

#include "Spectra/InclusiveSpectra.h"
#include "Spectra/ZRecentering.h"

class TpcEffMaker : public InclusiveSpectra {

public:
	TpcEffMaker( ) {}
	~TpcEffMaker();

	virtual void initialize();

protected:

	string trackType;
	virtual void analyzeTrack( Int_t iTrack );
	
	virtual void analyzeTofTrack( Int_t iTrack );

};

#endif