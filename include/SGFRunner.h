#ifndef SGF_RUNNER_H
#define SGF_RUNNER_H

#include <memory>

#include "Logger.h"
#include "HistoAnalyzer.h"
#include "HistoBins.h"

#include "SGFSchema.h"
#include "PhaseSpaceRecentering.h"

#include "ZbPidParameters.h"
#include "ZdPidParameters.h"

#include <map>


class SGFRunner : public HistoAnalyzer
{
protected:
	
	shared_ptr<XmlConfig> paramsConfig;

	bool useParams = false;

	unique_ptr< ZbPidParameters > zbParams;
	unique_ptr< ZdPidParameters > zdParams;

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


	static string yieldName( string plc, int iCen, int charge, int iEta );
	static string sigmaName( string plc, int iCen, int charge, int iEta );
	static string muName( string plc, int iCen, int charge, int iEta );


protected:

	void makeHistograms();
	void fillFitHistograms(int iPt, int iCen, int iCharge, int iEta );


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

	double zbSigma( string plc, double p, int iCen ){

		if ( useParams   )
			return zbParams->sigma( plc, p, iCen );
		
		return tofSigmaIdeal;
	}
	double zbMean( string plc, double p, int iCen ){
		if ( useParams )
			return zbParams->mean( plc, p, iCen );

		map< string, double> means = psr->centeredTofMap( centerSpecies, p );

		return means[ plc ];
	}
	double zdMean( string plc, double p ){
		map< string, double> means = psr->centeredDedxMap( centerSpecies, p );
		return means[ plc ];
	}
	double zdSigma( string plc, double p, int iCen ){

		if ( useParams )
			return zdParams->sigma( plc, p, iCen );

		return dedxSigmaIdeal;
	}

	
};



#endif