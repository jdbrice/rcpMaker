
#include "TSF/FitRunner.h"
#include "PidPhaseSpace.h"

#include "TGraph.h"

namespace TSF{
	FitRunner::FitRunner( XmlConfig * _cfg, string _np) 
	: HistoAnalyzer( _cfg, _np ){
		
		// Initialize the Phase Space Recentering Object
		tofSigmaIdeal = cfg->getDouble( nodePath+"PhaseSpaceRecentering.sigma:tof", 0.0012);
		dedxSigmaIdeal = cfg->getDouble( nodePath+"PhaseSpaceRecentering.sigma:dedx", 0.06);
		psr = new PhaseSpaceRecentering( dedxSigmaIdeal,
										 tofSigmaIdeal,
										 cfg->getString( nodePath+"Bichsel.table", "dedxBichsel.root"),
										 cfg->getInt( nodePath+"Bichsel.method", 0) );
		psrMethod = cfg->getString( nodePath+"PhaseSpaceRecentering.method", "traditional" );
		// alias the centered species for ease of use
		centerSpecies = cfg->getString( nodePath+"PhaseSpaceRecentering.centerSpecies", "K" );

		
		//Make the momentum transverse, eta, charge binning 
		binsPt = new HistoBins( cfg, "bin.pt" );
		binsEta = new HistoBins( cfg, "bin.eta" );
		binsCharge = new HistoBins( cfg, "bin.charge" );

		logger->setClassSpace( "FitRunner" );

		// setup reporters for the zb and zd fit projections
		zbReporter = unique_ptr<Reporter>(new Reporter( cfg->getString( nodePath + "output:path" ) + "rpTSF_zb.pdf",
			cfg->getInt( nodePath + "Reporter.output:width", 400 ), cfg->getInt( nodePath + "Reporter.output:height", 400 ) ) );
		zdReporter = unique_ptr<Reporter>(new Reporter( cfg->getString( nodePath + "output:path" ) + "rpTSF_zd.pdf",
			cfg->getInt( nodePath + "Reporter.output:width", 400 ), cfg->getInt( nodePath + "Reporter.output:height", 400 ) ) );

	}

	FitRunner::~FitRunner(){
	}


	void FitRunner::makeHistograms(){

		/**
		 * The bins to fit over
		 */
		vector<int> centralityFitBins = cfg->getIntVector( nodePath + "FitRange.centralityBins" );
		vector<int> etaFitBins = cfg->getIntVector( nodePath + "FitRange.etaBins" );
		vector<int> chargeFit = cfg->getIntVector( nodePath + "FitRange.charges" );

		book->cd();
		book->makeAll( nodePath + "histograms" );
		for ( int iCen : centralityFitBins ){
			for ( int iCharge : chargeFit ){
				for ( int iEta : etaFitBins ){
					
					for ( string plc : PidPhaseSpace::species ){
						// yield Histos
						book->cd( plc + "_yield" );
						string name = yieldName( plc, iCen, iCharge, iEta );
						book->clone( "/", "yield", plc+"_yield", name );

						// Mean Histos
						book->cd( plc + "_zbMu" );
						name = muName( plc, iCen, iCharge, iEta );
						book->clone( "/", "yield", plc+"_zbMu", name );

						book->cd( plc + "_zbMu" );
						name = "delta"+muName( plc, iCen, iCharge, iEta );
						book->clone( "/", "yield", plc+"_zbMu", name );

						book->cd( plc + "_zdMu" );
						name = muName( plc, iCen, iCharge, iEta );
						book->clone( "/", "yield", plc+"_zdMu", name );

						book->cd( plc + "_zdMu" );
						name = "delta" + muName( plc, iCen, iCharge, iEta );
						book->clone( "/", "yield", plc+"_zdMu", name );

						// Sigma Histos
						book->cd( plc + "_zbSigma" );
						name = sigmaName( plc, iCen, iCharge, iEta );
						book->clone( "/", "yield", plc+"_zbSigma", name );
						name = sigmaName( plc, iCen, iCharge, iEta ) + "_rel";
						book->clone( "/", "yield", plc+"_zbSigma", name );

						book->cd( plc + "_zdSigma" );
						name = sigmaName( plc, iCen, iCharge, iEta );
						book->clone( "/", "yield", plc+"_zdSigma", name );

					} // loop plc
				} // loop iEta
			} // loop iCharge
		} // loop iCen
	}

	void FitRunner::prepare( double avgP, int iCen ){

		//Constraints on the mu 	 
		double zbDeltaMu = cfg->getDouble( nodePath + "ParameterFixing.deltaMu:zb", 1.5 );
		double zdDeltaMu = cfg->getDouble( nodePath + "ParameterFixing.deltaMu:zd", 1.5 );
		
		//Constraints on the sigma  
		double zbDeltaSigma = cfg->getDouble( nodePath + "ParameterFixing.deltaSigma:zb", -1 );
		double zdDeltaSigma = cfg->getDouble( nodePath + "ParameterFixing.deltaSigma:zd", -1 );

		bool paramFixing = cfg->getBool( nodePath + "ParameterFixing:apply", true );

		// fit roi
		double roi = cfg->getDouble( nodePath + "FitSchema:roi", -1 );

		activePlayers.clear();
		for ( string plc : PidPhaseSpace::species ){

			// zb Parameters
			double zbMu = zbMean( plc, avgP );
			double zbSig = zbSigma( );

			// zd Parameters
			double zdMu = zdMean( plc, avgP );
			double zdSig = zdSigma( );
			
			// check if the sigmas should be fixed
			double zbMinParP = cfg->getDouble( nodePath + "ParameterFixing." + plc + ":zbSigma", 5.0 );
			double zdMinParP = cfg->getDouble( nodePath + "ParameterFixing." + plc + ":zdSigma", 5.0 );

			// value to shich zb sigma should be fixed
			double zbSigFix = schema->vars[ "zb_sigma_"+plc ]->val;

			if ( zbMinParP > 0 && avgP >= zbMinParP){
				schema->setInitialMu( "zb_mu_"+plc, zbMu, zbSigFix, 2.0 );
				schema->fixParameter( "zb_sigma_" + plc, zbSigFix, true );
			}
			else {
				schema->setInitialSigma( "zb_sigma_"+plc, zbSig, 0.005, 0.066);//0.006, 0.025 );
				schema->setInitialMu( "zb_mu_"+plc, zbMu, zbSig, zbDeltaMu );
			}

			schema->setInitialMu( "zd_mu_"+plc, zdMu, zdSig, zdDeltaMu );
			zdSigFix = schema->vars[ "zd_sigma_"+plc ]->val;
			if ( zdMinParP > 0 && avgP >= zdMinParP){	
				//schema->fixParameter( "zd_sigma_" + plc, zdSigFix, true );
				schema->setInitialSigma( "zd_sigma_"+plc, zdSigFix, zdSigFix - .005, zdSigFix + 0.005);
			}
			else 
				schema->setInitialSigma( "zd_sigma_"+plc, zdSig, 0.06, 0.08);
				
			
			choosePlayers( avgP, plc, roi );

			if ( avgP < 1.5 ){
				schema->vars[ "yield_" + plc ]->min = 0;
				schema->vars[ "yield_" + plc ]->max = schema->getNormalization() * 10;	
			} else {
				schema->vars[ "yield_" + plc ]->min = 0;
				schema->vars[ "yield_" + plc ]->max = schema->vars[ "yield_" + plc ]->val * 2;	
			}
			
		} // loop on plc to set initial vals
	}

	void FitRunner::choosePlayers( double avgP, string plc, double roi ){

		double zdOnly = cfg->getDouble( nodePath + "Timing:zdOnly" , 0.5 );
		double useZdEnhanced = cfg->getDouble( nodePath + "Timing:useZdEnhanced" , 0.6 );
		double useZbEnhanced = cfg->getDouble( nodePath + "Timing:useZbEnhanced" , 0.6 );
		double nSigZbEnhanced = cfg->getDouble( nodePath + "Timing:nSigZbEnhanced" , 3.0 );
		double nSigZdEnhanced = cfg->getDouble( nodePath + "Timing:nSigZdEnhanced" , 3.0 );

		if ( roi > 0 ){
			double zbSig = zbSigma( );
			double zbMu = zbMean( plc, avgP );
			double zdMu = zdMean( plc, avgP );
			double zdSig = zdSigma(  );

			if ( avgP > zdOnly )
				schema->addRange( "zb_All", zbMu - zbSig * roi, zbMu + zbSig * roi );
			schema->addRange( "zd_All", zdMu - zdSig * roi, zdMu + zdSig * roi );	
		}

		// always include the total yields
		activePlayers.push_back( "zd_All_g" + plc );
		schema->vars[ "yield_" + plc ]->exclude = false;

		// exclude all enhanced yields for a clean slate
		for ( string plc2 : PidPhaseSpace::species ){
			schema->vars[ "zb_"+plc+"_yield_"+plc2 ]->exclude = true;
			schema->vars[ "zd_"+plc+"_yield_"+plc2 ]->exclude = true;
		}


		if ( avgP >= zdOnly ){
			// remove the zb variables
			activePlayers.push_back( "zb_All_g" + plc );
			schema->vars[ "zb_sigma_" + plc ]->exclude 	= false;
			schema->vars[ "zb_mu_" + plc ]->exclude 	= false;
		} else {
			schema->vars[ "zb_sigma_" + plc ]->exclude = true;
			schema->vars[ "zb_mu_" + plc ]->exclude 	= true;
		}

		if ( avgP < useZdEnhanced ){
			for ( string plc2 : PidPhaseSpace::species ){
				schema->vars[ "zd_"+plc+"_yield_"+plc2 ]->exclude = true;
			}
		} else {

			double zbSig = zbSigma(  );
			double zbMu = zbMean( plc, avgP);

			

			for ( string plc2 : PidPhaseSpace::species ){
				double zdMu2 = zdMean( plc2, avgP );
				double zdSig2 = zdSigma(  );
				string var = "zd_"+plc+"_yield_"+plc2;
				double cv = schema->vars[ var ]->val;
				
				bool firstTimeIncluded = false;
				if ( schema->vars[ var ]->exclude )
					firstTimeIncluded = true;

				double zbMu2 = zbMean( plc2, avgP );
				double zbNd = ( zbMu - zbMu2 ) / zbSig;

				if ( abs( zbNd ) < nSigZbEnhanced && schema->datasetActive( "zd_" + plc ) ){
					activePlayers.push_back( "zd_" + plc + "_g" + plc2 );
					

					schema->vars[ var ]->exclude = false;
					schema->vars[ var ]->min = 0;
					schema->vars[ var ]->max = schema->getNormalization();

					schema->addRange( "zd_" + plc, zdMu2 - zdSig2 * roi, zdMu2 + zdSig2 * roi );

					if ( firstTimeIncluded && plc != plc2 ){
						schema->vars[ var ]->val = 1/schema->getNormalization();
						schema->vars[ var ]->error = 0.1/schema->getNormalization();
					}

				} else {
					schema->vars[ "zd_"+plc+"_yield_"+plc2 ]->exclude = true;
				}
			}

		}


		if ( avgP < useZbEnhanced ){
			for ( string plc2 : PidPhaseSpace::species ){
				schema->vars[ "zb_"+plc+"_yield_"+plc2 ]->exclude = true;
			}
		} else {

			double zdMu = zdMean( plc, avgP );
			double zdSig = zdSigma( );

			for ( string plc2 : PidPhaseSpace::species ){
				double zbSig2 = zbSigma(  );
				double zbMu2 = zbMean( plc2, avgP );

				string var = "zb_"+plc+"_yield_"+plc2;
				bool firstTimeIncluded = false;
				if ( schema->vars[ var ]->exclude )
					firstTimeIncluded = true;

				double zdMu2 = zdMean( plc2, avgP );
				double zdNd = ( zdMu - zdMu2 ) / zdSig;

				if ( abs( zdNd ) < nSigZdEnhanced && schema->datasetActive( "zb_" + plc ) ){
					activePlayers.push_back( "zb_" + plc + "_g" + plc2 );
					

					schema->vars[ var ]->exclude = false;
					schema->vars[ var ]->min = 0;
					schema->vars[ var ]->max = schema->getNormalization() * 10;

					if ( roi > 0 )
						schema->addRange( "zb_" + plc, zbMu2 - zbSig2 * roi, zbMu2 + zbSig2 * roi );

					if ( firstTimeIncluded && plc != plc2)
						schema->vars[ var ]->val = 0;

				} else {
					schema->vars[ "zb_"+plc+"_yield_"+plc2 ]->exclude = true;;
				}
			}

		}
	}

	void FitRunner::make(){

		//The bins to fit over
		vector<int> centralityFitBins = cfg->getIntVector( nodePath + "FitRange.centralityBins" );
		vector<int> etaFitBins = cfg->getIntVector( nodePath + "FitRange.etaBins" );
		vector<int> chargeFit = cfg->getIntVector( nodePath + "FitRange.charges" );

		// Make the histograms for storing the results
		makeHistograms();

		int firstPtBin = cfg->getInt( nodePath + "FitRange.ptBins:min", 0 );
		int lastPtBin = cfg->getInt( nodePath + "FitRange.ptBins:max", 1 );

		if ( lastPtBin >= binsPt->nBins() )
			lastPtBin = binsPt->nBins() - 1;

		 for ( int iCen : centralityFitBins ){
			for ( int iCharge : chargeFit ){
				for ( int iEta : etaFitBins ){
							
					
					//Create the schema and fitter 
					schema = shared_ptr<FitSchema>(new FitSchema( cfg, nodePath + "FitSchema" ));


					for ( int iPt = firstPtBin; iPt <= lastPtBin; iPt++ ){

						double avgP = averagePt( iPt );
						
						logger->warn(__FUNCTION__) << "<p> = " << avgP << endl;

						schema->clearRanges();

						DEBUG( "Creating fitter" );
						Fitter fitter( schema, inFile );
						
						// load the datasets from the file
						fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt, iEta);

						// prepare initial values, ranges, etc. for fit
						prepare( avgP, iCen );
						// build the minuit interface
						fitter.setupFit();
						// assign active players to this fit
						fitter.addPlayers( activePlayers );
						// do the fit
						fitter.fit( centerSpecies, iCharge, iCen, iPt, iEta );

						reportFit( &fitter, iPt );

						if ( fitter.isFitGood() )
							fillFitHistograms(iPt, iCen, iCharge, iEta, fitter.getNorm() / fitter.normFactor() );

					}// loop pt Bins
				} // loop eta bins
			} // loop charge bins
		} // loop centrality bins
	}

	void FitRunner::drawSet( string v, Fitter * fitter, int iPt ){
		logger->info(__FUNCTION__) << v << ", fitter=" << fitter << ", iPt=" << iPt << endl;
		TH1 * h = fitter->getDataHist( v );
		if ( !h ){
			logger->error(__FUNCTION__) << "Data histogram not found" << endl;
			return ;
		}
		h->Draw("pe");
		h->GetYaxis()->SetRangeUser( fitter->normFactor() * 1e-6, fitter->normFactor() * 2 );

		TGraph * sum = fitter->plotResult( v );
		sum->SetLineColor( kBlue );
		sum->Draw( "same" );
		
		vector<TGraph*> comps;
		vector<double> colors = { kRed, kOrange, kBlack };
		int i = 0;
		for ( string plc : PidPhaseSpace::species ){
			TGraph * g = fitter->plotResult( v+"_g"+plc );
			comps.push_back( g );
			g->SetLineColor( colors[ i ] );
			g->Draw( "same" );

			i++;
		}

		gPad->SetLogy(1);
	}

	


	void FitRunner::reportFit( Fitter * fitter, int iPt ){


		// plot the dedx then tof
		logger->info(__FUNCTION__) << "Reporting zd" << endl;
		zdReporter->newPage( 2, 2 );
		{
			drawSet( "zd_All", fitter, iPt );
			zdReporter->cd( 2, 1 );
			drawSet( "zd_Pi", fitter, iPt );
			zdReporter->cd( 1, 2 );
			drawSet( "zd_K", fitter, iPt );
			zdReporter->cd( 2, 2 );
			drawSet( "zd_P", fitter, iPt );
		}
		zdReporter->savePage();

		logger->info(__FUNCTION__) << "Reporting zb" << endl;
		zbReporter->newPage( 2, 2 );
		{
			drawSet( "zb_All", fitter, iPt );
			zbReporter->cd( 2, 1 );
			drawSet( "zb_Pi", fitter, iPt );
			zbReporter->cd( 1, 2 );
			drawSet( "zb_K", fitter, iPt );
			zbReporter->cd( 2, 2 );
			drawSet( "zb_P", fitter, iPt );
		}
		zbReporter->savePage();


	}


	void FitRunner::fillFitHistograms(int iPt, int iCen, int iCharge, int iEta, double norm ){

		double avgP = averageP( iPt, iEta );

		for ( string plc : PidPhaseSpace::species ){

			double zbMu = zbMean( plc, avgP );
			double zdMu = zdMean( plc, avgP );

			logger->info(__FUNCTION__) << "Filling Histograms for " << plc << endl;
			int iiPt = iPt + 1;

			// Yield			
			// shared yield doesnt exist for the pid Parameter fits
			if ( schema->vars.find( "yield_"+plc ) != schema->vars.end()  ){ 
				logger->info(__FUNCTION__) << "Filling Yield for " << plc << endl;
				string name = yieldName( plc, iCen, iCharge, iEta );
				book->cd( plc+"_yield");
				double sC = schema->vars[ "yield_"+plc ]->val * norm / book->get( name )->GetBinWidth( iiPt );
				double sE = schema->vars[ "yield_"+plc ]->error * norm / book->get( name )->GetBinWidth( iiPt );
				
				book->get( name )->SetBinContent( iiPt, sC );
				book->get( name )->SetBinError( iiPt, sE );
			}

			logger->info(__FUNCTION__) << "Filling Mus for " << plc << endl;
			//Mu
			// zb
			string name = muName( plc, iCen, iCharge, iEta );
			book->cd( plc+"_zbMu");
			double sC = schema->vars[ "zb_mu_"+plc ]->val;
			double sE = schema->vars[ "zb_mu_"+plc ]->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			name = "delta"+muName( plc, iCen, iCharge, iEta );
			book->cd( plc+"_zbMu");
			sC = schema->vars[ "zb_mu_"+plc ]->val - zbMu;
			sE = schema->vars[ "zb_mu_"+plc ]->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			// zd
			name = muName( plc, iCen, iCharge, iEta );
			book->cd( plc+"_zdMu");
			sC = schema->vars[ "zd_mu_"+plc ]->val;
			sE = schema->vars[ "zd_mu_"+plc ]->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			name = "delta"+muName( plc, iCen, iCharge, iEta );
			book->cd( plc+"_zdMu");
			sC = schema->vars[ "zd_mu_"+plc ]->val - zdMu;
			sE = schema->vars[ "zd_mu_"+plc ]->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			logger->info(__FUNCTION__) << "Filling Sigmas for " << plc << endl;
			//Sigma
			name = sigmaName( plc, iCen, iCharge, iEta );;
			book->cd( plc+"_zbSigma");
			sC = schema->vars[ "zb_sigma_"+plc ]->val;
			sE = schema->vars[ "zb_sigma_"+plc ]->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );


			name = sigmaName( plc, iCen, iCharge, iEta ) + "_rel";
			sC = schema->vars[ "zb_sigma_"+plc ]->val / schema->vars[ "zb_mu_"+plc ]->val;
			sE = schema->vars[ "zb_sigma_"+plc ]->error / schema->vars[ "zb_mu_"+plc ]->val;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );



			name = sigmaName( plc, iCen, iCharge, iEta );
			book->cd( plc+"_zdSigma");
			sC = schema->vars[ "zd_sigma_"+plc ]->val;
			sE = schema->vars[ "zd_sigma_"+plc ]->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

		}

		// just makes it so that the RooFit Frames aren't saved into HistoBook File
		inFile->cd();
	}



	string FitRunner::yieldName( string plc, int iCen, int charge, int iEta ){
		return "yield_" + plc + "_" + ts(iCen) + "_" + PidPhaseSpace::chargeString( charge ) + "_" + ts(iEta);
	}
	string FitRunner::sigmaName( string plc, int iCen, int charge, int iEta ){
		return "sigma_" + plc + "_" + ts(iCen) + "_" + PidPhaseSpace::chargeString( charge ) + "_" + ts(iEta);
	}
	string FitRunner::muName( string plc, int iCen, int charge, int iEta ){
		return "mu_" + plc + "_" + ts(iCen) + "_" + PidPhaseSpace::chargeString( charge ) + "_" + ts(iEta);
	}
	string FitRunner::fitName( int iPt, int iCen, int charge, int iEta ){
		return "fit_" + ts(iPt) + "_" + ts(iCen) + "_" + PidPhaseSpace::chargeString( charge ) + "_" + ts(iEta);
	}

}


