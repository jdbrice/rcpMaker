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



	public:
		Fitter( shared_ptr<FitSchema> _schema, TFile * dataFile );

		~Fitter();
		static vector<double> convergence;

		static void tminuitFCN(int &npar, double *gin, double &f, double *par, int flag);

		static double modelEval( string dataset, double x );
		static double modelYield( string dataset );

		static double chi2( double data, double pred, double ey ){
			double q = (data - pred) / ey;
			return q*q;
		};

		static double nll( double data, double model ){
			if ( 0 != model ){
				return -data * TMath::Log( model );
			}
			return 0;
		}

		static void updateParameters( int npar = 0, double * pars = 0){
			
			// update the variables
			for ( int i = 0; i < self->parNames.size(); i++ ){
				
				if ( npar >= self->parNames.size() ){
					double val = pars[ i ];
					self->schema->vars[ self->parNames[ i ] ]->val = val;
				} else {
					double val = 0, err = 0;
					self->minuit->GetParameter( i, val, err );
					self->schema->vars[ self->parNames[ i ] ]->val = val;
					self->schema->vars[ self->parNames[ i ] ]->error = err;
				}
			}
			self->schema->updateModels();		
		}

		void loadDatasets( string cs, int charge, int cenBin, int ptBin, int etaBin );
		void fit( string cs, int charge, int cenBin, int ptBin, int etaBin );

		TGraph * plotResult( string dataset );
		TH1 * getDataHist( string dataset ) { return dataHists[ dataset ]; }
		bool isFitGood(  ) { return fitIsGood; }


	};
}


#endif	