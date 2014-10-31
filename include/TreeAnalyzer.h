#ifndef TREE_ANALYZER_H
#define TREE_ANALYZER_H


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
#include "TChain.h"
#include "TError.h"

class TreeAnalyzer
{
// protected properties
protected:
	Logger 		*logger;
	XmlConfig 	*cfg;
	string 		nodePath;

	HistoBook 	*book;
	Reporter 	*reporter;
	TChain 		*chain;

	int nEventsToProcess;
// public methods
public:
	TreeAnalyzer( XmlConfig * config, string np, string fileList ="", string jobPrefix ="");
	~TreeAnalyzer();

	virtual void make();
	virtual void preEventLoop();
	virtual void postEventLoop(){}
	virtual void analyzeEvent(){}
	
};


#endif