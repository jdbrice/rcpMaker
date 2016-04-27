#ifndef TSF_FITTER_H
#define TSF_FITTER_H

#include "XmlConfig.h"
#include "Logger.h"
using namespace jdb;

#include "TSF/FitSchema.h"

#include <memory>

//ROOT
#include "TMinuit.h"
#include "TGraph.h"
#include "TFile.h"

namespace TSF{

	class Fitter{

		static constexpr auto tag = "Fitter";

		// Scaling for configurations that break some constraint
		// for instance - enforce 1/beta mass ordering with parabolic minimum
		static constexpr double penaltyScale = 1000.0;

		// the minuit interface
		unique_ptr<TMinuit> minuit;
		
		// fit schema - holds variables etc.
		shared_ptr<FitSchema> schema;
		// data file with the 8 distributions
		TFile * dataFile;

		// ordered list of parameters
		// these are the parameter names in minuit parameter order
		vector<string> parNames;

		// singelton since minuit requires a global scoped fit function
		static Fitter * self;

		// map of data from input file for loading into schema
		map< string, TH1 * > dataHists;

		// did the fit converge
		bool fitIsGood = true;

		// list of models that will contribute to the fit
		map<string, bool> players;

		double norm;
		bool sufficienctStatistics;
		bool fixedEff = false;

		// defaults used in the data projection steps
		// should be loaded from config but not yet
		double zbBinWidth, zdBinWidth, nSigAboveP, zbSigmaIdeal, zdSigmaIdeal, cut_nSigma_Pi, cut_nSigma_K, cut_nSigma_E;

	public:
		Fitter( shared_ptr<FitSchema> _schema, TFile * dataFile );

		~Fitter();
		void setupFit();

		static vector<double> convergence;

		static void tminuitFCN(int &npar, double *gin, double &f, double *par, int flag);

		double modelEval( string dataset, double x );
		static double modelYield( string dataset );

		static double chi2( double data, double pred, double ey ){
			double q = (data - pred) / ey;
			return q*q;
		};

		static double poisson( double data, double pred){
			double q = (data - pred) * (data - pred) / data;
			return q;
		};

		static double fractional( double data, double pred){
			double q = (data - pred) * (data - pred) / (data * data);
			return q;
		};

		static double nll( double data, double model ){
			if ( 0 != model ){
				return -data * TMath::Log( model );
			}
			return 0;
		}

		static void updateParameters( int npar = 0, double * pars = 0);

		vector<double> sumLog;
		double getSumLog( int n ){
			if ( n < 0 ) return 0;
			if ( n >= sumLog.size() ){
				sumLog.clear();
				int nLog = 2 * n + 1000;
				double fobs = 0;

				for ( int j = 1; j < nLog; j++ ){
					if ( j > 1 ) fobs += log( j );
					sumLog.push_back( fobs );
				}
			}
			return sumLog[ n ];
		}

		void registerDefaults( XmlConfig * cfg, string nodePath );
		void loadDatasets( string cs, int charge, int cenBin, int ptBin, bool enhanced, map<string, double> zbMu, map<string, double> zdMu, bool use_zb = true);
		
		void nop(  );
		void fit1(  );
		void fit2(  );
		void fit3( );
		void fit4( );
		void fit5( string plc );
		void fitErrors(  );

		void overrideStatistics( bool v ) { sufficienctStatistics = v; }
		

		TGraph * plotResult( string dataset );
		TH1 * getDataHist( string dataset ) { return dataHists[ dataset ]; }
		bool isFitGood(  ) { return fitIsGood; }

		void clearPlayers() { players.clear(); }
		void addPlayer( string mn ){
			players[ mn ] = true;
		}
		void addPlayers( vector<string> &pls ){
			clearPlayers();
			for ( auto k : pls ){
				players[ k ] = true;
			}
		}

		shared_ptr<FitSchema> getSchema() { return schema; }

		// getter for current parameters
		double currentValue( string var, int npar, double * pars );
		void setValue( string var, double value );

		double enforceEnhancedYields( int npar = 0, double * pars = 0 );
		double enforceMassOrder( int npar = 0, double * pars = 0 );


		double getNorm() { return norm; }
		double normFactor() { return 1000.0; }

		void fix( string var );
		void release( string var, bool check = true );
		void releaseAll( bool check = true );

		void fixShapes();
		void releaseShapes();


		void fsNominal();

		void reportFitStatus(){
			double m_fmin = 0, m_fedm = 0, m_errdef = 0;
			int m_npari = 0, m_nparx = 0, m_istat = 0;

			minuit->mnstat( m_fmin, m_fedm, m_errdef, m_npari, m_nparx, m_istat );
			INFO( tag, "FMIN = " << m_fmin );
		}


	};
}


#endif	