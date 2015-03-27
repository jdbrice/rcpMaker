#ifndef SGF_RUNNER_H
#define SGF_RUNNER_H

#include <memory>

#include "Logger.h"
#include "HistoAnalyzer.h"
#include "HistoBins.h"

#include "SGFSchema.h"
#include "TofPidParams.h"
#include "DedxPidParams.h"
#include "PhaseSpaceRecentering.h"

#include <map>


class SGFRunner : public HistoAnalyzer
{
protected:
	
	map< string, unique_ptr<TofPidParams> > tofParams;
	map<string, unique_ptr<DedxPidParams> > dedxParams;

	string centerSpecies;
	string psrMethod;
	double dedxSigmaIdeal, tofSigmaIdeal;
	PhaseSpaceRecentering * psr;

	shared_ptr<SGFSchema> schema;

	HistoBins* binsPt;
	HistoBins* binsEta;
	HistoBins* binsCharge;

public:
	SGFRunner( XmlConfig * _cfg, string _np  );

	~SGFRunner();

	virtual void make();


protected:

	
};



#endif