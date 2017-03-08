#ifndef SPECTRA_CORRECTER_H
#define SPECTRA_CORRECTER_H

// STL
#include <memory>
#include <map>
using namespace std;

// LOCAL
#include "Params/EffParams.h"
#include "Common.h"

// Roobarb
#include "XmlFunction.h"
#include "XmlBinnedData.h"
#include "XmlConfig.h"
#include "IConfig.h"
#include "IObject.h"
using namespace jdb;


class SpectraCorrecter : public IConfig, public IObject
{
protected:

	string plc;

	map<string, unique_ptr<XmlFunction> > 	tpcEff;
	map<string, unique_ptr<XmlBinnedData> > tofEff;
	map<string, unique_ptr<XmlFunction> > 	feedDown;

public:
	virtual const char* classname() const { return "SpectraCorrecter"; }

	SpectraCorrecter( XmlConfig _config, string _nodePath );
	~SpectraCorrecter();

	void setupCorrections();

	double tpcEffCorr( string plc, double pt, int iCen, int charge );
	double tpcEffWeight( string plc, double pt, int iCen, int charge, double sysNSigma = 0 );
	// double tofEffCorr( string plc, double pt, int iCen, int charge );
	// double tofEffWeight( string plc, double pt, int iCen, int charge, double sysNSigma = 0 );
	double feedDownWeight( string plc, double pt, int iCen, int charge, double sysNSigma = 0 );
	
};


#endif