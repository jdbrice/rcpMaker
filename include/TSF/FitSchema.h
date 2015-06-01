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


	public:
		FitSchema( XmlConfig * _cfg, string np );
		~FitSchema();

		map<string, shared_ptr<FitVar> > vars;
		map<string, shared_ptr<GaussModel> > models;
		map<string, FitDataset > datasets;

		void setMethod( string m = "chi2" ) { method = m; }
		string getMethod() { return method; }

		void makeFitVars();
		void makeModels();
		void makeGauss( string path );
		void updateModels( map<string, bool> &act);

		void loadDataset( string ds, TH1 * h );

		int numParams() { return nPars; }

		void reportModels();


		void setInitialMu( string var, double _mu, double _sigma, double _dmu );
		void setInitialSigma( string var, double _sigma, double _dsig );
		void setInitialSigma( string var, double _sigma, double _min, double _max );
		void fixParameter( string var, double val, bool fixed = true );

	
		void addRange( string dataset, double _min, double _max );
		void clearRanges(  );
		bool constrainFitRange() { return fitInRange; }
		bool inRange( string ds, double x );
		void reportFitRanges();

		vector<FitRange> & getRanges() {
			return ranges;
		}

	};


}


#endif