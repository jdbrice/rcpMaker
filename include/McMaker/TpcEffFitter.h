#ifndef MC_MAKER_TPCEFF_FITTER_H
#define MC_MAKER_TPCEFF_FITTER_H


// ROOBARB
#include "XmlConfig.h"
#include "HistoBook.h"
#include "TaskRunner.h"
	
using namespace jdb;

// STL
#include <memory>
#include <string>
#include <fstream>

// ROOT
#include "TF1.h"
#include "TFitResultPtr.h"

class TpcEffFitter : public TaskRunner
{
protected:

	string outputPath;

	unique_ptr<HistoBook> book;

public:
	virtual const char* classname() const { return "TpcEffFitter"; }

	TpcEffFitter( ){};
	~TpcEffFitter() {};

	// TODO: inheritance should make this unneeded
	virtual void init( XmlConfig _config, string _nodePath, int _jobIndex ){
		initialize();
	}
	virtual void initialize();
	virtual void make();

	void exportParams( int cbin, TF1 * f, TFitResultPtr result, ofstream &out );
	
};



#endif