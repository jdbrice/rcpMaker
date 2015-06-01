#include "TSF/Fitter.h"
#include "PidPhaseSpace.h"


namespace TSF{
	
	vector<double> Fitter::convergence;

	Fitter * Fitter::self = nullptr;
	Fitter::Fitter( shared_ptr<FitSchema> _schema, TFile * _dataFile ){

		self = this;
		schema = _schema;
		dataFile = _dataFile;
		minuit = unique_ptr<TMinuit>( new TMinuit( schema->numParams() ) );
		logger = unique_ptr<Logger>( new Logger() );
		logger->setClassSpace( "Fitter" );


		for ( auto k : schema->vars ){
			if ( k.second->exclude ) continue;
			parNames.push_back( k.first );
		}

		for ( int i = 0; i < parNames.size(); i++ ){
			if (schema->vars[ parNames[ i ] ]->exclude) continue;

			if ( 0 <= schema->vars[ parNames[ i ] ]->error )
				schema->vars[ parNames[ i ] ]->error = 0.001;

			minuit->DefineParameter( i, 		// parameter index
						parNames[ i ].c_str(), 	// name
						schema->vars[ parNames[ i ] ]->val, 
						schema->vars[ parNames[ i ] ]->error,
						schema->vars[ parNames[ i ] ]->min,
						schema->vars[ parNames[ i ] ]->max );

			if ( schema->vars[ parNames[ i ] ]->fixed )
				minuit->FixParameter( i );

		}

		minuit->SetPrintLevel( 0 );
		minuit->SetFCN( tminuitFCN );

		logger->info(__FUNCTION__) << endl;
	}


	Fitter::~Fitter(){

	}

	void Fitter::tminuitFCN(int &npar, double *gin, double &f, double *par, int flag){

		double fnVal = 0.0;
		updateParameters( npar, par );

		bool useRange = self->schema->constrainFitRange();
		string method = self->schema->getMethod();

		double normFactor = ( 100.0 / self->norm );

		// loop on datasets
		for ( auto k : self->schema->datasets ){

			string ds = k.first;

			// loop on datapoints
			for ( auto d : k.second ){
				
				if ( useRange && !self->schema->inRange( ds, d.x ))
					continue;


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
				} else {
					cout << "No Fit method" << endl;
				}
			} // loop on data points

			if ( "nll" == method){
				double mYield = modelYield( ds );
				double dsYield = k.second.yield(self->schema->getRanges()) /* ( normFactor )*/;
				//cout << "ds : " << ds << endl;
				//cout << "dsYield : " << dsYield << ", " << mYield << " dif = " << abs( dsYield - mYield ) << endl;
				// subtract off this dataset's (N - E) term
				//fnVal = fnVal + abs( dsYield - mYield );
				fnVal = fnVal - ( dsYield - mYield );
			}

		}

		//convergence.push_back( fnVal );
		//cout << " nll = " << fnVal << endl;
		f = fnVal  / normFactor ;		
	}

	bool Fitter::loadDatasets( string cs, int charge, int cenBin, int ptBin, int etaBin ){
		logger->info(__FUNCTION__) << endl;

		map< string, TH1D* > zb;
		map< string, TH1D* > zd;

		dataHists[ "zb_All"] = (TH1*)dataFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin )).c_str() 		);
		dataHists[ "zd_All"] = (TH1*)dataFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin )).c_str() 	);
		
		// dEdx enhanced distributions
		dataHists[ "zd_K" ]		= (TH1D*)dataFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin, "K" )).c_str() );
		dataHists[ "zd_Pi" ]	= (TH1D*)dataFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin, "Pi" )).c_str() );
		dataHists[ "zd_P" ]		= (TH1D*)dataFile->Get( ("dedx/" + PidPhaseSpace::dedxName( cs, charge, cenBin, ptBin, etaBin, "P" )).c_str() );

		// 1/beta enhanced distributions
		dataHists[ "zb_Pi" ] 	= (TH1D*)dataFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "Pi" )).c_str() );
		dataHists[ "zb_K" ] 	= (TH1D*)dataFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "K" )).c_str() );
		dataHists[ "zb_P" ] 	= (TH1D*)dataFile->Get( ("tof/" + PidPhaseSpace::tofName( cs, charge, cenBin, ptBin, etaBin, "P" )).c_str() );
		
		
		norm = dataHists[ "zd_All"]->Integral();
		bool doFit = true;

		for ( auto k : dataHists ){

			double nObs = k.second->GetEntries();
			logger->debug(__FUNCTION__) << "Num Entries in " << k.first << " : " << nObs << endl;

			// normalize
			k.second->Sumw2();
			k.second->Scale( 100.0 / norm );
			schema->loadDataset( k.first, k.second );
			
			/*if ( nObs < 300 ){
				doFit = false;
			}*/
		}


		return doFit;
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
		logger->info(__FUNCTION__) << "OK Starting for ( " << cs << ", " << charge << ", " << cenBin << ", " << ptBin << ", " << etaBin << " ) " << endl;

		// load the current datasets
		double sufficienctStatistics = loadDatasets( cs, charge, cenBin, ptBin, etaBin );

		if ( !sufficienctStatistics ){
			fitIsGood = false;
			logger->warn( __FUNCTION__ ) << "Insufficient Statistics : Skipping " << endl;
			return;
		}


		//updateParameters();
		//return;

		double arglist[10];
      	arglist[0] = -1;
      	int iFlag = 0;
      	fitIsGood = false; 

      	

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



		//minuit->mnexcm( "STATUS", arglist, 1, iFlag ); // get errors

      	// if ( iFlag > 0 )
      	// 	minuit->mnexcm( "MINOS", arglist, 1, iFlag );
      	// else
      	minuit->mnexcm( "HESSE", arglist, 1, iFlag ); // get errors

      	/*for ( int i = 0; i < parNames.size(); i++ ){
			//schema->vars[ parNames[ i ] ]->;
			minuit->Release( i );
		}*/

		cout << "iFlag " << iFlag << endl;
		//if ( 0 == iFlag )
			fitIsGood = true;

		//logger->info(__FUNCTION__) << "Calculating errors with HESE" << endl;
		//minuit->mnexcm( "HESSE", arglist, 1, iFlag );

		//minuit->mnexcm( "MINOS", arglist, 1, iFlag );

		

		// get the final state of all variables 
		logger->info(__FUNCTION__) << "Updating parameters after Fit" << endl;
		updateParameters();
		schema->reportModels();




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




}
