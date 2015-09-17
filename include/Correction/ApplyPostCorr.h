#ifndef CORRECTION_APPLY_POST_CORR_H
#define CORRECTION_APPLY_POST_CORR_H

#include "Correction/SpectraCorrecter.h"

//Roobarb
#include "XmlConfig.h"
#include "ConfigFunction.h"
#include "ConfigGraph.h"
#include "HistoAnalyzer.h"
using namespace jdb;

// STL
#include <string>
#include <map>
using namespace std;

class ApplyPostCorr : public HistoAnalyzer
{
protected:

	// map<string, unique_ptr<ConfigFunction> > tpcEff;
	map<string, unique_ptr<ConfigGraph> > tofEff;
	map<string, unique_ptr<ConfigFunction> > feedDown;

	unique_ptr<SpectraCorrecter> sc;

	string plc = "UNK";

	bool apply_feeddown;
	bool apply_tofEff;

public:
	static constexpr auto tag = "ApplyPostCorr";
	ApplyPostCorr( XmlConfig * _cfg, string _nodePath );
	~ApplyPostCorr() {}

	void setupCorrections();

	void make();
	
};



#endif