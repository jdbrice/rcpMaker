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
			parNames.push_back( k.first );
		}

		for ( int i = 0; i < parNames.size(); i++ ){

			minuit->DefineParameter( i, 		// parameter index
						parNames[ i ].c_str(), 	// name
						schema->vars[ parNames[ i ] ]->val, 
						schema->vars[ parNames[ i ] ]->error,
						schema->vars[ parNames[ i ] ]->min,
						schema->vars[ parNames[ i ] ]->max );

			if ( schema->vars[ parNames[ i ] ]->fixed )
				minuit->FixParameter( i );

		}

		minuit->SetPrintLevel( -1 );
		minuit->SetFCN( tminuitFCN );

		logger->info(__FUNCTION__) << endl;
	}


	Fitter::~Fitter(){

	}

	void Fitter::tminuitFCN(int &npar, double *gin, double &f, double *par, int flag){

		double fnVal = 0.0;
		updateParameters( npar, par );

		bool useRange = self->schema->constrainFitRange();

		// loop on datasets
		for ( auto k : self->schema->datasets ){

			string ds = k.first;

			// loop on datapoints
			for ( auto d : k.second ){
				
				if ( useRange && !self->schema->inRange( ds, d.x ))
					continue;


				if ( "nll" != self->schema->getMethod() ){
					// Minimize by chi^2
					if ( 0 == d.ey  )
						continue;
					double modelVal = modelEval( ds, d.x );
					fnVal += chi2( d.y, modelVal, d.ey );	
				} else {
					// Minimize by negative log likelihood
					double modelVal = modelEval( ds, d.x );
					//cout << "Model = " << modelVal << endl;
					//cout << "nllModel = " << nll( d.y, modelVal ) << endl;
					fnVal += nll( d.y, modelVal );
					//cout << "nll = " << nll( d.y, modelVal ) << endl; 
				}
			} // loop on data points

			if ( "nll" != self->schema->getMethod() ){
				// nothing needed here for chi2
			} else {
				double mYield = modelYield( ds );
				double dsYield = k.second.yield();

				// subtract off this dataset's (N - E) term
				fnVal = fnVal - ( dsYield - mYield );
			}

		}

		convergence.push_back( fnVal );
		//cout << " nll = " << fnVal << endl;
		f = fnVal;		
	}

	void Fitter::loadDatasets( string cs, int charge, int cenBin, int ptBin, int etaBin ){
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
		

		for ( auto k : dataHists ){
			schema->loadDataset( k.first, k.second );
		}
	}

	void Fitter::fit( string cs, int charge, int cenBin, int ptBin, int etaBin ){
		logger->info(__FUNCTION__) << endl;

		// load the current datasets
		loadDatasets( cs, charge, cenBin, ptBin, etaBin );

		convergence.clear();

		//return;
		double arglist[10];
      	arglist[0] = -1;
      	int iFlag = 0;
      	fitIsGood = false;

      	if ( schema->constrainFitRange() ){
      		schema->reportFitRanges();
      	}

      	arglist[ 0 ] = 2; // set minuit to value percison over speed
      	//minuit->mnexcm( "SET STR", arglist, 1, iFlag );

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

      	if ( iFlag > 0 ){
      		logger->info(__FUNCTION__) << "Poor convergence -> Calling MINOS" << endl;
      		minuit->mnexcm( "MINOS", arglist, 1, iFlag );
      	}

		cout << "iFlag " << iFlag << endl;
		if ( 0 == iFlag )
			fitIsGood = true;

		logger->info(__FUNCTION__) << "Calculating errors with HESE" << endl;
		minuit->mnexcm( "HESSE", arglist, 1, iFlag );

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
						
			if ( k.second->dataset == dataset ){
				val += k.second->eval( x, bw );
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
		logger->info(__FUNCTION__) << "Plotting : " << datasetOrModel << endl;

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

		pair<double, double> xBounds = schema->datasets[ datasetName ].rangeX(  );

		cout << "Plotting from ( " << xBounds.first << ", " << xBounds.second <<" ) "<< endl;	
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




}
