
#include "TSF/TpcFitRunner.h"
#include "Spectra/PidHistoMaker.h"

#include "TGraph.h"
#include "TBox.h"

#include "ANSIColors.h"

namespace TSF{
	TpcFitRunner::TpcFitRunner( ){
	}

	void TpcFitRunner::initialize(  ) {
		


		// Initialize the Phase Space Recentering Object
		tofSigmaIdeal = config.getDouble( nodePath+".ZRecentering.sigma:tof", 0.0012);
		dedxSigmaIdeal = config.getDouble( nodePath+".ZRecentering.sigma:dedx", 0.06);
		psr = new ZRecentering( dedxSigmaIdeal,
										 tofSigmaIdeal,
										 config.getString( nodePath+".Bichsel.table", "dedxBichsel.root"),
										 config.getInt( nodePath+".Bichsel.method", 0) );
		psrMethod = config.getString( nodePath+".ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
		centerSpecies = config.getXString( nodePath+".ZRecentering.centerSpecies", "K" );
		
		
		//Make the momentum transverse, eta, charge binning 
		binsPt = new HistoBins( config, "binning.pt" );


		INFO( classname(), "Image Name Modifier: " << imgNameMod );
		zdReporter = unique_ptr<Reporter>(new Reporter( config.getXString( nodePath + ".output:path" ) + imgNameMod + "_TPC_TSF_zd.pdf",
			config.getInt( nodePath + ".Reporter.output:width", 400 ), config.getInt( nodePath + ".Reporter.output:height", 400 ) ) );

		for ( string plc : Common::species ){
			string pre = "zd";
			if ( config.exists( nodePath + ".ParameterFixing." + pre + "." + plc ) ){
				INFOC(  "Creating Sigma Fixing range for " << pre << "_" << plc );
				XmlRange cr( &config, nodePath + ".ParameterFixing." + pre + "." + plc );
				sigmaRanges[ pre + "_" + plc ] = cr;
				INFOC( cr.toString() )
			}
		}// plc


		rnd = unique_ptr<TRandom3>( new TRandom3() );
		rnd->SetSeed( 0 );


		centralityFitBins.push_back( config.getInt( "cen", 0 )  );
		chargeFit.push_back( config.getInt( "charge", 1 ) );
	}

	TpcFitRunner::~TpcFitRunner(){
	} 


	void TpcFitRunner::makeHistograms(){

		book->cd();
		book->makeAll( nodePath + ".histograms" );

		for ( int iCen : centralityFitBins ){
			for ( int iCharge : chargeFit ){
				for ( string plc : Common::species ){
					// yield Histos
					book->cd( plc + "_yield" );
					string name = Common::yieldName( plc, iCen, iCharge );
					book->clone( "/", "yield", plc+"_yield", name );

					book->cd( "sys_sigma" );
					name = Common::yieldName( plc, iCen, iCharge );
					book->clone( "/", "yield", "sys_sigma", name );

					book->cd( "sys_tofEff" );
					name = Common::yieldName( plc, iCen, iCharge );
					book->clone( "/", "yield", "sys_tofEff", name );

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


					book->cd( "tofEff_dist" );
					book->clone( "/", "eff_dist", "tofEff_dist", Common::yieldName( plc, iCen, iCharge ) );
					for ( string plc2 : Common::species ){
						book->clone( "/", "sys_dist", "tofEff_dist", "sys_" + Common::yieldName( plc, iCen, iCharge, plc2 ) );
					}

					book->cd( "zd_sigma_dist" );
					book->clone( "/", "sigma_dist", "zd_sigma_dist", Common::yieldName( plc, iCen, iCharge ) );
					for ( string plc2 : Common::species ){
						book->clone( "/", "sys_dist", "zd_sigma_dist", "sys_" + Common::yieldName( plc, iCen, iCharge, plc2 ) );
					}

					book->cd( "zb_sigma_dist" );
					book->clone( "/", "sigma_dist", "zb_sigma_dist", Common::yieldName( plc, iCen, iCharge ) );
					for ( string plc2 : Common::species ){
						book->clone( "/", "sys_dist", "zb_sigma_dist", "sys_" + Common::yieldName( plc, iCen, iCharge, plc2 ) );
					}



				} // loop plc
			} // loop iCharge
		} // loop iCen
	} // makeHistograms()

	void TpcFitRunner::prepare( double avgP, int iCen ){
		INFOC( "( avgP=" << avgP << ", iCen=" << iCen << ")" )

		//Constraints on the mu 	 
		double zbDeltaMu = config.getDouble( nodePath + ".ParameterFixing.deltaMu:zb", 1.5 );
		double zdDeltaMu = config.getDouble( nodePath + ".ParameterFixing.deltaMu:zd", 1.5 );

		// fit roi
		

		activePlayers.clear();
		
		for ( string plc : Common::species ){

			INFOC(  "Setting up " << plc )
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

			// default low pt settings for zd
			// if ( !sigmaRanges[ "zd_" + plc ].above( avgP ) )
			schema->setInitialMu( "zd_mu_"+plc, zdMu, zdSig, 100.0 );
			schema->setInitialMuLimits( "zd_mu_"+plc, zdMu - 5*zdSig,zdMu + 100*zdSig );

			schema->setInitialSigma( "zd_sigma_"+plc, zdSig, 0.04, 0.24);

			if ( sigmaRanges[ "zd_" + plc ].above( avgP ) ){	
				
				double hm = sigmaSets[ "zd_" + plc ].mean();
				if ( 0 >= hm  )	// for running not in order so the sigma set isn't filled
					hm = zdSig;
				schema->setInitialSigma( "zd_sigma_"+plc, hm, hm - 0.002, hm + 0.002  );
				// schema->setInitialMu( "zd_mu_"+plc, zdMu, hm, zbDeltaMu );
			}
				
				
			// choose the active players
			choosePlayers( avgP, plc );

		} // loop on plc to set initial vals
	} // perpare(...)

	void TpcFitRunner::choosePlayers( double avgP, string plc ){

		activePlayers.push_back( "zd_All_g" + plc );
		schema->var( "yield_" + plc )->exclude = false;

		schema->var( "zd_sigma_" + plc )->exclude 	= false;
		schema->var( "zd_mu_" + plc )->exclude 	= false;


		double roi = config.getDouble( nodePath + ".FitSchema:roi", -1 );


		if ( roi > 0 ){
			double zbSig = zbSigma( );
			double zbMu = zbMean( plc, avgP );
			double zdMu = zdMean( plc, avgP );
			double zdSig = zdSigma(  );

			schema->addRange( "zd_All", zdMu - zdSig * roi, zdMu + zdSig * roi, "zd_mu_" + plc, "zd_sigma_" + plc, roi );
		}


	} // choosePlayers(...)


	void TpcFitRunner::respondToStats(double avgP){

	} // respondToStats(...)

	void TpcFitRunner::runNominal( int iCharge, int iCen, int iPt, shared_ptr<FitSchema> _schema ) {
		WARNC(  "(iCharge=" << iCharge << ", iCen=" << iCen << ", iPt=" << iPt << ")" );

		double avgP = binAverageP( iPt );
		auto zbMu = psr->centeredTofMap( centerSpecies, avgP );
		auto zdMu = psr->centeredDedxMap( centerSpecies, avgP );

		schema->clearRanges();

		// create the fitter
		shared_ptr<FitSchema> mySchema = schema;
		if ( nullptr != _schema )
			mySchema = _schema;
		
		Fitter fitter( mySchema, inFile );

		// loads the default values used for data projection
		fitter.registerDefaults( &config, nodePath );
		
		// prepare initial values, ranges, etc. for fit
		prepare( avgP, iCen );

		pair<float, float> zd_MinMax = dataRange( "zd", iPt );
		fitter.setZdProjectionMinMax( zd_MinMax.first, zd_MinMax.second );

		// load the datasets from the file
		fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt );

		// build the minuit interface
		fitter.setupFit();

		// assign active players to this fit
		fitter.addPlayers( activePlayers );
		
		INFOC( "====ACTIVE PLAYERS======" );
		INFOC( vts( activePlayers ) );
		INFOC( "====ACTIVE PLAYERS======" );
		
		fitter.nop();



		for ( int i = 0; i < 3; i ++){
			// gets close on yield with fixed shapes
			fitter.fit1(  );
			// gets close on shapes with fixed yields
			fitter.fit2(  );
		}

		int tries = 0;
		bool goodFit = false;
		while( goodFit == false && tries < 3 ){
			goodFit = fitter.isFitGood();
			fitter.fit4( );
			tries ++;
		}
		
		// if ( enhanced ){
		// 	for ( int i = 0; i < 3; i ++){
		// 		fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt, enhanced, zbMu, zdMu );
		// 		// gets close on yield with fixed shapes
		// 		fitter.fit1(  );
		// 		// gets close on shapes with fixed yields
		// 		fitter.fit2(  );
		// 	}
		// }
		
		// int tries = 0;
		// while( fitter.isFitGood() == false && tries < 3 ){
		// 	fitter.fit3( );
		// 	tries ++;
		// }

		if ( improveError )
			fitter.fitErrors();
		

		if ( nullptr == _schema ){


			reportFit( &fitter, iPt );
		
			// fill histograms if we converged
			if ( fitter.isFitGood())
				fillFitHistograms(iPt, iCen, iCharge, fitter );



			for ( string plc : Common::species ){



				// Keep track of the sigma for each species for fixing at high pt
				for ( string pre : {"zb", "zd"} ){
					XmlRange &range = sigmaRanges[ pre + "_" + plc ];
					// if we are in the good p range then add this value to the set
					if ( range.inInclusiveRange( avgP ) )
						sigmaSets[ pre+"_"+plc ].add( schema->var( pre + "_sigma_" + plc )->val ); 	
				} // plc
			} // pre
		}
	} // runNominal(...)


	void TpcFitRunner::make(){

		if ( inFile == nullptr || inFile->IsOpen() == false ){
			ERROR( "Invalid input data file - can't make" )
			return;
		}


		// Make the histograms for storing the results
		makeHistograms();

		int firstPtBin = config.getInt( nodePath + ".FitRange.ptBins:min", 0 );
		int lastPtBin = config.getInt( nodePath + ".FitRange.ptBins:max", 1 );

		if ( lastPtBin >= binsPt->nBins() )
			lastPtBin = binsPt->nBins() - 1;

		 for ( int iCen : centralityFitBins ){
			for ( int iCharge : chargeFit ){
							
				//Create the schema and fitter 
				schema = shared_ptr<FitSchema>(new FitSchema( &config, nodePath + ".FitSchema" ));

				sigmaSets.clear();
				for ( int iPt = firstPtBin; iPt <= lastPtBin; iPt++ ){
					INFOC( "####################### pT bin = " << iPt << " / " << lastPtBin << " #########################" );

					runNominal( iCharge, iCen, iPt );

					reportYields();

				}// loop iPt
			} // loop iCharge
		} // loop iCen
	} // make()


	pair<float, float> TpcFitRunner::dataRange( string v, int iPt ){
		double avgP = binAverageP( iPt );
		auto zbMu = psr->centeredTofMap( centerSpecies, avgP );
		auto zdMu = psr->centeredDedxMap( centerSpecies, avgP );

		double zbMin = 100;
		double zbMax = -100;
		for ( auto k : zbMu ){
			if ( k.first == "D" || k.first == "E" )
				continue;
			if ( k.second < zbMin )
				zbMin = k.second;
			if ( k.second > zbMax )
				zbMax = k.second;
		}

		double zdMin = 100;
		double zdMax = -100;
		for ( auto k : zdMu ){
			if ( k.first == "D" || k.first == "E" )
				continue;
			if ( k.second < zdMin )
				zdMin = k.second;
			if ( k.second > zdMax )
				zdMax = k.second;
		}

		double xMin = 0, xMax = 0;
		if ( v.substr(0, 2) == "zb" ){
			INFOC( "Using zb range ( " << zbMin << " -> " << zbMax << " )" );
			xMin = zbMin - 0.2;
			xMax = zbMax + 0.2;
		} else {
			INFOC( "Using zd range( " << zdMin << " -> " << zdMax << " )" );
			xMin = zdMin - 0.8;
			xMax = zdMax + 0.8;
		}

		return make_pair( xMin, xMax );
	}

	void TpcFitRunner::drawSet( string v, Fitter * fitter, int iPt ){
		INFOC(   v << ", fitter=" << fitter << ", iPt=" << iPt );

		double avgP = binAverageP( iPt );
		auto zbMu = psr->centeredTofMap( centerSpecies, avgP );
		auto zdMu = psr->centeredDedxMap( centerSpecies, avgP );

		double zbMin = 100;
		double zbMax = -100;
		for ( auto k : zbMu ){
			if ( k.first == "D" || k.first == "E" )
				continue;
			if ( k.second < zbMin )
				zbMin = k.second;
			if ( k.second > zbMax )
				zbMax = k.second;
		}

		double zdMin = 100;
		double zdMax = -100;
		for ( auto k : zdMu ){
			if ( k.first == "D" || k.first == "E" )
				continue;
			if ( k.second < zdMin )
				zdMin = k.second;
			if ( k.second > zdMax )
				zdMax = k.second;
		}


		TH1 * h = fitter->getDataHist( v );
		if ( !h ){
			WARNC(  "Data histogram not found" );
			return ;
		}
		h->Draw("pe");
		h->SetLineColor( kBlack );
		double scaler = 1e-8;

		int binmax = h->GetMaximumBin();
		double max = h->GetBinContent( binmax ) * 2;
		h->GetYaxis()->SetRangeUser( 0.1 / fitter->getNorm(), max );

		double xMin = 0, xMax = 0;
		if ( v.substr(0, 2) == "zb" ){
			INFOC(  "Using zb range ( " << zbMin << " -> " << zbMax << " )" );
			xMin = zbMin - 0.2;
			xMax = zbMax + 0.2;

			// TODO : fix hard code center species
			h->SetTitle( (" ; z_{b}^{" + Common::plc_label( centerSpecies ) + "} ; dN/dz_{b}").c_str() );
		} else {
			INFOC(  "Using zd range( " << zdMin << " -> " << zdMax << " )" );
			xMin = zdMin - 0.8;
			xMax = zdMax + 0.8;

			h->SetTitle( (" ; z_{d}^{" + Common::plc_label( centerSpecies ) + "}; dN/dz_{d}").c_str() );
		}

		h->GetXaxis()->SetRangeUser( xMin, xMax );
		h->GetXaxis()->SetNdivisions( 505 );
		h->GetYaxis()->SetTitleOffset( 1.4 );

		h->SetTitle( ( setTitle( v ) + "[" + ts(iPt) + "]" + " : " +  dts((*binsPt)[ iPt ]) + " < p_{T} [GeV/c] < " + dts( (*binsPt)[ iPt + 1 ] ) ).c_str() );



		// draw boxes to show the fit ranges
		for ( FitRange range : fitter->getSchema()->getRanges() ){
			if ( range.dataset != v )
				continue;
			TBox * b1 = new TBox( range.min, 0.1 / fitter->getNorm(), range.max, max  );
			b1->SetFillColorAlpha( kBlack, 0.25 );
			b1->SetFillStyle( 1001 );
			b1->Draw(  );
		}

		h->Draw("pe same");

		TGraph * sum = fitter->plotResult( v );
		sum->SetLineColor( kBlack );
		sum->SetLineWidth( 2 );
		sum->Draw( "same" );
		
		vector<TGraph*> comps;
		vector<double> colors = { kRed + 1, 	kAzure - 3, 	kGreen + 1, kBlue - 3 };
		int i = 0;
		
		for ( string plc : Common::species ){
			TGraph * g = fitter->plotResult( v+"_g"+plc );
			comps.push_back( g );
			g->SetLineColor( colors[ i ] );
			g->SetLineWidth( 2 );
			g->Draw( "same" );

			i++;
		}

		gPad->SetGrid( 1, 1 );
		gPad->SetLogy(1);
	} // drawSet(...)

	void TpcFitRunner::reportFit( Fitter * fitter, int iPt ){

		bool export_images = true;

		TCanvas* imgCanvas = new TCanvas( "imgCanvas", "imgCanvas", 400, 400 );

		gStyle->SetOptStat(0);
		bool drawBig = true;
		bool drawFitRatios=false;

		if ( drawBig ){
			INFOC(  "Reporting zd" );
			zdReporter->newPage( 1, 2 );
			{
				zdReporter->cd( 1, 1 );
				drawSet( "zd_All", fitter, iPt );
			}
		
			INFOC(  "Drawing fit vs. data ratios for zd" )
			{
				zdReporter->cd( 1, 2 );
				drawFitRatio( "zd_All", fitter, iPt );
			}
			zdReporter->savePage();
		}

		string imgPartA = config.getXString( nodePath + ".output:path" ) + "img/" + imgNameMod + "_";
		string imgPartB =  "_" + ts(iPt) + ".png";
		

		INFOC(  "Reporting zd" );
		// zdReporter->newPage( 2, 2 );
		// {

		// 	int pageXY = 1;
		// 	for ( string set : { "zd_All", "zd_Pi", "zd_K", "zd_P" } ){
					
		// 		if ( export_images ){
		// 			imgCanvas->cd();
		// 			drawSet( set, fitter, iPt );
		// 			string imgName = imgPartA + set + imgPartB;
		// 			gPad->Print( imgName.c_str() );
		// 		} 
		// 		zdReporter->cd( pageXY );
		// 		drawSet( set, fitter, iPt );
				
		// 		pageXY++;
		// 	}
		// } // reporting zd
		// zdReporter->savePage();

		if ( drawFitRatios ){
			INFOC(  "Drawing fit vs. data ratios for zd" )
			zdReporter->newPage( );
			{
				// zdReporter->cd( 1, 1 );
				drawFitRatio( "zd_All", fitter, iPt );
				// zdReporter->cd( 2, 1 );
				// drawFitRatio( "zd_Pi", fitter, iPt );
				// zdReporter->cd( 1, 2 );
				// drawFitRatio( "zd_K", fitter, iPt );
				// zdReporter->cd( 2, 2 );
				// drawFitRatio( "zd_P", fitter, iPt );
			}
			zdReporter->savePage();
		}


		delete imgCanvas;
	} // reportFit(...)

	void TpcFitRunner::reportYields(){

		double total = 0;
		for ( string plc  : Common::species ){
			if ( !schema->exists("yield_" + plc ) )
				continue;
			
			INFOC(  "Yield of " << plc << " = " << schema->var( "yield_" + plc )->val );
			total += schema->var( "yield_" + plc )->val;
		}

		INFOC(  "Total Fit Yield  = " <<  total );
		double yzb = schema->datasets[ "zb_All" ].yield();
		double yzd = schema->datasets[ "zd_All" ].yield();
		INFOC(  "Total Yield in zb_All = " << yzb );
		INFOC(  "Total Yield in zd_All = " << yzd );

		INFOC(  "Total Fit Yield / Total Data Yield (zd) = " << total / yzd );
		INFOC(  "Total Fit Yield / Total Data Yield (zb) = " << total / yzb );

		double roiyzb = schema->datasets[ "zb_All" ].yield( schema->getRanges() );
		double roiyzd = schema->datasets[ "zd_All" ].yield( schema->getRanges() );
		INFOC(  "zb_All / zd_All in roi = " << roiyzb / roiyzd );
	} // reportYields()

	void TpcFitRunner::fillFitHistograms(int iPt, int iCen, int iCharge, Fitter &fitter ){

		double avgP = binAverageP( iPt );

		for ( string plc : Common::species ){

			double zbMu = zbMean( plc, avgP );
			double zdMu = zdMean( plc, avgP );

			INFOC(  "Filling Histograms for " << plc );
			int iiPt = iPt + 1;

			// Yield			
			INFOC(  "Filling Yield for " << plc );
			string name = Common::yieldName( plc, iCen, iCharge );
			book->cd( plc+"_yield");
			double mNorm = fitter.getNorm();
			if ( config.getBool( nodePath + ".Normalize", true ) )
				mNorm = 1.0;
			double sC = schema->var( "yield_"+plc )->val * mNorm;
			double sE = schema->var( "yield_"+plc )->error * mNorm;
			
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

			INFOC(  "Filling Mus for " << plc );
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

			INFOC(  "Filling Sigmas for " << plc );
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
	} // fillFitHistograms(...)

	void TpcFitRunner::fillEnhancedYieldHistogram( string plc1, int iPt, int iCen, int iCharge, string plc2, Fitter &fitter ){
		
		for ( string pre : { "zb_", "zd_" } ){
			book->cd( pre + "enhanced");
			
			string vName = pre + plc1 + "_yield_" + plc2;
			string hName = Common::yieldName( plc1, iCen, iCharge, plc2 );

			double sC = schema->var( vName )->val;
			double sE = schema->var( vName )->error;
			book->setBin( hName, iPt, sC, sE );
		}
	} // fillEnhancedYieldHistogram(...)

	void TpcFitRunner::fillSystematicHistogram( string type, string plc, int iPt, int iCen, int iCharge, double sys ){
		
		double yNominal = schema->var( "yield_" + plc )->val;

		book->cd( "sys_" + type );
		string hName = Common::yieldName( plc, iCen, iCharge );
		book->setBin( hName, iPt, sys / yNominal, 0 );	
	} // fillEnhancedYieldHistogram(...)

	void TpcFitRunner::drawFitRatio( string ds, Fitter * fitter, int iPt ){

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
			
			double ratio = (yTh - dp.y) / yTh;


			int bin = h->GetXaxis()->FindBin( dp.x );
			DEBUGC( "x, y = " << dp.x << ", " << dp.y )
			DEBUGC( "yTh = " << yTh )
			DEBUGC( "ratio = " << ratio )
			
			double scale = schema->var( "yield_K" )->val;
			scale = 1.0;
			double r = 0.1;
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
	} // drawFitRatio(...)

	shared_ptr<FitSchema> TpcFitRunner::prepareSystematic( string sys, string plc, double delta ){
		INFOC(  "(sys=" << sys << ", plc=" << plc << ", delta=" << delta << ")" );

		shared_ptr <FitSchema> rSchema = shared_ptr<FitSchema>( new FitSchema( *schema ) );

		string var = sys + "_" + plc;
		// for instance zb_sigma
		// we want to fix sigma zb to given value and repeat the fit

		INFOC(  var << " (was) = " << schema->var( var )->val );

		rSchema->var( var )->val += delta;

		INFOC(  var << " (now) = " << rSchema->var( var )->val );

		return rSchema;
	} // prepareSystematic(...)

	void TpcFitRunner::runSystematic( shared_ptr<FitSchema> tmpSchema, int iCharge, int iCen, int iPt ){
		WARNC(  "(schema=" << tmpSchema << "iCharge=" << iCharge << ", iCen=" << iCen << ", iPt=" << iPt << ")" );

	
		double avgP = binAverageP( iPt );
		auto zbMu = psr->centeredTofMap( centerSpecies, avgP );
		auto zdMu = psr->centeredDedxMap( centerSpecies, avgP );

		schema->clearRanges();

		// create the fitter
		Fitter fitter( tmpSchema, inFile );

		// loads the default values used for data projection
		fitter.registerDefaults( &config, nodePath );
		
		// load the datasets from the file
		fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt, true, zbMu, zdMu );

		// build the minuit interface
		fitter.setupFit();
		// assign active players to this fit
		fitter.addPlayers( activePlayers );

		// reload the datasets from the file
		// now that we have better idea of mu, sigma ( for enhancement cuts )
		if ( avgP < 1.0 )
			fitter.loadDatasets(centerSpecies, iCharge, iCen, iPt, true, zbMu, zdMu );

		int tries = 0;
		while( fitter.isFitGood() == false && tries < 3 ){
			fitter.fit3( );
			tries ++;
		}

	} // runSystematic(...)
}


