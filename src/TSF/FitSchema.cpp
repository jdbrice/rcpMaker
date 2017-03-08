#include "TSF/FitSchema.h"


namespace TSF{


	FitSchema::FitSchema( XmlConfig * _cfg, string _nodePath ){
		INFO( tag, "")
		cfg = _cfg;
		nodePath = _nodePath;

		
		INFO( tag, "" )

		method = cfg-> getString( nodePath + ":method", "chi2" );
		verbosity = cfg->getInt( nodePath + ":v", 0 );

		makeFitVars();
		makeModels();
	}

	FitSchema::~FitSchema(){
		INFO( tag, "")

	}

	FitSchema::FitSchema( const FitSchema &other ){
		// basic copies
		this->nodePath = other.nodePath;
		this->cfg = other.cfg;
		this->nPars = other.nPars;
		this->method = other.method;
		this->fitInRange = other.fitInRange;
		this->normalization = other.normalization;
		this->verbosity = other.verbosity;
		this->ranges = other.ranges;

		// copy the a variables
		for ( auto k : other.vars ){
			this->vars[ k.first ] = shared_ptr<FitVar>( new FitVar( *k.second ) );
		}

		for ( auto ds : other.datasets ){
			this->datasets[ ds.first ] = ds.second;
		}

		for ( auto m : other.models ){
			this->models[ m.first ] = m.second;
		}
	}
   

	void FitSchema::makeFitVars(){
		INFO( tag, "")

		vector<string> fvPaths = cfg->childrenOf( nodePath, "FitVar" );
		for ( string path : fvPaths ){
			INFO( tag, cfg->getString( path + ":name" ) );
			vars[ cfg->getString( path + ":name" ) ] = shared_ptr<FitVar>(new FitVar( cfg, path ));
			nPars++;
		}
	}


	void FitSchema::makeModels(){
		INFO( tag, "")
		vector<string> paths = cfg->childrenOf( nodePath, "Model" );
		for ( string path : paths ){

			INFO( tag, path << " : " << cfg->getString( path + ":name" ) );
			makeGauss( path );
		}
	}

	void FitSchema::makeGauss( string mpath ){
		INFO( tag, "")
		string dataset 		= cfg->getString( mpath + ":dataset" );
		string modelName 	= cfg->getString( mpath + ":name" );

		INFO( tag, "dataset : " << dataset )

		vector<string> paths = cfg->childrenOf( mpath, "GaussModel" );
		for ( string path : paths ){

			string name = cfg->getString( path + ":name" );
			name = modelName + "_" + name;
			models[ name ] = shared_ptr<GaussModel>( new GaussModel( cfg, path, dataset ) );
			models[ name ]->setVars( vars );
		}

	}

	void FitSchema::loadDataset( string ds, TH1 * h ){
		INFO( tag, "( dataset=" << ds << ", TH1*=" << h << ")")

		if ( !h ){
			ERROR( tag, "Cannot load " << ds  )
			return;
		}

		datasets[ ds ].clear();
		datasets[ ds ].setName( ds );

		INFO( tag, "Loading " << h->GetNbinsX() << " nBins" );
		for ( int i = 1; i <= h->GetNbinsX(); i++ ){

			double center = h->GetBinCenter( i );
			double content = h->GetBinContent( i );
			double error = h->GetBinError( i );
			double bw = h->GetBinWidth( i );

			if ( center < -5 || center > 5 ){
				// WARN( tag, "Skipping point at " << center << ", " << content );
				continue;
			}

			FitDataPoint fdp( center, content, error, bw );
			datasets[ ds ].push_back( fdp );

		}

	}

	void FitSchema::updateModels( map<string, bool> &act  ){
		//DEBUG( tag, "")

		for ( auto k : models ){
			k.second->setVars( vars ); 
		}
	}

	void FitSchema::reportModels(){
		INFO( tag, "")
		for ( auto k : models ){
			INFO( tag, k.second->toString() )
		}
	}

	void FitSchema::setInitialMu( string var, double _mu, double _sigma, double _dmu ){
		if ( !exists( var ) ){
			WARN( tag, var << " DNE" )
			return;
		}

		vars[ var ]->val = _mu;
		
		if ( 0 == _sigma || 0 == _dmu ){ // unconstrained
			vars[ var ]->min = 0;
			vars[ var ]->max = 0;
		} else{ // set the range
			vars[ var ]->min = _mu - _sigma * _dmu;
			vars[ var ]->max = _mu + _sigma * _dmu;
		}

		INFO( tag,  vars[ var ]->toString() )
	}

	void FitSchema::setInitialMuLimits( string var, double _mu, double _sigma, double _dmu ){
		if ( !exists( var ) ){
			WARN( tag, var << " DNE" )
			return;
		}

		if ( 0 == _sigma || 0 == _dmu ){ // unconstrained
			vars[ var ]->min = 0;
			vars[ var ]->max = 0;
		} else{ // set the range
			vars[ var ]->min = _mu - _sigma * _dmu;
			vars[ var ]->max = _mu + _sigma * _dmu;
		}

		INFO( tag,  vars[ var ]->toString() )
	}

	void FitSchema::setInitialMuLimits( string var, double _min, double _max ){
		if ( !exists( var ) ){
			WARN( tag, var << " DNE" )
			return;
		}

		vars[ var ]->min = _min;
		vars[ var ]->max = _max;

		INFO( tag,  vars[ var ]->toString() )
	}

	/**
	 * Sets the inital value of the sigma parameter
	 * @var   	the string name of the parameter
	 * @_sigma 	The intial sigma value
	 * @_dsig   The variation in sigma allowed from ( 0 -> 1.0) => percent / 100
	 */ 
	void FitSchema::setInitialSigma( string var, double _sigma, double _dsig ){
		if ( !exists( var ) ){
			WARN( tag, var << " DNE" )
			return;
		}
		vars[ var ]->val = _sigma;

		// set the range
		vars[ var ]->min = _sigma * ( 1 - _dsig );
		vars[ var ]->max = _sigma * ( 1 + _dsig );
		INFO( tag,  vars[ var ]->toString() )

	}

	/**
	 * Sets the inital value of the sigma parameter
	 * @var   	the string name of the parameter
	 * @_sigma 	The intial sigma value
	 * @_min    The min sigma value
	 * @_max 	The max sigma value
	 */ 
	void FitSchema::setInitialSigma( string var, double _sigma, double _min, double _max ){
		if ( !exists( var ) ){
			WARN( tag, var << " DNE" )
			return;
		}
		vars[ var ]->val = _sigma;

		// set the range
		vars[ var ]->min = _min;
		vars[ var ]->max = _max;
		INFO( tag,  vars[ var ]->toString() )

	}

	void FitSchema::fixParameter( string var, double val, bool fixed ){
		if ( !exists( var ) ){
			WARN( tag, var << " DNE" )
			return;
		}
		vars[ var ]->val = val;
		vars[ var ]->fixed = fixed;

	}


	void FitSchema::addRange( string ds, double min, double max, string centerOn, string widthFrom, double roi ){

		FitRange fr( ds, min, max, centerOn, widthFrom, roi );
		ranges.push_back( fr );

		fitInRange = true;
	}


	void FitSchema::updateRanges( double roi ){


		for ( FitRange &r : ranges ){

			if ( exists( r.centerOn ) && exists( r.widthFrom ) ){

				if ( roi < 0 ){
					r.min = var( r.centerOn )->val - var( r.widthFrom )->val * r.roi;
					r.max = var( r.centerOn )->val + var( r.widthFrom )->val * r.roi;
				} else if ( r.dataset == "zd_All" || r.dataset == "zb_All" ){
					DEBUG( tag, "Overriding ROI" )
					r.min = var( r.centerOn )->val - var( r.widthFrom )->val * roi;
					r.max = var( r.centerOn )->val + var( r.widthFrom )->val * roi;
				}

				DEBUG( tag, "Updating Range " << r.centerOn << " +- " << r.widthFrom << "(" << r.min << ", " << r.max << " )"  )
			}
		}
	}


	void FitSchema::clearRanges(){
		INFO( tag, "")
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

		INFO( tag, "")
		for ( FitRange fr : ranges ){
			INFO( tag, fr.toString() );
		}
	}

	void FitSchema::setYieldRange( string var, double low, double hi ){
		vars[ var ]->min = low;
		vars[ var ]->max = hi;
	}






} // namespace TSF




