#ifndef MC_MAKER_ENERGY_LOSS_H
#define MC_MAKER_ENERGY_LOSS_H

#include "Spectra/InclusiveSpectra.h"


class EnergyLoss : public InclusiveSpectra
{
public:
	EnergyLoss( XmlConfig * config, string nodePath, string fileList ="", string jobPrefix ="" );
	~EnergyLoss() {}

	virtual void preEventLoop();

	void analyzeTrack( int iTrack );
	
};



#endif