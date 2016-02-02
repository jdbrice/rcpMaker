#ifndef MC_MAKER_TPCEFF_FITTER_H
#define MC_MAKER_TPCEFF_FITTER_H


// ROOBARB
#include "XmlConfig.h"
#include "HistoBook.h"
// Interface
	#include "IObject.h"
	#include "IConfig.h"
using namespace jdb;

// STL
#include <memory>
#include <string>
#include <fstream>

// ROOT
#include "TF1.h"
#include "TFitResultPtr.h"

class TpcEffFitter : public IConfig, public IObject
{
protected:

	string nodePath;
	string outputPath;

	unique_ptr<HistoBook> book;

public:
	static constexpr auto tag = "TpcEffFitter";
	TpcEffFitter( XmlConfig _cfg, string nodePath );
	~TpcEffFitter() {};


	void make();

	void exportParams( int cbin, TF1 * f, TFitResultPtr result, ofstream &out );
	
};



#endif