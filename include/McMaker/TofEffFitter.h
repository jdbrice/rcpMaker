#ifndef MC_MAKER_TOFEFF_FITTER_H
#define MC_MAKER_TOFEFF_FITTER_H


// ROOBARB
#include "XmlConfig.h"
#include "HistoBook.h"
#include "HistoAnalyzer.h"

using namespace jdb;

// STL
#include <memory>
#include <string>
#include <fstream>

// ROOT
#include "TF1.h"
#include "TGraphAsymmErrors.h"

class TofEffFitter : public HistoAnalyzer
{
protected:

	string outputPath;
	unique_ptr<HistoBook> book;

public:
	virtual const char* classname() const { return "TofEffFitter"; }
	
	// TODO: inheritance should make this unneeded
	virtual void init( XmlConfig &_config, string _nodePath ){
		TaskRunner::init( _config, _nodePath );
		initialize();
	}
	virtual void initialize();
	virtual void make();

	void exportParams( int cbin, TH1 * h, TGraphAsymmErrors * g, ofstream &out );
	
};



#endif