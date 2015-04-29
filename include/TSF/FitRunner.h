#ifndef TSF_FIT_RUNNER_H
#define TSF_FIT_RUNNER_H

// STL
#include <memory>
#include <map>
using namespace std;

// ROOBARB
#include "Logger.h"
#include "HistoAnalyzer.h"
#include "HistoBins.h"
using namespace jdb;

// Local
#include "TSF/FitSchema.h"
#include "TSF/Fitter.h"
#include "TofPidParams.h"
#include "DedxPidParams.h"
#include "PhaseSpaceRecentering.h"
#include "ZbPidParameters.h"
#include "ZdPidParameters.h"




namespace TSF{
	class FitRunner : public HistoAnalyzer
	{
	protected:
		
		shared_ptr<XmlConfig> paramsConfig;

		bool useParams = false;
		
		map<string, unique_ptr<DedxPidParams> > dedxParams;

		unique_ptr< ZbPidParameters > zbParams;
		unique_ptr< ZdPidParameters > zdParams;

		string centerSpecies;
		string psrMethod;
		double dedxSigmaIdeal, tofSigmaIdeal;
		PhaseSpaceRecentering * psr;

		shared_ptr<FitSchema> schema;

		HistoBins* binsPt;
		HistoBins* binsEta;
		HistoBins* binsCharge;

		unique_ptr<Reporter> zbReporter, zdReporter;

	public:
		FitRunner( XmlConfig * _cfg, string _np  );

		~FitRunner();

		virtual void make();


		static string yieldName( string plc, int iCen, int charge, int iEta );
		static string sigmaName( string plc, int iCen, int charge, int iEta );
		static string muName( string plc, int iCen, int charge, int iEta );
		static string fitName( int iPt, int iCen, int charge, int iEta );


	protected:

		void makeHistograms();
		void fillFitHistograms(int iPt, int iCen, int iCharge, int iEta );
		void reportFit( Fitter * fitter, int iPt );
		void drawSet( string v, Fitter * fitter, int iPt );


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

			if ( useParams && dedxParams[plc]  )
				return zdParams->sigma( plc, p, iCen );

			return dedxSigmaIdeal;
		}

		
	};

}




#endif