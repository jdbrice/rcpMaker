#ifndef analyzer_H
#define analyzer_H

#include "allroot.h"
#include "HistoBook.h"
#include "constants.h"
#include "XmlConfig.h"
#include "jdbUtils.h"
#include "reporter.h"


class analyzer{

protected:

	// the canvas used to draw report hidtos
	reporter* report;

	// the main chain object
	TChain * _chain;

	// the histobook that stores all of our analyzer histograms
	HistoBook *book;

	// config file
	XmlConfig* config;

public:


	// Constructor
	analyzer( TChain * chain, XmlConfig *config );

	// destructor
	~analyzer();

	void loopEvents();

};



#endif