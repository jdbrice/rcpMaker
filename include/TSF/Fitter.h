#ifndef TSF_FITTER_H
#define TSF_FITTER_H

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
		// logging interface
		unique_ptr<Logger> logger;

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

		void loadDatasets( string cs, int charge, int cenBin, int ptBin );
		
		void nop(  );
		void fit( string cs, int charge, int cenBin, int ptBin );
		void fit1(  );
		void fit2(  );
		void fit3(  );
		void fit4(  );
		void fitErrors(  );
		

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
		
		double enforceEnhancedYields( int npar = 0, double * pars = 0 );
		double enforceMassOrder( int npar = 0, double * pars = 0 );
		double enforceEff( int npar = 0, double * pars = 0 );


		double getNorm() { return norm; }
		double normFactor() { return 1000.0; }

		void fix( string var );
		void release( string var, bool check = true );

		void fixShapes();
		void releaseShapes();


		void fsNominal();


	};
}


#endif	