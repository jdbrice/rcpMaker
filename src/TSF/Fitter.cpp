#include "TSF/Fitter.h"
#include "Spectra/PidHistoMaker.h"


namespace TSF{
	
	vector<double> Fitter::convergence;

	Fitter * Fitter::self = nullptr;
	Fitter::Fitter( shared_ptr<FitSchema> _schema, TFile * _dataFile ){

		self = this;
		schema = _schema;
		dataFile = _dataFile;

		INFO( tag, "( schema=" << _schema << ", datafile=" << _dataFile << " )" )
	}

	void Fitter::setupFit(){
		minuit = unique_ptr<TMinuit>( new TMinuit( schema->numParams() ) );
		
		for ( auto k : schema->getVars() ){
			if ( k.second->exclude ) continue;
			parNames.push_back( k.first );
		}

		for ( int i = 0; i < parNames.size(); i++ ){
			if (schema->var( parNames[ i ] )->exclude) continue;

			if ( 0 <= schema->var( parNames[ i ] )->error )
				schema->var( parNames[ i ] )->error = 0.001;

			minuit->DefineParameter( i, 						// parameter index
						parNames[ i ].c_str(), 					// name
						schema->var( parNames[ i ] )->val, 		// initial value
						schema->var( parNames[ i ] )->error,	// intial step size
						schema->var( parNames[ i ] )->min,		// limit min
						schema->var( parNames[ i ] )->max );	// limit max

			if ( schema->var( parNames[ i ] )->fixed )
				minuit->FixParameter( i );

		}

		minuit->SetPrintLevel( schema->getVerbosity() );
		minuit->SetFCN( tminuitFCN );

		INFO( tag, "" )
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
					double modelVal = self->modelEval( ds, d.x );
					fnVal += chi2( d.y, modelVal, d.ey );	
				} else if ( "nll" == method ){
					// Minimize by negative log likelihood
					double modelVal = self->modelEval( ds, d.x );
					
					fnVal += nll( d.y, modelVal );
				} else if ( "poisson" == method ){
					// Minimize by poisson errors ie error = sqrt( N )
					if ( 0 >= d.y || 0 >= d.ey )
						continue;
					double modelVal = self->modelEval( ds, d.x );
					fnVal += poisson( d.y, modelVal );	
				} else if ( "fractional" == method ){
					// Minimize by fractional errors ie error =  N 
					if ( 0 >= d.y || 0 >= d.ey )
						continue;
					double modelVal = self->modelEval( ds, d.x );
					fnVal += fractional( d.y, modelVal );	
				} else {
					cout << "No Fit method" << endl;
				}
			} // loop on data points

			if ( "nll" != self->schema->getMethod() ){

				if ( "zb_All" == k.first ){
					double mYield = modelYield( ds );
					double dsYield = k.second.yield( );
					// nothing needed here for chi2
					fnVal = fnVal + ( dsYield - mYield )*( dsYield - mYield ) * penaltyScale;
					//INFO( tag, "Extended chi2 component = " << ( dsYield - mYield )*( dsYield - mYield ) * penaltyScale )
				}
			} else {
				double mYield = modelYield( ds );
				double dsYield = k.second.yield( self->schema->getRanges() );

				// subtract off this dataset's (N - E) term
				fnVal = fnVal - ( dsYield - mYield );
			}
		}

		// enforces 1/beta mass ordering
		double penalty = self->enforceMassOrder( npar, par );
		penalty = 1.0;
		// enforce the enhanced yield less than total yield
		// penalty *= self->enforceEnhancedYields( npar, par );
		
		// enforce the average total tofEff
		// if ( !self->fixedEff ){
		// 	penalty *= self->enforceEff( npar, par );
		// }

		f = (fnVal ) * penalty;
	}

	void Fitter::loadDatasets( string cs, int charge, int cenBin, int ptBin ){
		INFO( tag, "( cs=" << cs << ", charge=" << charge << ", iCen=" <<cenBin << ", ptBin=" << ptBin << ")" )

		map< string, TH1D* > zb;
		map< string, TH1D* > zd;

		schema->clearDatasets();

		DEBUG( "Loading " << "tof/" + Common::zbName( cs, charge, cenBin, ptBin ) );
		DEBUG( "Loading " << "dedx/" + Common::zbName( cs, charge, cenBin, ptBin ) );
		dataHists[ "zb_All"] = (TH1*)dataFile->Get( ("tof/" + Common::zbName( cs, charge, cenBin, ptBin )).c_str() 		);
		dataHists[ "zd_All"] = (TH1*)dataFile->Get( ("dedx/" + Common::zdName( cs, charge, cenBin, ptBin )).c_str() 	);
		DEBUG( "got zb_All = " << dataHists[ "zb_All"] )
		DEBUG( "got zd_All = " << dataHists[ "zd_All"] )


		// dEdx enhanced distributions
		DEBUG( "Loading " << "dedx/" + Common::zbName( cs, charge, cenBin, ptBin, "Pi" ) );
		DEBUG( "Loading " << "dedx/" + Common::zbName( cs, charge, cenBin, ptBin, "K" ) );
		DEBUG( "Loading " << "dedx/" + Common::zbName( cs, charge, cenBin, ptBin, "P" ) );
		dataHists[ "zd_K" ]		= (TH1D*)dataFile->Get( ("dedx/" + Common::zdName( cs, charge, cenBin, ptBin, "K" )).c_str() );
		dataHists[ "zd_Pi" ]	= (TH1D*)dataFile->Get( ("dedx/" + Common::zdName( cs, charge, cenBin, ptBin, "Pi" )).c_str() );
		dataHists[ "zd_P" ]		= (TH1D*)dataFile->Get( ("dedx/" + Common::zdName( cs, charge, cenBin, ptBin, "P" )).c_str() );

		// 1/beta enhanced distributions
		DEBUG( "Loading " << "tof/" + Common::zbName( cs, charge, cenBin, ptBin, "Pi" ) );
		DEBUG( "Loading " << "tof/" + Common::zbName( cs, charge, cenBin, ptBin, "K" ) );
		DEBUG( "Loading " << "tof/" + Common::zbName( cs, charge, cenBin, ptBin, "P" ) );
		dataHists[ "zb_Pi" ] 	= (TH1D*)dataFile->Get( ("tof/" + Common::zbName( cs, charge, cenBin, ptBin, "Pi" )).c_str() );
		dataHists[ "zb_K" ] 	= (TH1D*)dataFile->Get( ("tof/" + Common::zbName( cs, charge, cenBin, ptBin, "K" )).c_str() );
		dataHists[ "zb_P" ] 	= (TH1D*)dataFile->Get( ("tof/" + Common::zbName( cs, charge, cenBin, ptBin, "P" )).c_str() );
		
		DEBUG( "Getting norm from : EventQA/mappedRefMultBins" )
		norm = ((TH1D*)dataFile->Get( "EventQA/mappedRefMultBins" ))->GetBinContent( cenBin + 1 );

		// Used for setting the scale when drawing
		double maxYield = dataHists[ "zd_All"]->Integral();
		schema->setNormalization( maxYield / norm );


		sufficienctStatistics = true;

		for ( auto k : dataHists ){

			if ( !k.second ){
				ERROR( "dataHist " << k.first << " : " << k.second )
				continue;
			}

			double nObs = k.second->GetEntries();
			INFO( tag, "Num Entries in " << k.first << " : " << nObs );

			// normalize
			k.second->Sumw2();
			k.second->Scale( 1.0 / norm );

			if ( nObs > 75 )
				schema->loadDataset( k.first, k.second );
		}

	}
	
	void Fitter::nop( ){

		INFO( "DATASETS:" )
		INFO( "yield_zb_All = " << schema->datasets[ "zb_All" ].yield() );
		INFO( "yield_zd_All = " << schema->datasets[ "zd_All" ].yield() );
		INFO( tag, "yield for zb_All inside roi = " << schema->datasets[ "zb_All" ].yield( schema->getRanges() ) );
		INFO( tag, "yield for zd_All inside roi = " << schema->datasets[ "zd_All" ].yield( schema->getRanges() ) );

		INFO( tag, "1 Sigma : " );
		schema->updateRanges( 1.0 );
		INFO( tag, "yield fraction in roi zb = " << schema->datasets[ "zb_All" ].yield( schema->getRanges() ) /schema->datasets[ "zb_All" ].yield()  );
		INFO( tag, "yield fraction in roi zd = " << schema->datasets[ "zd_All" ].yield( schema->getRanges() ) /schema->datasets[ "zd_All" ].yield()  );

		INFO( tag, "2 Sigma : " );
		schema->updateRanges( 2.0 );
		INFO( tag, "yield fraction in roi zb = " << schema->datasets[ "zb_All" ].yield( schema->getRanges() ) /schema->datasets[ "zb_All" ].yield()  );
		INFO( tag, "yield fraction in roi zd = " << schema->datasets[ "zd_All" ].yield( schema->getRanges() ) /schema->datasets[ "zd_All" ].yield()  );

		INFO( tag, "3 Sigma : " );
		schema->updateRanges( 3.0 );
		INFO( tag, "yield fraction in roi zb = " << schema->datasets[ "zb_All" ].yield( schema->getRanges() ) /schema->datasets[ "zb_All" ].yield()  );
		INFO( tag, "yield fraction in roi zd = " << schema->datasets[ "zd_All" ].yield( schema->getRanges() ) /schema->datasets[ "zd_All" ].yield()  );

		INFO( tag, "4 Sigma : " );
		schema->updateRanges( 4.0 );
		INFO( tag, "yield fraction in roi zb = " << schema->datasets[ "zb_All" ].yield( schema->getRanges() ) /schema->datasets[ "zb_All" ].yield()  );
		INFO( tag, "yield fraction in roi zd = " << schema->datasets[ "zd_All" ].yield( schema->getRanges() ) /schema->datasets[ "zd_All" ].yield()  );
		

		// get the final state of all variables 
		INFO( tag, "Updating parameters after setup" );
		updateParameters();
	}

	void Fitter::fit1( ){

		double arglist[10];
		arglist[ 0 ] = 50000;
		arglist[ 1 ] = 1.0;
		int iFlag = -1;
		string status = "na";

		schema->setMethod( "chi2" );

		fix( "eff" );
		fixShapes();
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			status = minuit->fCstatu;
			INFO ( tag, "Step 1. Status " << status );
		releaseShapes();
		release( "eff" );



		// get the final state of all variables 
		INFO( tag, "Updating parameters after Fit" );
		updateParameters();
	}

	void Fitter::fit2(  ){

		double arglist[10];
		arglist[ 0 ] = 50000;
		arglist[ 1 ] = 1.0;
		int iFlag = -1;
		string status = "na";

		schema->setMethod( "chi2" );

		
		fix( "eff" );
		fix( "yield" );
		release( "_yield_" );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			status = minuit->fCstatu;
			INFO ( tag, "Step 1. Status " << status );
		release( "yield" );
		release( "eff" );
		
		schema->updateRanges();

		// get the final state of all variables 
		INFO( tag, "Updating parameters after Fit" );
		updateParameters();
	}

	void Fitter::fit3(  ){

		double arglist[10];
		arglist[ 0 ] = 5000;
		arglist[ 1 ] = 1.0;
		int iFlag = -1;
		string status = "na";

		INFO( tag, "BEFORE" );
		reportFitStatus();

		schema->setMethod( "chi2" );

		fixShapes( );
		fix( "eff" );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			status = minuit->fCstatu;
			INFO ( tag, "Step 1. Status " << status );
		releaseShapes(  );
		release( "eff" );
		schema->updateRanges();

		INFO( tag, "BEFORE" );
		reportFitStatus();

		// get the final state of all variables 
		INFO( tag, "Updating parameters after Fit" );
		updateParameters();
	}

	void Fitter::fit4(  ){

		double arglist[10];
		arglist[ 0 ] = 5000;
		arglist[ 1 ] = 1.0;
		int iFlag = -1;
		string status = "na";

		schema->setMethod( "chi2" );

		
		fix("eff");
		release( "eff_Pi" );
		// fixShapes();
		fix( "_yield_" );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			minuit->mnexcm( "MINI", arglist, 1, iFlag );
			status = minuit->fCstatu;
			INFO ( tag, "Step 1. Status " << status );
		// releaseShapes();
		release( "_yield_" );
		release( "eff" );
		schema->updateRanges();

		// get the final state of all variables 
		INFO( tag, "Updating parameters after Fit" );
		updateParameters();
	}

	void Fitter::fitErrors(  ){

		double arglist[10];
		arglist[ 0 ] = 50000;
		arglist[ 1 ] = 1.0;
		int iFlag = -1;
		string status = "na";

		schema->setMethod( "chi2" );

		
		minuit->mnexcm( "HESSE", arglist, 1, iFlag );
		minuit->mnexcm( "HESSE", arglist, 1, iFlag );
		status = minuit->fCstatu;
		INFO ( tag, "Errors Status " << status );
	


		// get the final state of all variables 
		INFO( tag, "Updating parameters after Fit" );
		updateParameters();
	}

	void Fitter::setValue( string var, double val ){
		int pos = find( parNames.begin(), parNames.end(), var ) - parNames.begin();
		if ( pos >= parNames.size() ){
			ERROR( tag, "Parameter " << var << " not found" ); 
		} else {

			WARN( tag, "Setting value (parno=" << pos << ", value = " << val << " )"  )
			double arglist[10];
			arglist[ 0 ] = pos + 1;
			arglist[ 1 ] = val;
			int iFlag = 0;
			minuit->mnexcm( "SET PAR", arglist, 2, iFlag );
		}
	}

	// evaluate all the models for a given dataset
	double Fitter::modelEval( string dataset, double x ){

		double bw = schema->datasets[ dataset ].pointNear( x ).bw;
		double val = 0.0;
		for ( auto k : schema->models ){
			//cout << "name : " << k.first << endl;
			if ( k.second->dataset == dataset && players[ k.first ] ){ // second part is looking for active
				val += k.second->eval( x, bw );
			} else if ( !players[ k.first ] ){
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
		DEBUG( tag, "Plotting : " << datasetOrModel );

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
			WARN( tag, "Invalid Request : " << datasetOrModel );
			return new TGraph();
		}

		if ( "" != modelName && !self->players[ modelName ] ){
			INFO( tag, "Skipping inactive Player" );
			return new TGraph();
		}

		pair<double, double> xBounds = schema->datasets[ datasetName ].rangeX(  );

		DEBUG( tag, "Plotting from ( " << xBounds.first << ", " << xBounds.second <<" ) " );	
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
				self->schema->var( self->parNames[ i ] )->val = val;

			} else {
				double val = 0, err = 0;
				self->minuit->GetParameter( i, val, err );
				self->schema->var( self->parNames[ i ] )->val = val;
				self->schema->var( self->parNames[ i ] )->error = err;

			}
		}
		self->schema->updateModels( self->players );		
	}


	double Fitter::enforceEnhancedYields( int npar , double * pars ){

		double penalty = 1.0;
		for ( string plc1 : Common::species ){
			double cy = currentValue( "yield_" + plc1, npar, pars );
			for ( string pre : { "zb_", "zd_" } ){
				//for ( string plc2 : Common::species ){
					double cey = currentValue( pre + plc1 + "_yield_" + plc1, npar, pars );
					if ( cey > cy ){
						penalty *= ( 1.0 + ( cey - cy ) );
					}
				//}
			}
		}
		return penalty;
	}

	double Fitter::enforceMassOrder( int npar , double * pars ){

		double penalty = 1.0;

		if ( !schema->exists( "zb_mu_Pi" ) )
			return 1.0;

		float muPi 	= currentValue( "zb_mu_Pi", npar, pars );
		float muK 	= currentValue( "zb_mu_K", npar, pars );
		float muP 	= currentValue( "zb_mu_P", npar, pars );

		if ( muPi > muK ){
			penalty *= ( 1.0 + ( muPi - muK ) * ( muPi - muK ) * Fitter::penaltyScale );
		}
		if ( muPi > muP ){
			penalty *= ( 1.0 + ( muPi - muP ) * ( muPi - muP ) * Fitter::penaltyScale );
		}
		if ( muK > muP ){
			penalty *= ( 1.0 + ( muK - muP ) * ( muK - muP ) * Fitter::penaltyScale );
		}
		
		return penalty;
	}

	double Fitter::enforceEff(int npar , double * pars) {
		if (!schema->tofEff())
			return 1.0;
		double penalty = 1.0;


		// we are enforcing the average eff between Pi, K, P to be ~100% - since we already corrected the "total" toff eff
		float effPi = currentValue( "eff_Pi", npar, pars );
		float yPi = currentValue( "yield_Pi", npar, pars );
		float effK 	= currentValue( "eff_K", npar, pars );
		float yK = currentValue( "yield_K", npar, pars );
		float effP 	= currentValue( "eff_P", npar, pars );
		float yP = currentValue( "yield_P", npar, pars );

		// DEBUG( "Fitter_Eff", "eff_Pi = " << effPi )
		// DEBUG( "Fitter_Eff", "eff_K = " << effK )
		// DEBUG( "Fitter_Eff", "eff_P = " << effP )

		float fudge = 0.00;
		float sum = (effPi   + effK   + effP ) ;/// ( yPi + yK + yP) ;
		INFO( tag, "Eff weighted sum : " << sum )
		//if ( sum >= 3.0 - fudge && sum <= 3.0 + fudge ){
		//	return 1.0;
		//}

		float x = 3.0 + fudge - ( sum );
		if ( sum > 3.0 + fudge ){
			x = 3.0 + fudge - ( sum );
		} else if ( sum < 3.0 - fudge ){
			x = 3.0 - fudge - ( sum );
		}
		penalty = 1.0 + ( x * x ) ;
		
		return penalty;
	}

	double Fitter::currentValue( string var, int npar, double * pars ){
		// sanity check
		if ( npar > parNames.size() ){
			ERROR( tag, "npar = " << npar << ", parNames.size() = " << parNames.size() )
			ERROR( tag, "npar vs. parNames mismatch in size for " << var )
			return -9999.999;
		}
		// update the variables
		for ( int i = 0; i < parNames.size(); i++ ){
			
			if ( parNames[ i ].find( var ) != string::npos ){
				return pars[ i ];	
			}
		}

		ERROR( tag, var << " not found" )
		// not found
		return 9999.999;
	}

	// just a shortcut function
	void Fitter::fixShapes(){
		fix( "mu" );
		fix( "sig" );
	}

	// just a shortcut function
	void Fitter::releaseShapes(){
		release( "mu" );
		release( "sig" );
	}


	void Fitter::fix( string var){

		if ( "eff" == var || "eff_P" == var || "eff_Pi" == var || "eff_K" == var)
			fixedEff = true;

		// loop over parameters
		for ( int i = 0; i < parNames.size(); i++ ){

			bool shouldFix = false;
			if ( string::npos != parNames[ i ].find( var ) )
				shouldFix = true;

			if ( shouldFix )
				minuit->FixParameter( i );
		} // i
	}

	void Fitter::release( string var, bool check ){

		if ( "eff" == var || "eff_P" == var || "eff_Pi" == var || "eff_K" == var)
			fixedEff = false;

		// loop over parameters
		for ( int i = 0; i < parNames.size(); i++ ){

			bool shouldRelease = false;
			if ( string::npos != parNames[ i ].find( var ) )
				shouldRelease = true;

			if ( shouldRelease && ( !check || !schema->var( parNames[ i ] )->fixed) )
				minuit->Release( i );
		} // i
	}

}
