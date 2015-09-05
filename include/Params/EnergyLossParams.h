#ifndef ENERGY_LOSS_PARAMS_H
#define ENERGY_LOSS_PARAMS_H

// ROOBARB
#include "XmlConfig.h"
#include "Logger.h"
#include "Utils.h"
using namespace jdb;

#include "TF1.h"

class EnergyLossParams{

protected:
	
	string formula;
	TF1 * fn;
	int centralityBin = -1;
	
	static int instances;


public:
	static constexpr auto tag = "EnergyLossParams";
	EnergyLossParams( XmlConfig * cfg, string nodePath, int ll = Logger::llGlobal);
	~EnergyLossParams() {}

	double eval( double pt ){
		if ( fn )
			return fn->Eval( pt );
		return -999;
	}



};

#endif