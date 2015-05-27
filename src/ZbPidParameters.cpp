#include "Params/ZbPidParameters.h"


void ZbPidParameters::setRefMass( double m ){
	refMass = m;
}

Double_t ZbPidParameters::minP = .1755;
Double_t ZbPidParameters::refMass = 0.493667; // kaon mass in GeV/c

Double_t ZbPidParameters::mean( Double_t p, Double_t m, 
								Double_t p0, Double_t p1, Double_t p2 ){
	using namespace TMath;

	const Double_t rp = minP / p;
	const Double_t d1 = ASin( rp ) * Sqrt( 1 - rp*rp );
	const Double_t a = ( 1 - rp / d1 ) * p1;
	
	const Double_t d2 = p * Sqrt( p*p + m*m );
	const Double_t b = (m*m / d2 ) * p2;

	const Double_t d3 = p * Sqrt( p*p + refMass*refMass );
	const Double_t c = (refMass*refMass / d3 ) * p2;

	return (p0 + a + b + c);
}

Double_t ZbPidParameters::sigma( 	Double_t p, Double_t m,
									Double_t p0, Double_t p1, Double_t p2 ){
	using namespace TMath;

	const Double_t rp = minP / p;
	const Double_t d1 = ASin( rp ) * Sqrt( 1 - rp*rp );
	const Double_t a = Power( ( 1 - rp / d1 ) * p1 , 2 );
	
	const Double_t d2 = p * Sqrt( p*p + m*m );
	const Double_t b = Power( (m*m / d2 ) * p2, 2 );

	const Double_t d3 = p * Sqrt( p*p + refMass*refMass );
	const Double_t c = Power( (refMass*refMass / d3 ) * p2, 2 );

	return Sqrt(Power( p0, 2 ) + a + b + c);
}



ZbPidParameters::ZbPidParameters( XmlConfig * cfg, string nodePath, PhaseSpaceRecentering * psr ){

	log.info(__FUNCTION__) << endl;
	this->cfg = cfg;
	this->nodePath = nodePath;
	this->psr = psr;

	// load in the parameters
	for ( string plc : PidPhaseSpace::species ){

		if ( cfg->exists( nodePath + "." + plc ) ){

			p0M[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".mean.p0" );
			p1M[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".mean.p1" );
			p2M[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".mean.p2" );

			p0S[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".sigma.p0" );
			p1S[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".sigma.p1" );
			p2S[ plc ] = cfg->getDoubleVector( nodePath + "." + plc + ".sigma.p2" );
		}
	}
}



Double_t ZbPidParameters::mean( string plc, Double_t p, int iCen ){

	log.debug( __FUNCTION__ ) << plc << "@ p=" << p << ", iCen=" << iCen <<endl;
	log.debug( __FUNCTION__ ) <<p0M[ plc ][ iCen ] << ", " << p1M[ plc ][ iCen ] <<", " << p2M[ plc ][ iCen ] << endl;

	return ZbPidParameters::mean( p, psr->mass( plc ),
									p0M[ plc ][ iCen ],
									p1M[ plc ][ iCen ],
									p2M[ plc ][ iCen ] );
}

Double_t ZbPidParameters::sigma( string plc, Double_t p, int iCen ){

	log.debug( __FUNCTION__ ) << plc << "@ p=" << p << ", iCen=" << iCen <<endl;
	log.debug( __FUNCTION__ ) <<p0S[ plc ][ iCen ] << ", " << p1S[ plc ][ iCen ] <<", " << p2S[ plc ][ iCen ] << endl;
	return ZbPidParameters::sigma( p, psr->mass( plc ),
									p0S[ plc ][ iCen ],
									p1S[ plc ][ iCen ],
									p2S[ plc ][ iCen ] );
}


