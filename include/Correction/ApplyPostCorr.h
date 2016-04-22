#ifndef CORRECTION_APPLY_POST_CORR_H
#define CORRECTION_APPLY_POST_CORR_H

#include "Correction/SpectraCorrecter.h"

//Roobarb
#include "XmlConfig.h"
#include "XmlFunction.h"
#include "XmlGraph.h"
#include "HistoAnalyzer.h"
using namespace jdb;

// STL
#include <string>
#include <map>
using namespace std;

class ApplyPostCorr : public HistoAnalyzer
{
protected:

	// map<string, unique_ptr<XmlFunction> > tpcEff;
	map<string, unique_ptr<XmlGraph> > tofEff;
	map<string, unique_ptr<XmlFunction> > feedDown;

	unique_ptr<SpectraCorrecter> sc;

	string plc = "UNK";

	bool apply_feeddown;
	bool apply_tofEff;

public:
	virtual const char* classname() const {return "ApplyPostCorr"; }
	ApplyPostCorr( );
	~ApplyPostCorr() {}

	virtual void initialize();

	void setupCorrections();

	virtual void make();
	
};



#endif