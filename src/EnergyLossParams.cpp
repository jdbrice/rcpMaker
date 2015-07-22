#include "Params/EnergyLossParams.h"

int EnergyLossParams::instances = 0;

EnergyLossParams::EnergyLossParams( XmlConfig * cfg, string nodePath, int ll){

	

	EnergyLossParams::instances++;
	this->nodePath = nodePath;
	formula = cfg->getString( nodePath + ":formula" );
	fn = new TF1( ("energyloss_" + ts(instances)).c_str(), formula.c_str() );

	int nPar = fn->GetNpar();
 
	WARN( "Found " << nPar );
	int p = 0;
	while ( cfg->exists( nodePath + ":p" + ts(p) ) ){
		double val = cfg->getDouble( nodePath + ":p" + ts(p) );
		WARN( "p" << p << " = " << val )
		fn->SetParameter( p, val );

		p++;
	}
	

}