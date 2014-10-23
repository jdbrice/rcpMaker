#ifndef Analyzer_H
#define Analyzer_H

#include "allroot.h"
#include "HistoBook.h"
#include "constants.h"
#include "XmlConfig.h"
#include "Utils.h"
#include "Reporter.h"

using namespace jdb;

class Analyzer{

protected:

	// the canvas used to draw report hidtos
	Reporter* report;

	// the main chain object
	TChain * _chain;

	// the histobook that stores all of our Analyzer histograms
	HistoBook *book;

	// config file
	XmlConfig* config;

public:


	// Constructor
	Analyzer( TChain * chain, XmlConfig *config );

	// destructor
	~Analyzer();

	void loopEvents();

};



#endif