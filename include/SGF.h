#ifndef SGF_H
#define SGF_H


// LOCAL
#include "SGFSchema.h"
#include "PhaseSpaceRecentering.h"
#include "PidPhaseSpace.h"

// Roobarb
#include "Reporter.h"
#include "Utils.h"
using namespace jdb;

// STL
#include <memory>
#include <map>

// ROOT
#include "TFile.h"



class SGF
{
protected:
	shared_ptr<SGFSchema> schema;
	TFile * inFile;
	string title;

public:
	SGF( shared_ptr<SGFSchema> _schema, TFile * _in );
	~SGF(){}

	void fit( string cs, int charge, int cenBin, int ptBin, int etaBin, bool fitRange = false );

	void showSample( string var, string sample );
	void report( Reporter* rp );


protected:

};





#endif