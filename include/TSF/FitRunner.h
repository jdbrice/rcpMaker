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


#include "TSF/FitSchema.h"
#include "TSF/Fitter.h"
#include "TofPidParams.h"
#include "DedxPidParams.h"
#include "PhaseSpaceRecentering.h"




namespace TSF{
	class FitRunner : public HistoAnalyzer
	{
	protected:
		
		

		bool useParams = false;
		map< string, unique_ptr<TofPidParams> > tofParams;
		map<string, unique_ptr<DedxPidParams> > dedxParams;

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

		double zbSigma( string plc, double p ){
			
			double m = psr->mass( plc );
			double mr = psr->mass( centerSpecies );

			if ( useParams && tofParams[plc]  )
				return tofParams[ plc ]->sigma( p, m, mr );
			
			return tofSigmaIdeal;
		}
		double zbMean( string plc, double p ){

			double m = psr->mass( plc );
			double mr = psr->mass( centerSpecies );

			if ( useParams && tofParams[plc]  )
				return tofParams[ plc ]->mean( p, m, mr );

			map< string, double> means = psr->centeredTofMap( centerSpecies, p );

			return means[ plc ];
		}
		double zdMean( string plc, double p ){
			map< string, double> means = psr->centeredDedxMap( centerSpecies, p );
			return means[ plc ];
		}
		double zdSigma( string plc, double p ){

			if ( useParams && dedxParams[plc]  )
				return dedxParams[ plc ]->sigma( p );

			return dedxSigmaIdeal;
		}

		
	};

}




#endif