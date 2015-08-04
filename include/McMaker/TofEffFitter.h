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

class TofEffFitter
{
protected:

	XmlConfig* cfg;
	string nodePath;
	string outputPath;

	unique_ptr<HistoBook> book;

public:
	TofEffFitter( XmlConfig * _cfg, string nodePath );
	~TofEffFitter() {};


	void make();

	void exportParams( int cbin, TF1 * f, ofstream &out );
	
};



#endif