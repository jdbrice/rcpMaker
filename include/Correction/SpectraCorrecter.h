#ifndef SPECTRA_CORRECTER_H
#define SPECTRA_CORRECTER_H

// STL
#include <memory>
#include <map>
using namespace std;

// LOCAL
#include "Params/EffParams.h"
#include "Spectra/PidHistoMaker.h"
#include "Common.h"

// Roobarb
#include "ConfigFunction.h"
#include "XmlConfig.h"
using namespace jdb;


class SpectraCorrecter
{
protected:
	
	XmlConfig * cfg;
	string nodePath;

	string plc;

	map<string, unique_ptr<ConfigFunction> > tpcEff;
	map<string, unique_ptr<ConfigFunction> > tofEff;
	map<string, unique_ptr<ConfigFunction> > feedDown;

public:

	SpectraCorrecter( XmlConfig * cfg, string _nodePath );
	~SpectraCorrecter();

	void setupCorrections();

	double tpcEffWeight( string plc, double pt, int iCen, int charge );
	double tofEffWeight( string plc, double pt, int iCen, int charge );
	double feedDownWeight( string plc, double pt, int iCen, int charge );
	
};


#endif