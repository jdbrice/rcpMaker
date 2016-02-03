#ifndef MC_MAKER_TOFEFF_FITTER_H
#define MC_MAKER_TOFEFF_FITTER_H


// ROOBARB
#include "XmlConfig.h"
#include "HistoBook.h"
using namespace jdb;

// STL
#include <memory>
#include <string>
#include <fstream>

// ROOT
#include "TF1.h"
#include "TGraphAsymmErrors.h"

class TofEffFitter : public IConfig, public IObject
{
protected:

	string outputPath;

	unique_ptr<HistoBook> book;

public:
	virtual const char* classname() const { return "TofEffFitter"; }
	TofEffFitter( XmlConfig _cfg, string nodePath );
	~TofEffFitter() {};


	void make();

	void exportParams( int cbin, TGraphAsymmErrors * g, ofstream &out );
	
};



#endif