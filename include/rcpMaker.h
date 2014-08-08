#ifndef rcpMaker_H
#define rcpMaker_H

#include "allroot.h"

#include "histoBook.h"
#include "constants.h"
#include "TOFrPicoDst.h"
#include <vector>

// clock_t, clock, CLOCKS_PER_SEC 
#include <time.h>       

// for testing if stdout is interactive or pipe / file
#include "xmlConfig.h"
#include "utils.h"
#include "reporter.h"


class rcpMaker{

private:

	// the canvas used to draw report hidtos
	reporter* report;

	
	// the main chain object
	TChain * _chain;

	// the histobook that stores all of our rcpMaker histograms
	histoBook *book;
	histoBook *pidLUT;

	// the pico dst for simpler chain usage
	TOFrPicoDst * pico;

	// config file
	xmlConfig* config;

	clock_t startTime;


	/**
	 * Cut Varaibles
	 */
	double vzMax, vzMin, vrMax;
	double vOffsetX, vOffsetY, vOffsetZ;

public:


	// Constructor
	rcpMaker( TChain * chain, xmlConfig *config );

	// destructor
	~rcpMaker();
	
	void loopEvents();


	bool keepEvent();
	bool keepTrack( int iHit );
	

	double nSigDedx( string pType, int iHit ) { 
		if ( "P" == pType )
			return pico->nSigP[ iHit ];
		if ( "K" == pType )
			return pico->nSigK[ iHit ];
		if ( "Pi" == pType )
			return pico->nSigPi[ iHit ];
		return -999.0;
	}
	double nSigInvBeta( string pType, int iHit  );
	double dBeta( string pType, int iHit  );

	double eMass( string pType ){
		if ( "P" == pType )
			return constants::protonMass;
		if ( "K" == pType )
			return constants::kaonMass;
		if ( "Pi" == pType )
			return constants::piMass;
		return -10.0;	
	}
	double eBeta( double m, double p ){
		return TMath::Sqrt( p*p / ( p*p + m*m ) );
	}

protected:


	/*
	*	Utility functions that should be moved soon
	*/ 
	void startTimer( ) { startTime = clock(); }
	double elapsed( ) { return ( (clock() - startTime) / (double)CLOCKS_PER_SEC ); }
};



#endif