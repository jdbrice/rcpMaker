/**
 *	Original Author: Evan Sangaline
 *	Author: Daniel Brandenburg 
 *  Date:	September 3rd, 204
 *  Description:
 *  A class for simulating tof 1/beta values for use with PID studies
 *  
 */

#ifndef TOF_GENERATOR_H
#define TOF_GENERATOR_H


#include "TROOT.h"
#include "TRandom3.h"
#include "TMath.h"


class TofGenerator
{

protected:

	TRandom3 * rGen;
	double obSigma;
	int nSamples;

public:
	/**
	 * Constructor
	 * @param obSigma  The sigma value to be used when generating random 1/beta values
	 * @param nSamples The number of samples to use when generating a random 1/beta value
	 *
	 * The constructor sets up its own TRandom3 random number generator.
	 * This makes each instance have a new unique Seed and generator (with TRandom3 the periodicity is 10**600 )
	 */
	TofGenerator( double obSigma = 0.012, int nSamples = 10 ){
		this->obSigma = obSigma;
		this->nSamples = nSamples;

		// setup the random number generator
		// Use the unique seed provided by ROOT
		rGen = new TRandom3( 0 );

	}
	~TofGenerator(){
		delete rGen;
	}

	/**
	 * Returns the expected mean 1/beta value
	 * @param  p    Momentum value [GeV/c]
	 * @param  mass Mass value [GeV/c^2]
	 * @return      Expected 1/beta for the given p, m
	 */
	double mean( double p, double mass ){
		return TMath::Sqrt( 1.0 + TMath::Power( mass / p, 2 ) );
	}

	/**
	 * Generates a single random 1/beta value
	 * @param  p    Momentum [GeV/c]
	 * @param  mass Mass [GeV/c^2]
	 * @return      Randomly generated 1/beta value for the given p, m
	 */
	double random(double p, double mass ){
		const double mu = mean( p, mass );
    	const double sigma = obSigma;
    	const int nu = nSamples;

    	double s = 0, s2 = 0;

    	// Generate nSamples # of samples
    	for(int i = 0; i < nu; i++) {
        	double x = rGen->Gaus();
        	s += x;
        	s2 += x*x;
    	}
	    
	    // Average
	    s /= double(nu);
	    s2 /= double(nu);
	    
	    double t = s * sqrt( double( nu ) );
	    t /= (s2 - s * s ) * double( nu ) / double( nu - 1 );

	    t *= sigma;
	    t += mu;
	    return t;
	}

};



#endif