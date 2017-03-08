#ifndef MC_MAKER_ENERGY_LOSS_H
#define MC_MAKER_ENERGY_LOSS_H

#include <fstream>
using namespace std;

// RcpMaker
#include "Spectra/InclusiveSpectra.h"

// Root
#include "TF1.h"

class EnergyLoss : public InclusiveSpectra
{
public:
	EnergyLoss() {}
	~EnergyLoss() {}

	virtual void initialize();

	virtual void preEventLoop();
	virtual void postEventLoop();

	virtual void analyzeTrack( Int_t iTrack, bool isTofTrack = false );

	void exportParams( int cbin, TF1 * f, ofstream &out );
	
};



#endif