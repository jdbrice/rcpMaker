#include "TSF/Fitter.h"
#include "PidPhaseSpace.h"


namespace TSF{
	
	vector<double> Fitter::convergence;

	Fitter * Fitter::self = nullptr;
	Fitter::Fitter( shared_ptr<FitSchema> _schema, TFile * _dataFile ){

		self = this;
		schema = _schema;
		dataFile = _dataFile;

		logger = unique_ptr<Logger>( new Logger() );
		logger->setClassSpace( "Fitter" );
		logger->info(__FUNCTION__) << endl;
	}

	void Fitter::setupFit(){
		minuit = unique_ptr<TMinuit>( new TMinuit( schema->numParams() ) );
		
		for ( auto k : schema->vars ){
			if ( k.second->exclude ) continue;
			parNames.push_back( k.first );
		}

		for ( int i = 0; i < parNames.size(); i++ ){
			if (schema->vars[ parNames[ i ] ]->exclude) continue;

			if ( 0 <= schema->vars[ parNames[ i ] ]->error )
				schema->vars[ parNames[ i ] ]->error = 0.001;

			minuit->DefineParameter( i, 						// parameter index
						parNames[ i ].c_str(), 					// name
						schema->vars[ parNames[ i ] ]->val, 	// initial value
						schema->vars[ parNames[ i ] ]->error,	// intial step size
						schema->vars[ parNames[ i ] ]->min,		// limit min
						schema->vars[ parNames[ i ] ]->max );	// limit max

			if ( schema->vars[ parNames[ i ] ]->fixed )
				minuit->FixParameter( i );

		}

		minuit->SetPrintLevel( 0 );
		minuit->SetFCN( tminuitFCN );

		logger->info(__FUNCTION__) << endl;
	}

	void Fitter::fixParameters(){
		for ( int i = 0; i < parNames.size(); i++ ){
			if (schema->vars[ parNames[ i ] ]->exclude) continue;

			if ( schema->vars[ parNames[ i ] ]->fixed )
				minuit->FixParameter( i );
		}
	}

	void Fitter::releaseParameters(){
		for ( int i = 0; i < parNames.size(); i++ ){
			if (schema->vars[ parNames[ i ] ]->exclude) continue;

			if ( schema->vars[ parNames[ i ] ]->fixed )
				minuit->Release( i );
		}
	}

	Fitter::~Fitter(){

	}

	void Fitter::tminuitFCN(int &npar, double *gin, double &f, double *par, int flag){

		double fnVal = 0.0;
		updateParameters( npar, par );

		bool useRange = self->schema->constrainFitRange();
		string method = self->schema->getMethod();

		double normFactor = ( self->normFactor() / self->getNorm() );

		// loop on datasets
		for ( auto k : self->schema->datasets ){

			string ds = k.first;

			// loop on datapoints
			for ( auto d : k.second ){
				
				if ( useRange && !self->schema->inRange( ds, d.x )){
					//double modelVal = modelEval( ds, d.x );
					//fnVal += modelVal;
					continue;
				}


				if ( "chi2" == method ){
					// Minimize by chi^2
					if ( 0 == d.ey  )
						continue;
					double modelVal = modelEval( ds, d.x );
					fnVal += chi2( d.y, modelVal, d.ey );	
				} else if ( "nll" == method ){
					// Minimize by negative log likelihood
					double modelVal = modelEval( ds, d.x );
					
					fnVal += nll( d.y, modelVal );
				} else if ( "poisson" == method ){
					// Minimize by poisson errors ie error = sqrt( N )
					if ( 0 >= d.y || 0 >= d.ey )
						continue;
					double modelVal = modelEval( ds, d.x );
					fnVal += poisson( d.y, modelVal );	
				} else if ( "fractional" == method ){
					// Minimize by fractional errors ie error =  N 
					if ( 0 >= d.y || 0 >= d.ey )
						continue;
					double modelVal = modelEval( ds, d.x );
					fnVal += fractional( d.y, modelVal );	
				} else {
					cout << "No Fit method" << endl;
				}
			} // loop on data points

			/*if ( "zd_All" == ds || "zb_All" == ds ){
				double mYield = modelYield( ds );
				double dsYield = k.second.yield(self->schema->getRanges());
				//cout << "ds : " << ds << endl;
				//cout << "dsYield : " << dsYield << ", " << mYield << " dif = " << abs( dsYield - mYield ) << endl;
				// subtract off this dataset's (N - E) term
				fnVal = fnVal - ( dsYield - mYield ) * ( 100.0 / self->norm );
			}*/

		}

		self->penalizeYields( npar, par );

		//convergence.push_back( fnVal );
		//cout << " nll = " << fnVal << endl;
		f = fnVal / normFactor ;		
	}

	void Fitter::loadDatasets( string cs, int charge, int cenBin, int ptBin, int etaBin ){
		logger->info(__FUNCTION__) << endl;

		map< string, TH1D* > zb;
		map< string, TH1D* > zd;

		schema->clearDatasets();

		DEBUG( "Loading " << "tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin ) );
		DEBUG( "Loading " << "dedx/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin ) );
		dataHists[ "zb_All"] = (TH1*)dataFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin )).c_str() 		);
		dataHists[ "zd_All"] = (TH1*)dataFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin )).c_str() 	);
		DEBUG( "got zb_All = " << dataHists[ "zb_All"] )
		DEBUG( "got zd_All = " << dataHists[ "zd_All"] )


		// dEdx enhanced distributions
		DEBUG( "Loading " << "dedx/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "Pi" ) );
		DEBUG( "Loading " << "dedx/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "K" ) );
		DEBUG( "Loading " << "dedx/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "P" ) );
		dataHists[ "zd_K" ]		= (TH1D*)dataFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin, "K" )).c_str() );
		dataHists[ "zd_Pi" ]	= (TH1D*)dataFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin, "Pi" )).c_str() );
		dataHists[ "zd_P" ]		= (TH1D*)dataFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin, "P" )).c_str() );

		// 1/beta enhanced distributions
		DEBUG( "Loading " << "tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "Pi" ) );
		DEBUG( "Loading " << "tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "K" ) );
		DEBUG( "Loading " << "tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "P" ) );
		dataHists[ "zb_Pi" ] 	= (TH1D*)dataFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "Pi" )).c_str() );
		dataHists[ "zb_K" ] 	= (TH1D*)dataFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "K" )).c_str() );
		dataHists[ "zb_P" ] 	= (TH1D*)dataFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "P" )).c_str() );
		
		double maxYield = dataHists[ "zd_All"]->Integral();
		DEBUG( "Getting norm from : EventQA/mappedRefMultBins" )
		norm = ((TH1D*)dataFile->Get( "EventQA/mappedRefMultBins" ))->GetBinContent( cenBin + 1 );

		schema->setNormalization( maxYield / norm );

		sufficienctStatistics = true;

		for ( auto k : dataHists ){

			double nObs = k.second->GetEntries();
			logger->info(__FUNCTION__) << "Num Entries in " << k.first << " : " << nObs << endl;

			// normalize
			k.second->Sumw2();
			k.second->Scale( 1.0 / norm );

			if ( nObs > 75 )
				schema->loadDataset( k.first, k.second );
		}

	}

	void Fitter::fixedFit( string cs, int charge, int cenBin, int ptBin, int etaBin ){

		for ( int i = 0; i < parNames.size(); i++ ){

			bool shouldFix = false;
			if ( string::npos != parNames[ i ].find( "sig" ) || string::npos != parNames[ i ].find( "mu" ) )
				shouldFix = true;

			if ( schema->vars[ parNames[ i ] ]->fixed || shouldFix )
				minuit->FixParameter( i );
		}

		fit( cs, charge, cenBin, ptBin, etaBin );

		for ( int i = 0; i < parNames.size(); i++ ){

			bool shouldFix = false;
			if ( string::npos != parNames[ i ].find( "sig" ) || string::npos != parNames[ i ].find( "mu" ) )
				shouldFix = true;

			if ( !schema->vars[ parNames[ i ] ]->fixed )
				minuit->Release( i );
		}

	}
	
	void Fitter::fit( string cs, int charge, int cenBin, int ptBin, int etaBin ){
		logger->info(__FUNCTION__) << endl;


		// are we using fit ranges?
		// if so lets report
		if ( schema->constrainFitRange() ){
      		schema->reportFitRanges();
      	}

      	// just let us know what you are doing
		logger->info(__FUNCTION__) << "OK Starting for ( charge=" << charge << ", cen=" << cenBin << ", ptBin=" << ptBin << ", etaBin=" << etaBin << " ) " << endl;

		// load the current datasets
		//loadDatasets( cs, charge, cenBin, ptBin, etaBin );

		if ( !sufficienctStatistics ){
			fitIsGood = false;
			updateParameters();
			logger->warn( __FUNCTION__ ) << "Insufficient Statistics : Skipping " << endl;
			return;
		}


		//
		//return;

		double arglist[10];
      	arglist[0] = -1;
      	int iFlag = 0;
      	fitIsGood = false; 

      	
      	schema->setMethod( "fractional" );
      	logger->info(__FUNCTION__) << "Fit Method : " << self->schema->getMethod() << endl;
      	
      	if ( "nll" != self->schema->getMethod() )
      		arglist[ 0 ] = 1.0; 
      	else 
      		arglist[ 0 ] = 0.5; 
      	minuit->mnexcm( "SET ERR", arglist, 1, iFlag );

      	int tries = 0;

      	
      	iFlag = 4;
      	while ( iFlag > 0 && tries < 3 ){
      		logger->info(__FUNCTION__) << "Running MINI" << endl;
      		arglist[ 0 ] = -1;
			arglist[ 1 ] = 1.0;
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			tries++;
      	}


      	schema->setMethod( "poisson" );
      	logger->info(__FUNCTION__) << "Fit Method : " << self->schema->getMethod() << endl;
      	//fixShapes();
      	minuit->mnexcm( "MINI", arglist, 1, iFlag );
      	minuit->mnexcm( "MINI", arglist, 1, iFlag );
      	//releaseShapes();

      	if ( 0 == iFlag )
			fitIsGood = true;
		else 
			fitIsGood = false;

		//minuit->mnexcm( "STATUS", arglist, 1, iFlag ); // get errors

      	// if ( iFlag > 0 )
      	//minuit->mnexcm( "MINOS", arglist, 1, iFlag );
      	// else
      	minuit->mnexcm( "HESSE", arglist, 1, iFlag ); // get errors

      	/*for ( int i = 0; i < parNames.size(); i++ ){
			//schema->vars[ parNames[ i ] ]->;
			minuit->Release( i );
		}*/

		cout << "iFlag " << iFlag << endl;
		

		//logger->info(__FUNCTION__) << "Calculating errors with HESE" << endl;
		//minuit->mnexcm( "HESSE", arglist, 1, iFlag );

		//minuit->mnexcm( "MINOS", arglist, 1, iFlag );

		

		// get the final state of all variables 
		logger->info(__FUNCTION__) << "Updating parameters after Fit" << endl;
		updateParameters();
		//schema->reportModels();




	}



	// evaluate all the models for a given dataset
	double Fitter::modelEval( string dataset, double x ){

		double bw = self->schema->datasets[ dataset ].pointNear( x ).bw;
		double val = 0.0;
		for ( auto k : self->schema->models ){
			//cout << "name : " << k.first << endl;
			if ( k.second->dataset == dataset && self->players[ k.first ] ){ // second part is looking for active
				val += k.second->eval( x, bw );
			} else if ( !self->players[ k.first ] ){
			}
		}
		return val;
	}
	double Fitter::modelYield( string dataset ){

		double val = 0.0;
		for ( auto k : self->schema->models ){
						
			if ( k.second->dataset == dataset ){
				val += k.second->y;
			}
		}
		return val;
	}


	TGraph * Fitter::plotResult( string datasetOrModel ){
		logger->debug(__FUNCTION__) << "Plotting : " << datasetOrModel << endl;

		string datasetName = "";
		string modelName = "";

		shared_ptr<GaussModel> gm;
		bool found = true;

		auto dsIt = schema->datasets.find( datasetOrModel );
		auto mIt = schema->models.find( datasetOrModel );

		if ( dsIt != schema->datasets.end() ){ // show all models for this dataset
			datasetName = dsIt->first;
		} else if ( mIt != schema->models.end() ){ // show just this model
			gm = mIt->second;
			modelName = mIt->first;
			datasetName = mIt->second->dataset;
		} else {
			found = false;
		}

		if ( !found ){
			Logger::log.error( __FUNCTION__ ) << "Invalid Request : " << datasetOrModel << endl;
			return new TGraph();
		}

		if ( "" != modelName && !self->players[ modelName ] ){
			logger->info(__FUNCTION__) << "Skipping inactive Player" << endl;
			return new TGraph();
		}

		pair<double, double> xBounds = schema->datasets[ datasetName ].rangeX(  );

		logger->debug(__FUNCTION__) << "Plotting from ( " << xBounds.first << ", " << xBounds.second <<" ) "<< endl;	
		double stepsize = (xBounds.second - xBounds.first) / 500.0;
		vector<double> vx, vy;

		if ( found ){

			int i = 0;
			for ( double x = xBounds.first; x < xBounds.second; x+= stepsize ){

				FitDataPoint fdp = schema->datasets[ datasetName ].pointNear( x );
				

				vx.push_back(x);
				if ( !gm )
					vy.push_back( modelEval( datasetName, x ) );
				else 
					vy.push_back( gm->eval( x, fdp.bw ) );

				i++;
			}	


		}		

		TGraph * graph = new TGraph( vx.size(), vx.data(), vy.data() );
		return graph;
	}


	void Fitter::updateParameters( int npar, double * pars){
			
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
		self->schema->updateModels( self->players );		
	}

	double Fitter::currentYield( string plc, int npar, double * pars ){
		// update the variables
		for ( int i = 0; i < self->parNames.size(); i++ ){
			
			if ( self->parNames[ i ].find( "yield_" + plc ) != string::npos ){
				return pars[ i ];	
			}
		}
		return 0.0;
	}

	double Fitter::currentYield( string enh, string plc2, int npar , double * pars  ){
		// update the variables
		for ( int i = 0; i < self->parNames.size(); i++ ){
			
			if ( self->parNames[ i ].find( enh + "_yield_" + plc2 ) != string::npos ){
				return pars[ i ];	
			}
		}
		return 0.0;
	}

	double Fitter::penalizeYields( int npar , double * pars ){

		double penalty = 1.0;
		for( string plc : PidPhaseSpace::species ){
			double cy = currentYield( plc, npar, pars );
			for( string plc2 : PidPhaseSpace::species ){
				double cey = currentYield( "zb_"+plc2, plc, npar, pars );
				if ( cey > cy ){
					penalty *= (1.0 + ( cey - cy )/cey);
				}
			}
		}

		for( string plc : PidPhaseSpace::species ){
			double cy = currentYield( plc, npar, pars );
			double cey = currentYield( "zb_"+plc, plc, npar, pars );
			if ( cey < cy * .30 ){
				penalty *= (1.0 + ( cey - (cy * .30) )/cey);
			}
			
		}
		return penalty;
	}


	void Fitter::fixShapes(){
		for ( int i = 0; i < parNames.size(); i++ ){

			bool shouldFix = false;
			if ( string::npos != parNames[ i ].find( "sig" ) || string::npos != parNames[ i ].find( "mu" ) )
				shouldFix = true;

			for ( string plc : PidPhaseSpace::species ){
				if ( string::npos != parNames[ i ].find( "zb_" + plc +"_yield_" +plc ) )
					shouldFix = true;
			}

			if ( schema->vars[ parNames[ i ] ]->fixed || shouldFix )
				minuit->FixParameter( i );
		}
	}

	void Fitter::releaseShapes(){
		for ( int i = 0; i < parNames.size(); i++ ){

			bool shouldFix = false;
			if ( string::npos != parNames[ i ].find( "sig" ) || string::npos != parNames[ i ].find( "mu" ) )
				shouldFix = true;

			for ( string plc : PidPhaseSpace::species ){
				if ( string::npos != parNames[ i ].find( "zb_" + plc +"_yield_" +plc ) )
					shouldFix = true;
			}

			if ( schema->vars[ parNames[ i ] ]->fixed || shouldFix )
				minuit->Release( i );
		}
	}


}
