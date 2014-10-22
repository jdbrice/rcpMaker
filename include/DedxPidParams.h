#ifndef DEDX_PID_PARAMS_H
#define DEDX_PID_PARAMS_H

#include "TMath.h"
#include "XmlConfig.h"
using namespace jdb;
class DedxPidParams
{
protected:
	Double_t p0, p1, p2, p3;
public:
	DedxPidParams( Double_t ip0, Double_t ip1, Double_t ip2, Double_t ip3 ){

		p0 = ip0;
		p1 = ip1;
		p2 = ip2;
		p3 = ip3;
	}

	DedxPidParams( XmlConfig * config, string np ){
		p0 = config->getDouble( np + "sigma:p0" );
		p1 = config->getDouble( np + "sigma:p1" );
		p2 = config->getDouble( np + "sigma:p2" );
		p3 = config->getDouble( np + "sigma:p3" );

		cout << " Params : " << p0 << ", " << p1 << ", " << p2 << ", " << p3 << endl;
	}

	~DedxPidParams(){}
	

	Double_t sigma( Double_t p ){
		using namespace TMath;

		double pMeV = p * 1000;
		double r = p0 - ( p1 + p2 * pMeV ) * TMath::Exp( -p3 * pMeV );
		return r;
	}
	
};

#endif


