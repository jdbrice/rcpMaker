#ifndef HISTO_ANALYZER_H
#define HISTO_ANALYZER_H


/**
 * JDB 
 */
#include "Utils.h"
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
#include "HistoBook.h"
#include "ConfigRange.h"
#include "ConfigPoint.h"
#include "Reporter.h"
using namespace jdb;

/**
 * ROOT
 */
#include "TROOT.h"
#include "TFile.h"
#include "TError.h"

class HistoAnalyzer
{
// protected properties
protected:
	Logger 		*lg;
	XmlConfig 	*cfg;
	string 		nodePath;

	HistoBook 	*book;
	Reporter 	*reporter;
	TFile 		*inFile;

// public methods
public:
	HistoAnalyzer( XmlConfig * config, string nodePath );
	~HistoAnalyzer();

	virtual void make() {}
	
};


#endif