#include "TSF/FitSchema.h"


namespace TSF{


	FitSchema::FitSchema( XmlConfig * _cfg, string _nodePath ){

		cfg = _cfg;
		nodePath = _nodePath;

		logger = unique_ptr<Logger>(LoggerConfig::makeLogger( cfg, nodePath + "Logger" ));
		logger->setClassSpace( "FitSchema" );
		logger->info(__FUNCTION__) << endl;

		method = cfg-> getString( nodePath + ":method", "chi2" );

		makeFitVars();
		makeModels();
	}

	FitSchema::~FitSchema(){


	}
   

	void FitSchema::makeFitVars(){

		vector<string> fvPaths = cfg->childrenOf( nodePath, "FitVar" );
		for ( string path : fvPaths ){
			logger->info( __FUNCTION__ ) << cfg->getString( path + ":name" ) << endl;
			vars[ cfg->getString( path + ":name" ) ] = shared_ptr<FitVar>(new FitVar( cfg, path ));
			nPars++;
		}
	}


	void FitSchema::makeModels(){

		logger->info(__FUNCTION__)<< endl;

		vector<string> paths = cfg->childrenOf( nodePath, "Model" );
		for ( string path : paths ){

			logger->info(__FUNCTION__ ) << path << " : " << cfg->getString( path + ":name" )<< endl;
			makeGauss( path );
		}
	}

	void FitSchema::makeGauss( string mpath ){

		string dataset 		= cfg->getString( mpath + ":dataset" );
		string modelName 	= cfg->getString( mpath + ":name" );

		logger->info(__FUNCTION__) << "dataset : " << dataset << endl;

		vector<string> paths = cfg->childrenOf( mpath, "GaussModel" );
		for ( string path : paths ){

			string name = cfg->getString( path + ":name" );
			name = modelName + "_" + name;
			models[ name ] = shared_ptr<GaussModel>( new GaussModel( cfg, path, dataset ) );
			models[ name ]->setVars( vars );
		}

	}

	void FitSchema::loadDataset( string ds, TH1 * h ){

		logger->info(__FUNCTION__) << "Loading Dataset " << ds << " from " << h << endl;
		if ( !h ){
			logger->error( __FUNCTION__ ) << "Cannot load dataset " << ds << endl;
			return;
		}

		datasets[ ds ].clear();

		// // normalize h first
		//h->Sumw2();
		//h->Scale( 100.0 / h->Integral() );

		for ( int i = 1; i <= h->GetNbinsX(); i++ ){

			double center = h->GetBinCenter( i );
			double content = h->GetBinContent( i );
			double error = h->GetBinError( i );
			double bw = h->GetBinWidth( i );

			FitDataPoint fdp( center, content, error, bw );
			datasets[ ds ].push_back( fdp );

		}

	}

	void FitSchema::updateModels( map<string, bool> &act  ){

		for ( auto k : models ){

			k.second->setVars( vars ); 
			//if ( !act[ k.first ] )
			//	k.second->y = 0.0001;
		}
	}

	void FitSchema::reportModels(){

		for ( auto k : models ){
			logger->info(__FUNCTION__) << k.second->toString() << endl;  
		}
	}

	void FitSchema::setInitialMu( string var, double _mu, double _sigma, double _dmu ){
		vars[ var ]->val = _mu;
		
		if ( 0 == _sigma || 0 == _dmu ){ // unconstrained
			vars[ var ]->min = 0;
			vars[ var ]->max = 0;
		} else { // set the range
			vars[ var ]->min = 0; //_mu - _sigma * _dmu;
			vars[ var ]->max = 0; //_mu + _sigma * _dmu;
		}

		logger->info(__FUNCTION__) <<  vars[ var ]->toString() << endl;
	}

	/**
	 * Sets the inital value of the sigma parameter
	 * @var   	the string name of the parameter
	 * @_sigma 	The intial sigma value
	 * @_dsig   The variation in sigma allowed from ( 0 -> 1.0) => percent / 100
	 */ 
	void FitSchema::setInitialSigma( string var, double _sigma, double _dsig ){
		vars[ var ]->val = _sigma;

		// set the range
		vars[ var ]->min = _sigma * ( 1 - _dsig );
		vars[ var ]->max = _sigma * ( 1 + _dsig );
		logger->info(__FUNCTION__) <<  vars[ var ]->toString() << endl;

	}

	/**
	 * Sets the inital value of the sigma parameter
	 * @var   	the string name of the parameter
	 * @_sigma 	The intial sigma value
	 * @_min    The min sigma value
	 * @_max 	The max sigma value
	 */ 
	void FitSchema::setInitialSigma( string var, double _sigma, double _min, double _max ){
		vars[ var ]->val = _sigma;

		// set the range
		vars[ var ]->min = _min;
		vars[ var ]->max = _max;
		logger->info(__FUNCTION__) <<  vars[ var ]->toString() << endl;

	}

	void FitSchema::fixParameter( string var, double val, bool fixed ){
		vars[ var ]->val = val;
		vars[ var ]->fixed = fixed;

	}


	void FitSchema::addRange( string ds, double min, double max ){

		FitRange fr( ds, min, max );
		ranges.push_back( fr );

		fitInRange = true;
	}

	void FitSchema::clearRanges(){
		ranges.clear();
		fitInRange = false;
	}

	bool FitSchema::inRange( string ds, double x ){

		bool foundDS = false;
		for ( FitRange fr : ranges ){

			if ( ds == fr.dataset ){
				foundDS = true;
				if ( fr.inRange( x ) )
					return true;
			}
		}
		if ( foundDS )
			return false;
		else
			return true;
	}

	void FitSchema::reportFitRanges(  ){

		logger->info(__FUNCTION__) << "FitRanges : " << endl;
		for ( FitRange fr : ranges ){
			logger->info( __FUNCTION__ ) << fr.toString() << endl;
		}
	}






} // namespace TSF




