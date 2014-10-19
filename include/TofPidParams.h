#ifndef TOF_PID_PARAMS_H
#define TOF_PID_PARAMS_H

#include "TMath.h"
#include "XmlConfig.h"
using namespace jdb;
class TofPidParams
{
protected:
	Double_t sigP, muP, sigEP, muEP, sigT, muT;
	static constexpr Double_t minP = 175.5; 
public:
	TofPidParams( Double_t mT, Double_t mP, Double_t mEP,
				Double_t sT, Double_t sP, Double_t sEP ){
		muT = mT;
		muP = mP;
		muEP = mEP;

		sigT = sT;
		sigP = sP;
		sigEP = sEP;
	}

	TofPidParams( XmlConfig * config, string np ){
		muT = config->getDouble( np + "mean:t" );
		muP = config->getDouble( np + "mean:p" );
		muEP = config->getDouble( np + "mean:ep" );

		sigT = config->getDouble( np + "sigma:t" );
		sigP = config->getDouble( np + "sigma:p" );
		sigEP = config->getDouble( np + "sigma:ep" );
	}

	~TofPidParams(){}

	Double_t mean( Double_t p, Double_t m, Double_t mr ){
		using namespace TMath;

		const Double_t pMeV = p * 1000;
		const Double_t mMeV = m * 1000;
		const Double_t mrMeV = mr * 1000;

		const Double_t rp = minP / pMeV;
		const Double_t d1 = ASin( rp ) * Sqrt( 1 - rp*rp );
		const Double_t a = ( 1 - rp / d1 ) * muP;
		
		const Double_t d2 = pMeV * Sqrt( pMeV*pMeV + mMeV*mMeV );
		const Double_t b = (mMeV*mMeV / d2 ) * muEP;

		const Double_t d3 = pMeV * Sqrt( pMeV*pMeV + mrMeV*mrMeV );
		const Double_t c = (mrMeV*mrMeV / d3 ) * muEP;

		return (muT + a + b + c);
	}
	

	Double_t sigma( Double_t p, Double_t m, Double_t mr ){
		using namespace TMath;

		const Double_t pMeV = p * 1000;
		const Double_t mMeV = m * 1000;
		const Double_t mrMeV = mr * 1000;

		const Double_t rp = minP / pMeV;
		const Double_t d1 = ASin( rp ) * Sqrt( 1 - rp*rp );
		const Double_t a = Power( ( 1 - rp / d1 ) * sigP , 2 );
		
		const Double_t d2 = pMeV * Sqrt( pMeV*pMeV + mMeV*mMeV );
		const Double_t b = Power( (mMeV*mMeV / d2 ) * sigEP, 2 );

		const Double_t d3 = pMeV * Sqrt( pMeV*pMeV + mrMeV*mrMeV );
		const Double_t c = Power( (mrMeV*mrMeV / d3 ) * sigEP, 2 );

		return Sqrt(Power( sigT, 2 ) + a + b + c);
	}
	
};

#endif


