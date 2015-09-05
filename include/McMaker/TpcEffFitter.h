#ifndef MC_MAKER_TPCEFF_FITTER_H
#define MC_MAKER_TPCEFF_FITTER_H


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

class TpcEffFitter
{
protected:

	XmlConfig* cfg;
	string nodePath;
	string outputPath;

	unique_ptr<HistoBook> book;

public:
	static constexpr auto tag = "TpcEffFitter";
	TpcEffFitter( XmlConfig * _cfg, string nodePath );
	~TpcEffFitter() {};


	void make();

	void exportParams( int cbin, TF1 * f, ofstream &out );
	
};



#endif