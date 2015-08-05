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
#include "Spectra/ZRecentering.h"




namespace TSF{
	class FitRunner : public HistoAnalyzer
	{
	protected:

		string centerSpecies;
		string psrMethod;
		double dedxSigmaIdeal, tofSigmaIdeal;
		ZRecentering * psr;

		shared_ptr<FitSchema> schema;

		HistoBins* binsPt;
		

		unique_ptr<Reporter> zbReporter, zdReporter;

		vector<string> activePlayers;

		double zdSigFix = 0;

		map<string, vector<double> > zbSigMem;
		map< string, double> lockedZbSig;

	public:
		FitRunner( XmlConfig * _cfg, string _np  );

		~FitRunner();

		virtual void make();


	protected:

		void makeHistograms();
		void fillFitHistograms(int iPt, int iCen, int iCharge );
		void reportFit( Fitter * fitter, int iPt );
		void drawSet( string v, Fitter * fitter, int iPt );

		void prepare( double avgP, int iCen );
		void choosePlayers( double avgP, string plc, double roi );

		/* Average P in a bin range assuming a flat distribution
		 * The distribution is really an exp, but we just need to be consistent
		 * @param  bin 0 indexed bit 
		 * @return     average value in the given bin range
		 */
		double binAverageP( int bin ){
			if ( bin < 0 || bin > binsPt->nBins() ){
				return -1;
			}
			return ((*binsPt)[ bin ] + (*binsPt)[ bin + 1 ]) / 2.0;
		}

		double zbSigma( ){	
			return tofSigmaIdeal;
		}
		double zbMean( string plc, double p ){
			map< string, double> means = psr->centeredTofMap( centerSpecies, p );
			return means[ plc ];
		}
		double zdMean( string plc, double p ){
			map< string, double> means = psr->centeredDedxMap( centerSpecies, p );
			return means[ plc ];
		}
		double zdSigma( ){
			return dedxSigmaIdeal;
		}

	};

}




#endif