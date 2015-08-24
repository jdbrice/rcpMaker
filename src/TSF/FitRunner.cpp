
#include "TSF/FitRunner.h"
#include "Spectra/PidHistoMaker.h"

#include "TGraph.h"
#include "TBox.h"

namespace TSF{
	FitRunner::FitRunner( XmlConfig * _cfg, string _np) 
	: HistoAnalyzer( _cfg, _np ){
		
		// Initialize the Phase Space Recentering Object
		tofSigmaIdeal = cfg->getDouble( nodePath+"ZRecentering.sigma:tof", 0.0012);
		dedxSigmaIdeal = cfg->getDouble( nodePath+"ZRecentering.sigma:dedx", 0.06);
		psr = new ZRecentering( dedxSigmaIdeal,
										 tofSigmaIdeal,
										 cfg->getString( nodePath+"Bichsel.table", "dedxBichsel.root"),
										 cfg->getInt( nodePath+"Bichsel.method", 0) );
		psrMethod = cfg->getString( nodePath+"ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
		centerSpecies = cfg->getString( nodePath+"ZRecentering.centerSpecies", "K" );

		
		//Make the momentum transverse, eta, charge binning 
		binsPt = new HistoBins( cfg, "binning.pt" );
		

		logger->setClassSpace( tag );

		// setup reporters for the zb and zd fit projections
		zbReporter = unique_ptr<Reporter>(new Reporter( cfg->getString( nodePath + "output:path" ) + "rp_" + centerSpecies + "_TSF_zb.pdf",
			cfg->getInt( nodePath + "Reporter.output:width", 400 ), cfg->getInt( nodePath + "Reporter.output:height", 400 ) ) );
		zdReporter = unique_ptr<Reporter>(new Reporter( cfg->getString( nodePath + "output:path" ) + "rp_"+ centerSpecies +"_TSF_zd.pdf",
			cfg->getInt( nodePath + "Reporter.output:width", 400 ), cfg->getInt( nodePath + "Reporter.output:height", 400 ) ) );

		Logger::setGlobalLogLevel( Logger::logLevelFromString( cfg->getString( nodePath + "Logger:globalLogLevel" ) ) );



	}

	FitRunner::~FitRunner(){
	} 


	void FitRunner::makeHistograms(){

		/**
		 * The bins to fit over
		 */
		vector<int> centralityFitBins = cfg->getIntVector( nodePath + "FitRange.centralityBins" );
		vector<int> chargeFit = cfg->getIntVector( nodePath + "FitRange.charges" );

		book->cd();
		book->makeAll( nodePath + "histograms" );
		for ( int iCen : centralityFitBins ){
			for ( int iCharge : chargeFit ){
				for ( string plc : Common::species ){
					// yield Histos
					book->cd( plc + "_yield" );
					string name = Common::yieldName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_yield", name );

					// Mean Histos
					book->cd( plc + "_zbMu" );
					name = Common::muName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zbMu", name );

					book->cd( plc + "_zbMu" );
					name = "delta"+Common::muName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zbMu", name );

					book->cd( plc + "_zdMu" );
					name = Common::muName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zdMu", name );

					book->cd( plc + "_zdMu" );
					name = "delta" + Common::muName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zdMu", name );

					// Sigma Histos
					book->cd( plc + "_zbSigma" );
					name = Common::sigmaName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zbSigma", name );
					name = Common::sigmaName( plc, iCen, iCharge ) + "_rel";
					book->clone( "/", "yield", plc+"_zbSigma", name );

					book->cd( plc + "_zdSigma" );
					name = Common::sigmaName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_zdSigma", name );

					// Plc Tof Efficiency 
					book->cd( "tofEff" );
					book->clone( "/", "yield", "tofEff", Common::effName( plc, iCen, iCharge )  );

					for ( string plc2 : Common::species ){
						book->cd( "zb_enhanced" );
						book->clone("/", "yield", "zb_enhanced", Common::yieldName( plc, iCen, iCharge, plc2 ) );

						book->cd( "zd_enhanced" );
						book->clone("/", "yield", "zd_enhanced", Common::yieldName( plc, iCen, iCharge, plc2 ) );
					}


				} // loop plc
			} // loop iCharge
		} // loop iCen
	}

	void FitRunner::prepare( double avgP, int iCen ){
		TRACE( tag, "( avgP=" << avgP << ", iCen=" << iCen << ")" )

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
		
		for ( string plc : Common::species ){

			INFO( tag, "Setting up " << plc )
			if ( !schema->exists( "yield_" + plc ) ){
				WARN( "No Yield exists for " << plc << " so we are skipping it" )
				continue;
			}

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
			double zbSigFix = schema->var( "zb_sigma_"+plc )->val;

			if ( zbMinParP > 0 && avgP >= zbMinParP){	
				schema->setInitialMu( "zb_mu_"+plc, zbMu, zbSigFix, 10 );
				INFO( tag, "Fixing zb_sigma_" << plc << " to " << sigmaSets[ "zb_" + plc ].mean() )
				double hm = sigmaSets[ "zb_" + plc ].mean();
				//schema->fixParameter( "zb_sigma_" + plc, sigmaSets[ "zb_" + plc ].mean(), true );
				schema->setInitialSigma( "zb_sigma_"+plc, hm, hm - 0.0006, hm + 0.0006 );
			}
			else {
				schema->setInitialSigma( "zb_sigma_"+plc, zbSig, zbSig * 0.5, zbSig * 6 );
				schema->setInitialMu( "zb_mu_"+plc, zbMu, zbSig, zbDeltaMu );
			}

			schema->setInitialMu( "zd_mu_"+plc, zdMu, zdSig, 10 );
			zdSigFix = zdSigma(); //schema->var( "zd_sigma_"+plc )->val;
			if ( zdMinParP > 0 && avgP >= zdMinParP){	
				schema->setInitialMu( "zd_mu_"+plc, zdMu, zdSig, zdDeltaMu);
				//schema->fixParameter( "zd_sigma_" + plc, sigmaSets[ "zd_" + plc ].mean(), true );
				double hm = sigmaSets[ "zd_" + plc ].mean();
				schema->setInitialSigma( "zd_sigma_"+plc, hm, hm - 0.004, hm  );
			}
			else 
				schema->setInitialSigma( "zd_sigma_"+plc, zdSig, 0.04, 0.14);
				
			
			choosePlayers( avgP, plc, roi );

		
			schema->var( "yield_" + plc )->min = 0;
			schema->var( "yield_" + plc )->max = schema->getNormalization() * 10;	
			schema->var( "yield_" + plc )->val = .0001;

			double zdOnly = cfg->getDouble( nodePath + "Timing:zdOnly" , 0.5 );
			
			// TODO: decide on eff scheme
			schema->var( "eff_" + plc )->val = 1.0;
			if ( avgP <= 0.5 )
				schema->var( "eff_" + plc )->fixed = true;
			else 
				schema->var( "eff_" + plc )->fixed = false;
			
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
				schema->addRange( "zb_All", zbMu - zbSig * roi, zbMu + zbSig * roi, "zb_mu_" + plc, "zb_sigma_" + plc, roi );
		
			schema->addRange( "zd_All", zdMu - zdSig * roi, zdMu + zdSig * roi, "zd_mu_" + plc, "zd_sigma_" + plc, roi );	

		}

		// always include the total yields
		activePlayers.push_back( "zd_All_g" + plc );
		schema->var( "yield_" + plc )->exclude = false;

		// exclude all enhanced yields for a clean slate
		for ( string plc2 : Common::species ){
			schema->var( "zb_"+plc+"_yield_"+plc2 )->exclude = true;
			schema->var( "zd_"+plc+"_yield_"+plc2 )->exclude = true;
		}


		if ( avgP >= zdOnly ){
			// remove the zb variables
			activePlayers.push_back( "zb_All_g" + plc );
			schema->var( "zb_sigma_" + plc )->exclude 	= false;
			schema->var( "zb_mu_" + plc )->exclude 	= false;
		} else {
			schema->var( "zb_sigma_" + plc )->exclude = true;
			schema->var( "zb_mu_" + plc )->exclude 	= true;
		}

		if ( avgP < useZdEnhanced ){
			for ( string plc2 : Common::species ){
				schema->var( "zd_"+plc+"_yield_"+plc2 )->exclude = true;
			}
		} else {

			double zbSig = zbSigma(  );
			double zbMu = zbMean( plc, avgP);

			for ( string plc2 : Common::species ){
				double zdMu2 = zdMean( plc2, avgP );
				double zdSig2 = zdSigma(  );
				string var = "zd_"+plc+"_yield_"+plc2;
				double cv = schema->var( var )->val;
				
				bool firstTimeIncluded = false;
				if ( schema->var( var )->exclude )
					firstTimeIncluded = true;

				double zbMu2 = zbMean( plc2, avgP );
				double zbNd = ( zbMu - zbMu2 ) / zbSig;

				if ( abs( zbNd ) < nSigZbEnhanced && schema->datasetActive( "zd_" + plc ) ){
					activePlayers.push_back( "zd_" + plc + "_g" + plc2 );
					

					schema->var( var )->exclude = false;
					schema->var( var )->min = 0;
					schema->var( var )->max = schema->getNormalization() * 10;

					if ( roi > 0 )
						schema->addRange( "zd_" + plc, zdMu2 - zdSig2 * roi, zdMu2 + zdSig2 * roi, "zd_mu_" + plc2, "zd_sigma_" + plc2, roi );

					if ( firstTimeIncluded && plc != plc2 ){
						schema->var( var )->val = 1/schema->getNormalization();
						schema->var( var )->error = 0.1/schema->getNormalization();
					}
					// TODO: initial yield?
					// schema->var( var )->val = 0.00001;
					

				} else {
					schema->var( "zd_"+plc+"_yield_"+plc2 )->exclude = true;
				}
			}

		}


		if ( avgP < useZbEnhanced ){
			for ( string plc2 : Common::species ){
				schema->var( "zb_"+plc+"_yield_"+plc2 )->exclude = true;
			}
		} else {

			double zdMu = zdMean( plc, avgP );
			double zdSig = zdSigma( );

			for ( string plc2 : Common::species ){
				double zbSig2 = zbSigma(  );
				double zbMu2 = zbMean( plc2, avgP );

				string var = "zb_"+plc+"_yield_"+plc2;
				bool firstTimeIncluded = false;
				if ( schema->var( var )->exclude )
					firstTimeIncluded = true;

				double zdMu2 = zdMean( plc2, avgP );
				double zdNd = ( zdMu - zdMu2 ) / zdSig;

				if ( abs( zdNd ) < nSigZdEnhanced && schema->datasetActive( "zb_" + plc ) ){
					activePlayers.push_back( "zb_" + plc + "_g" + plc2 );
					

					schema->var( var )->exclude = false;
					schema->var( var )->min = 0;
					schema->var( var )->max = schema->getNormalization() * 10;

					if ( roi > 0 )
						schema->addRange( "zb_" + plc, zbMu2 - zbSig2 * roi, zbMu2 + zbSig2 * roi, "zb_mu_" + plc2, "zb_sigma_" + plc2, roi );

					if ( firstTimeIncluded && plc != plc2){
						schema->var( var )->val = 1/schema->getNormalization();
						schema->var( var )->error = 0.1/schema->getNormalization();
					}
					// TODO: initial yield?
					// schema->var( var )->val = 0.01;

				} else {
					schema->var( "zb_"+plc+"_yield_"+plc2 )->exclude = true;;
				}
			}

		}
	}

	void FitRunner::make(){

		if ( inFile == nullptr || inFile->IsOpen() == false ){
			ERROR( "Invalid input data file - can't make" )
			return;
		}


		//The bins to fit over
		vector<int> centralityFitBins = cfg->getIntVector( nodePath + "FitRange.centralityBins" );
		vector<int> chargeFit = cfg->getIntVector( nodePath + "FitRange.charges" );

		// Make the histograms for storing the results
		makeHistograms();

		int firstPtBin = cfg->getInt( nodePath + "FitRange.ptBins:min", 0 );
		int lastPtBin = cfg->getInt( nodePath + "FitRange.ptBins:max", 1 );

		if ( lastPtBin >= binsPt->nBins() )
			lastPtBin = binsPt->nBins() - 1;

		 for ( int iCen : centralityFitBins ){
			for ( int iCharge : chargeFit ){
							
				//Create the schema and fitter 
				schema = shared_ptr<FitSchema>(new FitSchema( cfg, nodePath + "FitSchema" ));

				sigmaSets.clear();
				for ( int iPt = firstPtBin; iPt <= lastPtBin; iPt++ ){

					double avgP = binAverageP( iPt );
					
					logger->warn(__FUNCTION__) << "<p> = " << avgP << endl;

					schema->clearRanges();

					DEBUG( "Fitter", "Creating fitter" );
					Fitter fitter( schema, inFile );
					
					// load the datasets from the file
					fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt );

					// prepare initial values, ranges, etc. for fit
					prepare( avgP, iCen );
					// build the minuit interface
					fitter.setupFit();
					// assign active players to this fit
					fitter.addPlayers( activePlayers );
					// do the fit
					fitter.nop();
					reportFit( &fitter, iPt );
					
					for ( int i = 0; i < 5; i++ ){
						
						fitter.fit1(  );
						reportFit( &fitter, iPt );

						fitter.fit2(  );
						reportFit( &fitter, iPt );

					}

					// for ( int i : { 0, 1, 2 } ){
						fitter.fit3(  );
						reportFit( &fitter, iPt );
					// }

					// for ( int i = 0; i < 3; i++ ){
					// 	fitter.fit4(  );
					// 	reportFit( &fitter, iPt );
					// }

					//fitter.fitErrors();


					schema->reportModels();

				
					if ( fitter.isFitGood() )
						fillFitHistograms(iPt, iCen, iCharge, fitter );

					
					for ( string pre : {"zb", "zd"} ){
						for ( string plc : Common::species ){

							double r1 = 0.6;
							double r2 = 1.0;

							INFO( tag, "SigmaHistory for " << pre << "_" << plc )
							if ( "zb" == pre && "P" == plc ){
								r1 = 1.2;
								r2 = 2.0;
							} else if ( "zb" == pre ){
								r1 = 0.7;
								r2 = 1.2;
							}
							
							sigmaSets[ pre+"_"+plc ].setRange( r1, r2 );
							sigmaSets[ pre+"_"+plc ].add( avgP, schema->var( pre + "_sigma_" + plc )->val ); 
						}
					}

				}// loop pt Bins
			} // loop charge bins
		} // loop centrality bins
	}

	void FitRunner::drawSet( string v, Fitter * fitter, int iPt ){
		logger->info(__FUNCTION__) << v << ", fitter=" << fitter << ", iPt=" << iPt << endl;
		TH1 * h = fitter->getDataHist( v );
		if ( !h ){
			WARN( tag, "Data histogram not found" );
			return ;
		}
		h->Draw("pe");
		h->SetLineColor( kBlack );
		double scaler = 1e-6;
		h->GetYaxis()->SetRangeUser( schema->getNormalization() * scaler, schema->getNormalization() * 0.2 );

		h->SetTitle( ( dts((*binsPt)[ iPt ]) + " < pT < " + dts( (*binsPt)[ iPt + 1 ] ) ).c_str() );

		// draw boxes to show the fit ranges
		for ( FitRange range : fitter->getSchema()->getRanges() ){
			if ( range.dataset != v )
				continue;
			TBox * b1 = new TBox( range.min, schema->getNormalization() * scaler, range.max, schema->getNormalization() * 0.2  );
			b1->SetFillColorAlpha( kBlack, 0.25 );
			b1->SetFillStyle( 1001 );
			b1->Draw(  );
		}
		
	

		h->Draw("pe same");

		TGraph * sum = fitter->plotResult( v );
		sum->SetLineColor( kBlue );
		sum->SetLineWidth( 1 );
		sum->Draw( "same" );
		
		vector<TGraph*> comps;
		vector<double> colors = { kOrange, kRed - 3, kGreen - 3, kBlue - 3 };
		int i = 0;
		
		for ( string plc : Common::species ){
			TGraph * g = fitter->plotResult( v+"_g"+plc );
			comps.push_back( g );
			g->SetLineColor( colors[ i ] );
			g->SetLineWidth( 1 );
			g->Draw( "same" );

			i++;
		}

		gPad->SetGrid( 1, 1 );
		gPad->SetLogy(1);
	}

	void FitRunner::reportFit( Fitter * fitter, int iPt ){

		gStyle->SetOptStat(0);
		bool drawBig = false;
		bool drawFitRatios=false;

		if ( drawBig ){
			INFO( tag, "Reporting zd" );
			zdReporter->newPage( 1, 2 );
			{
				zdReporter->cd( 1, 1 );
				drawSet( "zd_All", fitter, iPt );
			}
		
			INFO( tag, "Drawing fit vs. data ratios for zd" )
			{
				zdReporter->cd( 1, 2 );
				drawFitRatio( "zd_All", fitter, iPt );
			}
			zdReporter->savePage();


			INFO( tag, "Reporting zb" );
			zbReporter->newPage( 1, 2 );
			{
				zbReporter->cd( 1, 1 );
				drawSet( "zb_All", fitter, iPt );
			}
		
			INFO( tag, "Drawing fit vs. data ratios for zb" )
			{
				zbReporter->cd( 1, 2 );
				drawFitRatio( "zb_All", fitter, iPt );
			}
			zbReporter->savePage();
		}

		// plot the dedx then tof
		logger->info(__FUNCTION__) << "Reporting zd" << endl;
		zdReporter->newPage( 2, 2 );
		{
			zdReporter->cd( 1, 1 );
			drawSet( "zd_All", fitter, iPt );
			zdReporter->cd( 2, 1 );
			drawSet( "zd_Pi", fitter, iPt );
			zdReporter->cd( 1, 2 );
			drawSet( "zd_K", fitter, iPt );
			zdReporter->cd( 2, 2 );
			drawSet( "zd_P", fitter, iPt );
		}
		zdReporter->savePage();

		if ( drawFitRatios ){
			INFO( tag, "Drawing fit vs. data ratios for zd" )
			zdReporter->newPage( 2, 2 );
			{
				zdReporter->cd( 1, 1 );
				drawFitRatio( "zd_All", fitter, iPt );
				zdReporter->cd( 2, 1 );
				drawFitRatio( "zd_Pi", fitter, iPt );
				zdReporter->cd( 1, 2 );
				drawFitRatio( "zd_K", fitter, iPt );
				zdReporter->cd( 2, 2 );
				drawFitRatio( "zd_P", fitter, iPt );
			}
			zdReporter->savePage();
		}



		INFO( tag, "Reporting zb" )
		zbReporter->newPage( 2, 2 );
		{
			zbReporter->cd( 1, 1 );
			drawSet( "zb_All", fitter, iPt );
			zbReporter->cd( 2, 1 );
			drawSet( "zb_Pi", fitter, iPt );
			zbReporter->cd( 1, 2 );
			drawSet( "zb_K", fitter, iPt );
			zbReporter->cd( 2, 2 );
			drawSet( "zb_P", fitter, iPt );
		}
		zbReporter->savePage();

		if ( drawFitRatios ){
			INFO( tag, "Drawing fit vs. data ratios for zb" )
			zbReporter->newPage( 2, 2 );
			{
				zbReporter->cd( 1, 1 );
				drawFitRatio( "zb_All", fitter, iPt );
				zbReporter->cd( 2, 1 );
				drawFitRatio( "zb_Pi", fitter, iPt );
				zbReporter->cd( 1, 2 );
				drawFitRatio( "zb_K", fitter, iPt );
				zbReporter->cd( 2, 2 );
				drawFitRatio( "zb_P", fitter, iPt );
			}
			zbReporter->savePage();
		}


	}


	void FitRunner::fillFitHistograms(int iPt, int iCen, int iCharge, Fitter &fitter ){

		double avgP = binAverageP( iPt );

		for ( string plc : Common::species ){

			double zbMu = zbMean( plc, avgP );
			double zdMu = zdMean( plc, avgP );

			logger->info(__FUNCTION__) << "Filling Histograms for " << plc << endl;
			int iiPt = iPt + 1;

			// Yield			
			logger->info(__FUNCTION__) << "Filling Yield for " << plc << endl;
			string name = Common::yieldName( plc, iCen, iCharge );
			book->cd( plc+"_yield");
			double sC = schema->var( "yield_"+plc )->val / book->get( name )->GetBinWidth( iiPt );
			double sE = schema->var( "yield_"+plc )->error;
			
			// TODO: add the fit error in quadrature ?
			//double N = sC * fitter.getNorm();
			//sE = sqrt( 1/N + sE*sE );

			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			// block for sep yields
			if ( schema->exists( "zd_yield_" + plc ) && schema->exists( "zb_yield_" + plc ) ){
				string name = Common::yieldName( plc, iCen, iCharge );
				book->cd( plc+"_yield");
				double zdC = schema->var( "zd_yield_"+plc )->val / book->get( name )->GetBinWidth( iiPt );
				double zdE = schema->var( "zd_yield_"+plc )->error / book->get( name )->GetBinWidth( iiPt );
				book->setBin( name, iiPt, zdC, zdE );

				double zbC = schema->var( "zb_yield_"+plc )->val / book->get( name )->GetBinWidth( iiPt );
				double zbE = schema->var( "zb_yield_"+plc )->error / book->get( name )->GetBinWidth( iiPt );

				book->cd( "tofEff" );
				book->setBin( Common::effName( plc, iCen, iCharge ), iiPt, 
					zbC / zdC, ((zbE / zbC) + ( zdE / zdC )) * ( zbC / zdC )  );

			}

			logger->info(__FUNCTION__) << "Filling Mus for " << plc << endl;
			//Mu
			// zb
			name = Common::muName( plc, iCen, iCharge );
			book->cd( plc+"_zbMu");
			sC = schema->var( "zb_mu_"+plc )->val;
			sE = schema->var( "zb_mu_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			name = "delta"+Common::muName( plc, iCen, iCharge );
			book->cd( plc+"_zbMu");
			sC = schema->var( "zb_mu_"+plc )->val - zbMu;
			sE = schema->var( "zb_mu_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			// zd
			name = Common::muName( plc, iCen, iCharge );
			book->cd( plc+"_zdMu");
			sC = schema->var( "zd_mu_"+plc )->val;
			sE = schema->var( "zd_mu_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			name = "delta"+Common::muName( plc, iCen, iCharge );
			book->cd( plc+"_zdMu");
			sC = schema->var( "zd_mu_"+plc )->val - zdMu;
			sE = schema->var( "zd_mu_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			logger->info(__FUNCTION__) << "Filling Sigmas for " << plc << endl;
			//Sigma
			name = Common::sigmaName( plc, iCen, iCharge );;
			book->cd( plc+"_zbSigma");
			sC = schema->var( "zb_sigma_"+plc )->val;
			sE = schema->var( "zb_sigma_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );


			name = Common::sigmaName( plc, iCen, iCharge ) + "_rel";
			sC = schema->var( "zb_sigma_"+plc )->val / schema->var( "zb_mu_"+plc )->val;
			sE = schema->var( "zb_sigma_"+plc )->error / schema->var( "zb_mu_"+plc )->val;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );



			name = Common::sigmaName( plc, iCen, iCharge );
			book->cd( plc+"_zdSigma");
			sC = schema->var( "zd_sigma_"+plc )->val;
			sE = schema->var( "zd_sigma_"+plc )->error;
			
			book->get( name )->SetBinContent( iiPt, sC );
			book->get( name )->SetBinError( iiPt, sE );

			if ( schema->exists( "eff_" + plc ) ){
				book->cd( "tofEff" );
				book->setBin( Common::effName( plc, iCen, iCharge ), iiPt, 
						schema->var( "eff_" + plc )->val, schema->var( "eff_" + plc )->error );
			}

			for ( string plc2 : Common::species ){
				fillEnhancedYieldHistogram( plc, iiPt, iCen, iCharge, plc2, fitter );
			}
			

		}
	}

	void FitRunner::fillEnhancedYieldHistogram( string plc1, int iPt, int iCen, int iCharge, string plc2, Fitter &fitter ){
		
		for ( string pre : { "zb_", "zd_" } ){
			book->cd( pre + "enhanced");
			
			string vName = pre + plc1 + "_yield_" + plc2;
			string hName = Common::yieldName( plc1, iCen, iCharge, plc2 );

			double sC = schema->var( vName )->val / book->get( hName )->GetBinWidth( iPt );
			double sE = schema->var( vName )->error;
			book->setBin( hName, iPt, sC, sE );
		}
	}



	void FitRunner::drawFitRatio( string ds, Fitter * fitter, int iPt ){

		book->cd();
		// clone and renaim to avoid naming clashes
		TH1 * h = (TH1*)fitter->getDataHist( ds )->Clone( (ds + "_vs_" + ts(iPt) ).c_str() );
		TH1 * h2 = (TH1*)fitter->getDataHist( ds )->Clone( (ds + "_vs2_" + ts(iPt) ).c_str() );
		h->SetTitle( (ts(iPt) + "; z;(fit - data) / K_{yield}").c_str() );
		h->Reset();

		h2->SetTitle( " ; z;(fit - data)^{2} / K_{yield}" );
		h2->Reset();

		shared_ptr<FitSchema> schema = fitter->getSchema();
		for ( auto dp : schema->datasets[ ds ]){
			

			double yTh = fitter->modelEval( ds, dp.x );
			
			double ratio = yTh - dp.y;


			int bin = h->GetXaxis()->FindBin( dp.x );
			DEBUG( tag, "x, y = " << dp.x << ", " << dp.y )
			DEBUG( tag, "yTh = " << yTh )
			DEBUG( tag, "ratio = " << ratio )
			
			double scale = schema->var( "yield_K" )->val;
			double r = 0.02;
			h->GetYaxis()->SetRangeUser( -r, r );
			h->SetBinContent( bin, ratio / scale );
			h->SetBinError( bin, dp.ey);

			h2->SetBinContent( bin, ratio*ratio * 10 );
			h2->SetBinError( bin, dp.ey);

		}

		h->Draw();
		//h2->SetMarkerColor( kRed );
		//h2->SetLineColor( kRed );
		//h2->Draw("same");
	}
}


