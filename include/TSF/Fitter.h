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

		unique_ptr<TMinuit> minuit;
		unique_ptr<Logger> logger;

		shared_ptr<FitSchema> schema;
		TFile * dataFile;

		vector<string> parNames;

		static Fitter * self;

		map< string, TH1 * > dataHists;

		bool fitIsGood;

		map<string, bool> players;

		double norm;
		bool sufficienctStatistics;

	public:
		Fitter( shared_ptr<FitSchema> _schema, TFile * dataFile );

		~Fitter();
		void setupFit();

		static vector<double> convergence;

		static void tminuitFCN(int &npar, double *gin, double &f, double *par, int flag);

		static double modelEval( string dataset, double x );
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
		void fit( string cs, int charge, int cenBin, int ptBin );
		void fixParameters();
		void releaseParameters();


		void fixedFit( string cs, int charge, int cenBin, int ptBin );

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

		// getter for current parameters
		double currentMu( string var, string plc, int npar, double * pars );
		double currentYield( string var, int npar = 0, double * pars = 0 );
		double currentYield( string var, string plc, int npar = 0, double * pars = 0 );
		double penalizeYields( int npar = 0, double * pars = 0 );


		double getNorm() { return norm; }
		double normFactor() { return 1000.0; }

		void fixShapes();
		void releaseShapes();


	};
}


#endif	