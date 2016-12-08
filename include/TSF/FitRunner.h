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
#include "TSF/HistorySet.h"
#include "Common.h"

// ROOT
#include "TRandom3.h"


namespace TSF{
	class FitRunner : public HistoAnalyzer
	{
		static constexpr auto tag = "FitRunner";
	protected:

		string centerSpecies;
		string psrMethod;
		double dedxSigmaIdeal, tofSigmaIdeal;
		ZRecentering * psr;

		shared_ptr<FitSchema> schema;

		HistoBins* binsPt;
		
		unique_ptr<Reporter> zbReporter, zdReporter;

		vector<string> activePlayers;

		map< string, HistorySet > tofPidEffSets;
		map< string, XmlRange > tofPidEffRanges;

		map< string, HistorySet > sigmaSets;
		map< string, XmlRange > sigmaRanges;

		unique_ptr<TRandom3> rnd;

		vector<int> centralityFitBins;
		vector<int> chargeFit;

		string imgNameMod;


	public:
		virtual const char * classname() const { return "FitRunner"; }
		FitRunner( );//XmlConfig _config, string _np, int iCharge = -10, int iCen = -1  );
		~FitRunner();

		/* Applies overrides based on config values
		 *
		 */
		virtual void overrideConfig(){

			// these are really just for convenience in the case of passing single values
			map< string, string > opts; 
			int iCharge = 0;
			int iCen = -1;
			if ( config.exists( "iCharge" ) ){
				opts[ nodePath + ".FitRange.charges" ] = config[ "iCharge" ];
				iCharge = config.getInt( "iCharge" );
			}
			if ( config.exists( "iCen" ) ){
				opts[ nodePath + ".FitRange.centralityBins" ] = config[ "iCen" ];
				iCen = config.getInt( "iCen" );
			}

			if ( iCen >= 0 && abs(iCharge) > 0 ){
				centerSpecies = config.getXString( nodePath+".ZRecentering.centerSpecies", "K" );
				imgNameMod = "rp_" + centerSpecies + "_" + Common::chargeString( iCharge ) + "_iCen_" + ts(iCen);	
			}
			
			for ( auto kv : opts ){
				INFO( classname(), kv.first << " : " << kv.second );
			}

			config.applyOverrides( opts );

		}

		// void setup( XmlConfig &_config, string _nodePath, int iCharge = -10, int iCen = -1 );
		virtual void initialize(  );

		virtual void make();


	protected:

		virtual void postMake(){
			// For debugging config issues
			// config.toXmlFile( config.getString("env:cwd") + "FitRunnerConfig.xml" );
		}

		void makeHistograms();
		void fillFitHistograms(int iPt, int iCen, int iCharge, Fitter &fitter );
		void fillEnhancedYieldHistogram( string plc1, int iPt, int iCen, int iCharge, string plc2, Fitter &fitter );
		void fillSystematicHistogram( string type, string plc, int iPt, int iCen, int iCharge, double sys );

		void reportYields();
		void reportFit( Fitter * fitter, int iPt );
		void drawSet( string v, Fitter * fitter, int iPt );
		void drawFitRatio( string ds, Fitter * fitter, int iPt );

		void prepare( double avgP, int iCen );
		shared_ptr<FitSchema> prepareSystematic( string sys, string plc, double delta );
		void choosePlayers( double avgP, string plc );
		void respondToStats( double avgP );

		void runNominal( int iCharge, int iCen, int iPt, shared_ptr<FitSchema> _schema = nullptr );
		void runSigmaSystematic( int iCharge, int iCen, int iPt );
		void runTofEffSystematic( int iCharge, int iCen, int iPt );
		void runSystematic( shared_ptr<FitSchema>, int iCharge, int iCen, int iPt );

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

		string setTitle( string setName ){
			if ( "zb_All" == setName )
				return "All";
			if ( "zb_Pi" == setName )
				return "Pi Enhanced";
			if ( "zb_K" == setName )
				return "K Enhanced";
			if ( "zb_P" == setName )
				return "P Enhanced";

			if ( "zd_All" == setName )
				return "All";
			if ( "zd_Pi" == setName )
				return "Pi Enhanced";
			if ( "zd_K" == setName )
				return "K Enhanced";
			if ( "zd_P" == setName )
				return "P Enhanced";
			return "ERROR";
		}

	};

}




#endif