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


	double p( double pt, double eta ){
		return pt * cosh( eta );
	}
	double averageP( int ptBin, int etaBin ){
		if ( ptBin < 0 || ptBin > binsPt->nBins() ){
			return 0;
		}
		if ( etaBin < 0 || etaBin > binsEta->nBins() ){
			return 0;
		} 

		double avgPt = ((*binsPt)[ ptBin ] + (*binsPt)[ ptBin + 1 ]) / 2.0;
		double avgEta = ((*binsEta)[ etaBin ] + (*binsEta)[ etaBin + 1 ]) / 2.0;

		return p( avgPt, avgEta );

	}

	
};



#endif