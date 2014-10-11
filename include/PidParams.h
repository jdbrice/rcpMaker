#ifndef PID_PARAMS_H
#define PID_PARAMS_H

#include "TMath.h"
#include "XmlConfig.h"
using namespace jdb;
class PidParams
{
protected:
	Double_t sigP, muP, sigEP, muEP, sigT, muT;
	static constexpr Double_t minP = 175.5; 
public:
	PidParams( Double_t mT, Double_t mP, Double_t mEP,
				Double_t sT, Double_t sP, Double_t sEP ){
		muT = mT;
		muP = mP;
		muEP = mEP;

		sigT = sT;
		sigP = sP;
		sigEP = sEP;
	}

	PidParams( XmlConfig * config, string np ){
		muT = config->getDouble( np + "mean:t" );
		muP = config->getDouble( np + "mean:p" );
		muEP = config->getDouble( np + "mean:ep" );

		sigT = config->getDouble( np + "sig:t" );
		sigP = config->getDouble( np + "sig:p" );
		sigEP = config->getDouble( np + "sig:ep" );
	}

	~PidParams(){}

	Double_t mean( Double_t p, Double_t m ){

		using namespace TMath;

		const Double_t rp = minP / p;
		const Double_t d1 = ASin( rp ) * Sqrt( 1 - rp*rp );
		const Double_t a = ( 1 - rp / d1 ) * muP;
		
		const Double_t d2 = p * Sqrt( p*p + m*m );
		const Double_t b = (m*m / d2 ) * muEP;

		return (muT + a + b);

	}
	

	Double_t sigma( Double_t p, Double_t m ){

		using namespace TMath;

		const Double_t rp = minP / p;
		const Double_t d1 = ASin( rp ) * Sqrt( 1 - rp*rp );
		const Double_t a = Power( ( 1 - rp / d1 ) * sigP , 2 );
		
		const Double_t d2 = p * Sqrt( p*p + m*m );
		const Double_t b = Power( (m*m / d2 ) * sigEP, 2 );

		return (Power( sigT, 2 ) + a + b);

	}
	
};

#endif


