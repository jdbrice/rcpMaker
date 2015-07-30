#include "Params/FeedDownParams.h"

int FeedDownParams::instances = 0;

FeedDownParams::FeedDownParams( XmlConfig * cfg, string nodePath ){

	FeedDownParams::instances++;
	this->nodePath = nodePath;
	formula = cfg->getString( nodePath + ":formula" );
	fn = new TF1( ("feeddown_" + ts(instances)).c_str(), formula.c_str() );

	cl = cfg->getDouble( nodePath + ":cl" );
	ch = cfg->getDouble( nodePath + ":ch" );
 	
	int p = 0;
	while ( cfg->exists( nodePath + ":p" + ts(p) ) ){
		double val = cfg->getDouble( nodePath + ":p" + ts(p) );
		DEBUG( "p" << p << " = " << val )
		fn->SetParameter( p, val );

		p++;
	}


}  