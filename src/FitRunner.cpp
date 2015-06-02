
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

		/**
		 * Make the momentum transverse, eta, charge binning
		 */
		binsPt = new HistoBins( cfg, "bin.pt" );
		binsEta = new HistoBins( cfg, "bin.eta" );
		binsCharge = new HistoBins( cfg, "bin.charge" );

		logger->setClassSpace( "FitRunner" );

		zbReporter = unique_ptr<Reporter>(new Reporter( cfg->getString( nodePath + "output:path" ) + "rpTSF_zb.pdf",
			cfg->getInt( nodePath + "Reporter.output:width", 400 ), cfg->getInt( nodePath + "Reporter.output:height", 400 ) ) );
		zdReporter = unique_ptr<Reporter>(new Reporter( cfg->getString( nodePath + "output:path" ) + "rpTSF_zd.pdf",
			cfg->getInt( nodePath + "Reporter.output:width", 400 ), cfg->getInt( nodePath + "Reporter.output:height", 400 ) ) );

		/**
		 * Setup the PID Params
		 */
		if ( cfg->exists( nodePath +  "PidParameters:url" ) ){
			string fnPidParams = cfg->getString( nodePath + "PidParameters:url" );
			useParams = true;
			
			logger->info(__FUNCTION__) << "Loading PID parameters from : " << fnPidParams << endl;
			paramsConfig = shared_ptr<XmlConfig>( new XmlConfig( fnPidParams ) );
			
			logger->info(__FUNCTION__) << "Making zbParams" << endl;
			zbParams = unique_ptr<ZbPidParameters>( new ZbPidParameters( paramsConfig.get(), "TofPidParams", psr ) );
			zdParams = unique_ptr<ZdPidParameters>( new ZdPidParameters( paramsConfig.get(), "DedxPidParams" ) );	
		}

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
						book->cd( plc + "_zdSigma" );
						name = sigmaName( plc, iCen, iCharge, iEta );
						book->clone( "/", "yield", plc+"_zdSigma", name );

					} // loop plc
				} // loop iEta
			} // loop iCharge
		} // loop iCen


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

						double avgP = averageP( iPt, iEta );
						
						logger->warn(__FUNCTION__) << "<p> = " << avgP << endl;
						logger->info(__FUNCTION__) << "pt Bin : " << iPt << endl;
						logger->info(__FUNCTION__) << "Cen Bin : " << iCen << endl;
						logger->info(__FUNCTION__) << "Charge Bin : " << iCharge << endl;
						logger->info(__FUNCTION__) << "Eta Bin : " << iEta << endl;

						schema->clearRanges();

						prepare( avgP, iCen );

						//if ( avgP > 1.4 ){
						//	schema->setMethod( "nll" );
						//}
						//
						

						Fitter fitter( schema, inFile );
						fitter.addPlayers( activePlayers );
						//fitter.fixedFit( centerSpecies, iCharge, iCen, iPt, iEta );
						fitter.fit( centerSpecies, iCharge, iCen, iPt, iEta );

						// fill info about fit convergence
						//book->cd( "fit" );
						// book->make1D( 	fitName(iPt, iCen, iCharge, iEta ),
						// 				fitName(iPt, iCen, iCharge, iEta ),
						// 				1000, 0, 999.5 );
						// int bin = 1;
						// for ( double c : Fitter::convergence ){
						// 	if ( bin > 1000 )
						// 		break;
						// 	book->get( fitName(iPt, iCen, iCharge, iEta ) )->SetBinContent( bin, c );
						// 	bin ++;
						// }


						reportFit( &fitter, iPt );

						if ( fitter.isFitGood() )
							fillFitHistograms(iPt, iCen, iCharge, iEta, fitter.getNorm() / 100.0 );

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
			double zbMu = zbMean( plc, avgP, iCen );
			double zbSig = zbSigma( plc, avgP, iCen );

			// zd Parameters
			double zdMu = zdMean( plc, avgP );
			double zdSig = zdSigma( plc, avgP, iCen );

			// update the schema
			logger->trace(__FUNCTION__) << plc << " : zb mu=" << zbMu << ", sig=" << zbSig << endl;
			logger->trace(__FUNCTION__) << plc << " : zd mu=" << zdMu << ", sig=" << zdSig << endl; 
			
			// check if the sigmas should be fixed
			double zbMinParP = cfg->getDouble( nodePath + "ParameterFixing." + plc + ":zbSigma", 5.0 );
			double zdMinParP = cfg->getDouble( nodePath + "ParameterFixing." + plc + ":zdSigma", 5.0 );


			double zdSigFix = schema->vars[ "zd_sigma_"+plc ]->val;
			double zbSigFix = schema->vars[ "zb_sigma_"+plc ]->val;
			/*if ( iCen == 0 ){
				if ( "Pi" == plc )
					zbSigFix = 0.0115;
				if ( "K" == plc )
					zbSigFix = 0.012;
				if ( "P" == plc )
					zbSigFix = 0.0125;
			} else {
				if ( "Pi" == plc )
					zbSigFix = 0.0115;
				if ( "K" == plc )
					zbSigFix = 0.012;
				if ( "P" == plc )
					zbSigFix = 0.0125;
			}*/
			if ( zbMinParP > 0 && avgP >= zbMinParP)
				schema->fixParameter( "zb_sigma_" + plc, zbSigFix, true );
			else 
				schema->setInitialSigma( "zb_sigma_"+plc, zbSig, 0.005, 0.066);//0.006, 0.025 );


			if ( true /*== paramFixing && iPt != 0*/ ){ // if 1st bin let schema settings stick
				
				schema->setInitialMu( "zb_mu_"+plc, zbMu, zbSig, zbDeltaMu );
				schema->setInitialMu( "zd_mu_"+plc, zdMu, zdSig, zdDeltaMu );

				//if ( avgP < 0. ){
					
				/*if ( zdMinParP > 0 && avgP >= zdMinParP)
					schema->fixParameter( "zd_sigma_" + plc, zdSigFix, true );
				else */
					schema->setInitialSigma( "zd_sigma_"+plc, zdSig, 0.06, 0.08);//0.04, 0.16 );	
				//}
				
			}



				/*
			if ( false == paramFixing ){
				schema->setInitialMu( "zb_mu_"+plc, zbMu, zbSig, zbDeltaMu );
				schema->setInitialMu( "zd_mu_"+plc, zdMu, zdSig, zdDeltaMu );

				schema->setInitialSigma( "zb_sigma_"+plc, zbSig, tofSigmaIdeal * .25, tofSigmaIdeal * 4 );
				schema->setInitialSigma( "zd_sigma_"+plc, zdSig,dedxSigmaIdeal * .25 , dedxSigmaIdeal * 4 );
			}

			//if ( true == paramFixing  ){
				
				
				if ( zdMinParP > 0 && avgP >= zdMinParP )
					schema->fixParameter( "zd_sigma_" + plc, zdSig );
				
			//}*/
		
			if ( roi > 0 ){
				double roiTof = roi * 10;
				schema->addRange( "zb_All", zbMu - zbSig * roiTof, zbMu + zbSig * roiTof );
				schema->addRange( "zb_Pi", zbMu - zbSig * roiTof, zbMu + zbSig * roiTof );
				schema->addRange( "zb_K", zbMu - zbSig * roiTof, zbMu + zbSig * roiTof );
				schema->addRange( "zb_P", zbMu - zbSig * roiTof, zbMu + zbSig * roiTof );	

				schema->addRange( "zd_All", zdMu - zdSig * roi, zdMu + zdSig * roi );
				schema->addRange( "zd_Pi", zdMu - zdSig * roi, zdMu + zdSig * roi );
				schema->addRange( "zd_K", zdMu - zdSig * roi, zdMu + zdSig * roi );
				schema->addRange( "zd_P", zdMu - zdSig * roi, zdMu + zdSig * roi );	
			}

			// decide which models to include
			
			for ( auto plc2 : PidPhaseSpace::species ){
				
				if ( plc2 != "P" || avgP > 0.8  ){
					activePlayers.push_back( "zb_" + plc2 + "_g" + plc2 );
					schema->vars[ "zb_P_yield_P" ]->exclude = false;
				} else {
					schema->vars[ "zb_P_yield_P" ]->exclude = true;
				}
				activePlayers.push_back( "zd_" + plc2 + "_g" + plc2 );
				if ( plc == plc2 ) continue;

				double zbMu2 = zbMean( plc2, avgP, iCen );
				double zbNd = ( zbMu - zbMu2 ) / zbSig;

				if ( abs( zbNd ) < 3.0 ){
					activePlayers.push_back( "zd_" + plc + "_g" + plc2 );
					schema->vars[ "zd_"+plc+"_yield_"+plc2 ]->exclude = false;
				} else {
					schema->vars[ "zd_"+plc+"_yield_"+plc2 ]->exclude = true;
				}

				double zdMu2 = zdMean( plc2, avgP );
				double zdNd = ( zdMu - zdMu2 ) / zdSig;

				if ( abs( zdNd ) < 3.0 ){
					activePlayers.push_back( "zb_" + plc + "_g" + plc2 );
					schema->vars[ "zb_"+plc+"_yield_"+plc2 ]->exclude = false;
				} else {
					schema->vars[ "zb_"+plc+"_yield_"+plc2 ]->exclude = true;;
				}



			}

			// always include the all stuff
			activePlayers.push_back( "zb_All_gPi" );
			activePlayers.push_back( "zb_All_gK" );
			activePlayers.push_back( "zd_All_gPi" );
			activePlayers.push_back( "zd_All_gK" );
			if ( avgP > 0.4 ){
				activePlayers.push_back( "zb_All_gP" );
				activePlayers.push_back( "zd_All_gP" );
			}

		} // loop on plc to set config
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

			double zbMu = zbMean( plc, avgP, iCen );
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



