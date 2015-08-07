#ifndef SPECTRA_CORRECTER_H
#define SPECTRA_CORRECTER_H

// STL
#include <memory>
#include <map>
using namespace std;

// LOCAL
#include "Params/EffParams.h"
#include "Spectra/PidHistoMaker.h"


class SpectraCorrecter
{
protected:
	
	map<string, unique_ptr<ConfigFunction> > tpcEff;
	map<string, unique_ptr<ConfigFunction> > tofEff;
	map<string, unique_ptr<ConfigFunction> > feedDown;

public:

	SpectraCorrecter
	
};


#endif