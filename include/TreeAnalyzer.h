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
	
	/**jdoc{ "name" : "Logger *logger", "desc" : "The logging object for the job"}*/
	Logger 		*logger;
	/**jdoc{"name" : "XmlConfig *cfg", "desc" : ""}*/
	XmlConfig 	*cfg;
	/**jdoc{"name" : "string nodePath", "desc" : ""}*/
	string 		nodePath;

	/**jdoc{"name" : "HistoBook *book", "desc" : ""}*/
	HistoBook 	*book;
	/**jdoc{"name" : "Reporter *reporter", "desc" : ""}*/
	Reporter 	*reporter;
	/**jdoc{"name" : "TChain *chain", "desc" : ""}*/
	TChain 		*chain;

	/**jdoc{"name" : "int nEventsToProcess", "desc" : ""}*/
	int nEventsToProcess;

// public methods
public:
	/*jdoc{
	"name" : 'TreeAnalyzer( XmlConfig * config, string nodePath, string fileList ="", string jobPrefix ="")',
	"params" : [ "XmlConfig* config", "string nodePath", 'string fileList=""', 'string jobPrefix=""' ],
	"paramDesc" : [ "Project's config object", 
		"The node path containing configuration parameters",
		"Optional: File list for parallel jobs",
		"Optional: Job prefix for parallel jobs. Will be prepended to output file names." ],
	"returns" : [  ],
	"desc" : ""
	}*/
	TreeAnalyzer( XmlConfig * config, string np, string fileList ="", string jobPrefix ="");
	~TreeAnalyzer();

	/*jdoc{
	"name" : "virtual void make()",
	"params" : [ ],
	"paramDesc" : [ ],
	"returns" : [  ],
	"desc" : "The maker function for publicly starting the job"
	}*/
	virtual void make();

protected:
	/*jdoc{
	"name" : "virtual bool keepEvent()",
	"params" : [ ],
	"paramDesc" : [ ],
	"returns" : [ "true : Event should be processed", "false : Event should be skipped" ],
	"desc" : "Central function for event cuts"
	}*/
	virtual bool keepEvent();

	/*jdoc{
	"name" : "virtual void preEventLoop",
	"params" : [ ],
	"paramDesc" : [ ],
	"returns" : [  ],
	"desc" : "Called before the event loop for intialization"
	}*/
	virtual void preEventLoop();

	/*jdoc{
	"name" : "virtual void postEventLoop()",
	"params" : [ ],
	"paramDesc" : [ ],
	"returns" : [  ],
	"desc" : "Called after the event loop for cleanup, report generation, etc."
	}*/
	virtual void postEventLoop(){}

	/*jdoc{
	"name" : "virtual void analyzeEvent()",
	"params" : [ ],
	"paramDesc" : [ ],
	"returns" : [  ],
	"desc" : "Analyzes a single event in the chain"
	}*/
	virtual void analyzeEvent(){}
	
};


#endif