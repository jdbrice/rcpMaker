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

	// the pico dst for simpler chain usage
	TOFrPicoDst * pico;

	// config file
	xmlConfig* config;

	clock_t startTime;

public:


	// Constructor
	rcpMaker( TChain * chain, xmlConfig *config );

	// destructor
	~rcpMaker();
	
	void loopEvents();

	

protected:


	/*
	*	Utility functions that should be moved soon
	*/ 
	void startTimer( ) { startTime = clock(); }
	double elapsed( ) { return ( (clock() - startTime) / (double)CLOCKS_PER_SEC ); }
};



#endif