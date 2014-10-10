#ifndef SPECTRA_MAKER_H
#define SPECTRA_MAKER_H 

/**
 * JDB 
 */

#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
using namespace jdb;

/**
 * ROOT
 */
#include "TChain.h"

/**
 * STD
 */

/**
 * Local
 */
#include "TofPicoDst.h"

class InclusiveSpectra
{
protected:

	Logger * lg;
	XmlConfig * cfg;
	string nodePath;

	TChain * chain;
	TofPicoDst * pico;


public:
	InclusiveSpectra( XmlConfig * config, string nodePath );
	~InclusiveSpectra();



protected:

	/**
	 * Loops the tree events and calculates the non-linear
	 * recentering for use with unbinned methods
	 */
	void eventLoop();
	
};


#endif