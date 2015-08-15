#ifndef TSF_FIT_SCHEMA_H
#define TSF_FIT_SCHEMA_H

// RooBarb
#include "XmlConfig.h"
#include "Logger.h"
#include "LoggerConfig.h"
using namespace jdb;

// Local
#include "TSF/FitVar.h"
#include "TSF/GaussModel.h"
#include "TSF/FitDataset.h"
#include "TSF/FitRange.h"

// STL
#include <vector>
#include <map>
#include <memory>
#include <utility>

//ROOT
#include "TH1.h"

namespace TSF{

	class FitSchema
	{
	protected:
		XmlConfig * cfg;
		string nodePath;

		unique_ptr<Logger> logger;

		int nPars = 0;

		string method;

		bool fitInRange = false;
		vector<FitRange> ranges;

		double normalization;

		int verbosity;

		map<string, shared_ptr<FitVar> > vars;

	public:
		FitSchema( XmlConfig * _cfg, string np );
		~FitSchema();

		
		map<string, shared_ptr<GaussModel> > models;
		map<string, FitDataset > datasets;

		shared_ptr<FitVar> var( string name ) {
			if ( vars.count( name ) ){
				return vars[ name ];
			} 
			WARN( name << " DNE" )
			return shared_ptr<FitVar>( new FitVar( "DNE", 0, 0, 0, 0 ) );
		}

		map<string, shared_ptr<FitVar> > getVars() { return vars; }

		bool exists( string name ) { return  vars.count( name ) >= 1; }

		void setMethod( string m = "chi2" ) { method = m; }
		string getMethod() { return method; }

		void makeFitVars();
		void makeModels();
		void makeGauss( string path );
		void updateModels( map<string, bool> &act);

		void loadDataset( string ds, TH1 * h );

		int numParams() { return nPars; }

		void reportModels();

		void clearDatasets(){ datasets.clear(); }
		bool datasetActive( string dsName ){
			if ( datasets.find( dsName ) != datasets.end() )
				return true;
			return false;
		}


		void setInitialMu( string var, double _mu, double _sigma, double _dmu );
		void setInitialSigma( string var, double _sigma, double _dsig );
		void setInitialSigma( string var, double _sigma, double _min, double _max );
		void fixParameter( string var, double val, bool fixed = true );
		void setYieldRange( string var, double low, double high );

		// Fit Ranges
		void addRange( string dataset, double _min, double _max );
		void clearRanges(  );
		bool constrainFitRange() { return fitInRange; }
		bool inRange( string ds, double x );
		void reportFitRanges();

		void setNormalization( double n) {
			normalization = n;
		}
		double getNormalization(){
			return normalization;
		}

		vector<FitRange> & getRanges() {
			return ranges;
		}

		double enforceMassOrdering(){ return 1.0;};

		int getVerbosity() const { return verbosity; }

		bool tofEff() const { return vars.count( "eff_Pi") >= 1;}

	};


}


#endif