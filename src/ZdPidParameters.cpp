#include "Params/ZdPidParameters.h"



Double_t ZdPidParameters::sigma( 	Double_t p,
									Double_t p0, Double_t p1, Double_t p2, Double_t p3 ){
	using namespace TMath;
	double r = p0 - ( p1 + p2 * p ) * TMath::Exp( -p3 * p );
	return r;
}



ZdPidParameters::ZdPidParameters( XmlConfig * cfg, string nodePath){

	log.info(__FUNCTION__) << endl;
	this->cfg = cfg;
	this->nodePath = nodePath;

	// load in the parameters
	for ( string plc : PidPhaseSpace::species ){

		if ( cfg->exists( nodePath + "." + plc ) ){

			p0[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".sigma.p0" );
			p1[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".sigma.p1" );
			p2[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".sigma.p2" );
			p3[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".sigma.p3" );
		}
	}
}


Double_t ZdPidParameters::sigma( string plc, Double_t p, int iCen ){

	log.debug( __FUNCTION__ ) << plc << "@ p=" << p << ", iCen=" << iCen <<endl;
	log.debug( __FUNCTION__ ) <<p0[ plc ][ iCen ] << ", " << p1[ plc ][ iCen ] << ", " << p2[ plc ][ iCen ] << ", " << p3[ plc ][ iCen ] << endl;

	return ZdPidParameters::sigma( 	p,
									p0[ plc ][ iCen ],
									p1[ plc ][ iCen ],
									p2[ plc ][ iCen ], 
									p3[ plc ][ iCen ] );
}


