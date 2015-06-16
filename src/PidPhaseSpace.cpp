#include "PidPhaseSpace.h"
#include "TLine.h"

vector<string> PidPhaseSpace::species = { "Pi", "K", "P" };

PidPhaseSpace::PidPhaseSpace( XmlConfig* config, string np, string fl, string jp ) : InclusiveSpectra( config, np, fl, jp ) {

	/**
	 * Phase space padding options
	 */
 	tofPadding = cfg->getDouble( "binning.padding:tof", .2 );
	dedxPadding = cfg->getDouble( "binning.padding:dedx", .25 );
	tofScalePadding = cfg->getDouble( "binning.padding.tofScale", .05 );
	dedxScalePadding = cfg->getDouble( "binning.padding.dedxScale", .05 );

	logger->info(__FUNCTION__) << "Tof Padding ( " << tofPadding << ", " << tofScalePadding << " ) " << endl;
	/**
	 * Initialize the Phase Space Recentering Object
	 */
	tofSigmaIdeal = cfg->getDouble( np+"PhaseSpaceRecentering.sigma:tof", 0.011);
	dedxSigmaIdeal = cfg->getDouble( np+"PhaseSpaceRecentering.sigma:dedx", 0.033);
	psr = new PhaseSpaceRecentering( dedxSigmaIdeal,
									 tofSigmaIdeal,
									 cfg->getString( np+"Bichsel.table", "dedxBichsel.root"),
									 cfg->getInt( np+"Bichsel.method", 0) );

		// method for phase space recentering
	psrMethod = config->getString( np+"PhaseSpaceRecentering.method", "traditional" );
		// alias the centered species for ease of use
	centerSpecies = cfg->getString( np+"PhaseSpaceRecentering.centerSpecies", "K" );
		// enhanced distro options
	tofCut = cfg->getDouble( np+"enhanceDistributions:tof", 1.0 );
	dedxCut = cfg->getDouble( np+"enhanceDistributions:dedx", 1.0 );

	/**
	 * Flags to turn on and off certain histos
	 */
	make2D = cfg->getBool( np + "makePhaseSpace:2D", false );
	makeEnhanced = cfg->getBool( np + "makePhaseSpace:enhanced", false );

	/**
	 * Make the dedx + tof binning 
	 * Only the bin width is used for dynamic bins
	 */
	dedxBinWidth = cfg->getDouble( "binning.dedx:width", 0.015 );
	tofBinWidth = cfg->getDouble( "binning.tof:width", 0.006 );

	/**
	 * Make the momentum transverse binning
	 */
	binsPt = unique_ptr<HistoBins>(new HistoBins( cfg, "bin.pt" ));

	/**
	 * Make the eta binning
	 */
	binsEta = unique_ptr<HistoBins>(new HistoBins( cfg, "bin.eta" ));

	/**
	 * Make charge bins
	 */
	charges = cfg->getIntVector( "bin.charges" );


	/**
	 * Cuts below Pion to remove electrons
	 * And above P to remove deuterons
	 */
	nSigBelow = cfg->getDouble( nodePath + "enhanceDistributions:nSigBelow", 3.0 );
	nSigAbove = cfg->getDouble( nodePath + "enhanceDistributions:nSigAbove", 3.0 );
	logger->info(__FUNCTION__) << "nSig cuts for e, d : " << nSigBelow << ", " << nSigAbove << endl;

	makeCombinedCharge = false;

 }

 PidPhaseSpace::~PidPhaseSpace(){

 }

void PidPhaseSpace::preEventLoop() {
	logger->info(__FUNCTION__) << endl;
	
	InclusiveSpectra::preEventLoop();

	book->cd();
	preparePhaseSpaceHistograms( centerSpecies );	
}

void PidPhaseSpace::postEventLoop() {
	logger->info(__FUNCTION__) << endl;

	if ( cfg->getBool( nodePath+"MakeQA:tof", false ) )
		reportAll( "" );
	if ( cfg->getBool( nodePath+"MakeQA:dedx", false ) )
		reportAll( "dedx" );

	book->cd();

}

void PidPhaseSpace::analyzeTrack( int iTrack ){

	book->cd();


	double pt 	= pico->trackPt( iTrack );
	double p 	= pico->trackP( iTrack );
	double eta 	= pico->trackEta( iTrack );

	int ptBin 	= binsPt->findBin( pt );
	int etaBin 	= binsEta->findBin( TMath::Abs( eta ) );
	int charge 	= pico->trackCharge( iTrack );

	double avgP = averageP( ptBin, etaBin );

	binByMomentum = true;
	// even if we use p binning we still want to cut on eta
	if ( true == binByMomentum  && etaBin >= 0 ){
		ptBin = binsPt->findBin( p );
		etaBin = 0;
		avgP = averagePt( ptBin );
	}

	if ( ptBin < 0 || etaBin < 0 || cBin < 0 )
		return;


	// fill the dN/dPt plots
	if ( cBin >= 0 ){
		string cName = "pt_" + ts( cBin );
		book->fill( cName, pt, eventWeight );		
	}


	double tof = psr->rTof(centerSpecies, pico->trackBeta(iTrack), p );
	double dedx = psr->rDedx(centerSpecies, pico->trackDedx(iTrack), p );

	double tofNL = psr->nlTof(centerSpecies, pico->trackBeta(iTrack), p, avgP );
	double dedxNL = psr->nlDedx(centerSpecies, pico->trackDedx(iTrack), p, avgP );

	
	
	book->fill( "betaRaw", p, 1.0/pico->trackBeta( iTrack ) );


	book->fill( "dedxRaw", p, pico->trackDedx( iTrack ) );
	book->fill( "eta", eta );

	book->fill( "trBeta", p, tof );
	book->fill( "trDedx", p, dedx );
	book->fill( "nlBeta", p, tofNL );
	book->fill( "nlDedx", p, dedxNL );

	if ( "nonlinear" == psrMethod ){
		tof = tofNL;
		dedx = dedxNL;
	} 

	if ( make2D ){
		book->cd( "dedx_tof" );

		// combined charge 
		if ( makeCombinedCharge ) book->fill( speciesName( centerSpecies, 0, cBin, ptBin, etaBin ), dedx, tof );
		book->fill( speciesName( centerSpecies, charge, cBin, ptBin, etaBin ), dedx, tof );
			
	}
	
	enhanceDistributions(avgP, ptBin, etaBin, charge, dedx, tof );

	book->cd();
}

void PidPhaseSpace::preparePhaseSpaceHistograms( string plc ){

	logger->info(__FUNCTION__) << "Making Histograms with centering spceies: " << plc << endl;

	book->cd();

	// Loop through pt, then eta then charge
	for ( int ptBin = 0; ptBin < binsPt->nBins(); ptBin++ ){

		double p = (binsPt->bins[ ptBin ] + binsPt->bins[ ptBin + 1 ]) / 2.0;

		double tofLow, tofHigh, dedxLow, dedxHigh;
		autoViewport( plc, p, psr, &tofLow, &tofHigh, &dedxLow, &dedxHigh, tofPadding, dedxPadding, tofScalePadding, dedxScalePadding );
		
		vector<double> tofBins = HistoBins::makeFixedWidthBins( tofBinWidth, tofLow, tofHigh );
		vector<double> dedxBins = HistoBins::makeFixedWidthBins( dedxBinWidth, dedxLow, dedxHigh );

		for ( int etaBin = 0; etaBin < binsEta->size()-1; etaBin++ ){
			// Loop over charge, skip if config doesnt include that charge bin
			for ( int charge : charges ){

				for ( int iCen = 0; iCen < nCentralityBins(); iCen++ ){

					// the name of the 2D histogram
					string hName = speciesName( plc, charge, iCen, ptBin, etaBin  );

					string title = "dE/dx Vs. #beta^{-1}; dE/dx; #beta^{-1}";

					// 2d dedx X tof 
					// 2D for NMF 
					if ( make2D ){
						book->cd( "dedx_tof" );
						book->make2D( hName, title, dedxBins.size()-1, dedxBins.data(), tofBins.size()-1, tofBins.data() );
					}

					// tof projections
					book->cd( "tof" );
					book->make1D( tofName( plc, charge, iCen, ptBin, etaBin ), 
						"#beta^{-1}", tofBins.size()-1, tofBins.data() );
					
					if ( makeEnhanced ){ 
						for ( int iS = 0; iS < species.size(); iS++ ){
							book->make1D( tofName( plc, charge, iCen, ptBin, etaBin, species[ iS ] ), 
								"#beta^{-1}", tofBins.size()-1, tofBins.data() );
						} 
					}

					// dedx projections
					book->cd( "dedx" );		
					book->make1D( dedxName( plc, charge, iCen, ptBin, etaBin ), 
									"dEdx", dedxBins.size()-1, dedxBins.data() );
					
					// Enhanced
					if ( makeEnhanced ){ 
						for ( int iS = 0; iS < species.size(); iS++ ){
							book->make1D( dedxName( plc, charge, iCen, ptBin, etaBin, species[ iS ] ), 
								"dEdx", dedxBins.size()-1, dedxBins.data() );
						}
					}

					

				 
				} // loop on centrality
			}// loop on charge
		}// loop on eta bins
	} // loop on ptBins
}

void PidPhaseSpace::autoViewport( 	string pType, double p, PhaseSpaceRecentering * lpsr, double * tofLow, double* tofHigh, double * dedxLow, double * dedxHigh, double tofPadding, double dedxPadding, double tofScaledPadding, double dedxScaledPadding  ){


	TofGenerator * tofGen = lpsr->tofGenerator();
	Bichsel * dedxGen = lpsr->dedxGenerator();

	double tofCenter = tofGen->mean( p, lpsr->mass( pType ) );
	double dedxCenter = dedxGen->meanLog( p, lpsr->mass( pType ), -1, 1000 );

	vector<double> tofMean;
	vector<double> dedxMean;

	for ( int i = 0; i < species.size(); i ++ ){
		tofMean.push_back(  tofGen->mean( p, lpsr->mass( species[ i ] ) ) );
		dedxMean.push_back( dedxGen->meanLog( p, lpsr->mass( species[ i ] ), -1, 1000 ) );
	}


	double tHigh = (tofMean[ 0 ] - tofCenter);
	double tLow = (tofMean[ 0 ] - tofCenter);
	double dHigh = (dedxMean[ 0 ] - dedxCenter);
	double dLow = (dedxMean[ 0 ] - dedxCenter);
	for ( int i = 0; i < species.size(); i++ ){
		if ( (tofMean[ i ] - tofCenter) > tHigh )
			tHigh = (tofMean[ i ] - tofCenter);
		if ( (tofMean[ i ] - tofCenter) < tLow )
			tLow = (tofMean[ i ] - tofCenter);

		if ( (dedxMean[ i ] - dedxCenter) > dHigh )
			dHigh = (dedxMean[ i ] - dedxCenter);
		if ( (dedxMean[ i ] - dedxCenter) < dLow )
			dLow = (dedxMean[ i ] - dedxCenter);
	}

	*tofHigh = tHigh + tofPadding;
	*tofLow = tLow - tofPadding;

	*dedxHigh = dHigh + dedxPadding;
	*dedxLow = dLow  - dedxPadding;

	double tofRange = *tofHigh - *tofLow;
	double dedxRange = *dedxHigh - *dedxLow;

	double scaledPaddingTof = tofRange * tofScaledPadding;
	double scaledPaddingDedx = dedxRange * dedxScaledPadding;

	*tofLow -= scaledPaddingTof;
	*tofHigh += scaledPaddingTof;

	*dedxLow -= scaledPaddingDedx;
	*dedxHigh += scaledPaddingDedx;

	return;
}



void PidPhaseSpace::enhanceDistributions( double avgP, int ptBin, int etaBin, int charge, double dedx, double tof ){
	
	double tSigma = tofSigmaIdeal;
	double dSigma = dedxSigmaIdeal;

	// get the centered tof and dedx means
	vector<double> tMeans = psr->centeredTofMeans( centerSpecies, avgP );
	vector<double> dMeans = psr->centeredDedxMeans( centerSpecies, avgP );
	vector<string> mSpecies = psr->allSpecies();

	

	book->cd( "tof" );
	// unenhanced - all tof tracks
	// combined charge 
	if ( makeCombinedCharge ) book->fill( tofName( centerSpecies, 0, cBin, ptBin, etaBin ), tof, eventWeight );
	book->fill( tofName( centerSpecies, charge, cBin, ptBin, etaBin ), tof, eventWeight );

	// enhanced by species
	if ( makeEnhanced ){
		for ( int iS = 0; iS < mSpecies.size(); iS++ ){
			if ( dedx >= dMeans[ iS ] - dSigma && dedx <= dMeans[ iS ] + dSigma ){
				if ( makeCombinedCharge ) book->fill( tofName( centerSpecies, 0, cBin, ptBin, etaBin, mSpecies[ iS ] ), tof, eventWeight );
				book->fill( tofName( centerSpecies, charge, cBin, ptBin, etaBin, mSpecies[ iS ] ), tof, eventWeight );
			}
		} // loop on species from centered means
	}

	
	// 3 sigma below pi to reject electrons
	// and 3 sigma above proton to reject deuteron
	if ( 	tof < tMeans[ 0 ] - tSigma * nSigBelow || tof > tMeans[ 2 ] + tSigma * nSigAbove )
		return;


	book->cd( "dedx" );
	// unenhanced - all dedx
	// combined charge 
	if ( makeCombinedCharge ) book->fill( dedxName( centerSpecies, 0, cBin, ptBin, etaBin ), dedx, eventWeight );
	book->fill( dedxName( centerSpecies, charge, cBin, ptBin, etaBin ), dedx, eventWeight );

	// enhanced by species
	if ( makeEnhanced ){
		for ( int iS = 0; iS < mSpecies.size(); iS++ ){
			
			double ttMean = tMeans[ iS ];

			if ( tof >= ttMean - tSigma && tof <= ttMean + tSigma ){
				if ( makeCombinedCharge ) book->fill( dedxName( centerSpecies, 0, cBin, ptBin, etaBin, mSpecies[ iS ] ), dedx, eventWeight );
				book->fill( dedxName( centerSpecies, charge, cBin, ptBin, etaBin, mSpecies[ iS ] ), dedx, eventWeight );
			}
		} // loop on species from centered means	
	}

	

}


void PidPhaseSpace::reportAll( string type ){



	logger->info( __FUNCTION__ ) << endl;
	
	double sigma = tofSigmaIdeal;
	if ( "dedx" == type )
		sigma = dedxSigmaIdeal;

	vector<int> charges = { -1, 0, 1 };

	book->cd();
	int nCenBins = nCentralityBins();
	int nChargeBins = charges.size();
	int nEtaBins = binsEta->nBins();
	map<string,  unique_ptr<Reporter> > rps;
	string baseURL = cfg->getString(  nodePath + "output:path", "./" );
	string baseName= cfg->getString(  nodePath + "output.Reports", "" );


	book->cd( "tof" );
	if ( "dedx" == type )
		book->cd( "dedx" );
	// Make the slew of reporters
	for ( int etaBin = 0; etaBin < nEtaBins; etaBin++ ){
		// Loop over charge, skip if config doesnt include that charge bin
		for ( int charge : charges ){
			
			for ( int iCen = 0; iCen < nCenBins; iCen++ ){
			
				string sn = tofName( centerSpecies, charge, iCen, 1, etaBin );
				if ( "dedx" == type )
					sn = dedxName( centerSpecies, charge, iCen, 1, etaBin );
				
				if ( !book->exists( sn ) ) continue;

				rps[ sn ] = unique_ptr<Reporter>(new Reporter( baseURL + jobPrefix + baseName + sn + ".pdf", 1000, 500 )) ;


			} // loop centralities
		} // loop charges 
	} // loop eta bins

	
	// Loop through pt, then eta then charge
	for ( int ptBin = 0; ptBin < binsPt->nBins(); ptBin++ ){
		for ( int etaBin = 0; etaBin < nEtaBins; etaBin++ ){
			// Loop over charge, skip if config doesnt include that charge bin
			for ( int charge : charges ){
				
				for ( int iCen = 0; iCen < nCenBins; iCen++ ){
				
					double avgP = averageP( ptBin, 0 );	

					vector<double> tMeans = psr->centeredTofMeans( centerSpecies, avgP );
					vector<double> dMeans = psr->centeredDedxMeans( centerSpecies, avgP );				
					vector<double> means = tMeans;
					if ( "dedx" == type )
						means = dMeans;


					string n = tofName( centerSpecies, charge, iCen, ptBin, etaBin );
					if ( "dedx" == type )
						n = dedxName( centerSpecies, charge, iCen, ptBin, etaBin );
					
					string sn = tofName( centerSpecies, charge, iCen, 1, etaBin );
					if ( "dedx" == type )
						sn = dedxName( centerSpecies, charge, iCen, 1, etaBin );
					if ( !book->exists( n ) ) continue;

					string ptLowEdge = dts(binsPt->getBins()[ ptBin ]);
					string ptHiEdge = dts(binsPt->getBins()[ ptBin + 1 ]);
					string etaLowEdge = dts(binsEta->getBins()[ etaBin ]);
					string etaHiEdge = dts(binsEta->getBins()[ etaBin + 1 ]);
					//string range = dts(binsPt->getBins()[ ptBin ]) + " < Pt < " + dts(binsPt->getBins()[ ptBin ]) + " : #eta = " << dts(binsEta->getBins()[ etaBin ])

					if ( !rps[sn] ){
						logger->error(__FUNCTION__) << "NO reporter for : " << sn << endl;
						continue;
					}
					rps[ sn ]->newPage();
					book->style( n )->set( "logY", 1 )->
					set( "title", ptLowEdge + " < Pt < " + ptHiEdge + " : " + etaLowEdge + " < |#eta| < " + etaHiEdge )->
					set( "x", "z_{1/#beta}" )->
					set( "y", "events / " + dts( tofBinWidth ) )->
					set( "draw", "pe" )->
					draw();

					map<string, int> colors;
					colors[ "Pi" ] = kBlue;
					colors[ "K" ] = kGreen;
					colors[ "P" ] = kBlack;
					int i = 0;
					for ( string plc : species ){
						
						double ttMean = means[ i ];
						TLine * l1 = new TLine( ttMean - sigma * tofCut, 0, ttMean - sigma* tofCut, 10 );
						TLine * l2 = new TLine( ttMean + sigma* tofCut, 0, ttMean + sigma* tofCut, 10 );
						l1->SetLineColor( colors[ plc ] );
						l2->SetLineColor( colors[ plc ] );

						l1->Draw("same");
						l2->Draw("same");
						i++;
					}
					if ( "dedx" != type  ){
						double ttMean = tMeans[ 0 ];
						logger->info(__FUNCTION__) << "PiMu = " << ttMean << ", sigma = " << sigma << ", nSigma = " << nSigBelow << endl; 
						logger->info(__FUNCTION__) << "ptBin = " << ptBin << " electron cut : " << ttMean - sigma * nSigBelow <<  endl;
						TLine * l1 = new TLine( ttMean - sigma * nSigBelow, 0, ttMean - sigma * nSigBelow, 100 );
						l1->SetLineStyle( 2 );
						l1->SetLineColor( kRed );
						l1->Draw( "same" );

						ttMean = tMeans[ 2 ];
						TLine * l2 = new TLine( ttMean + sigma * nSigAbove, 0, ttMean + sigma * nSigAbove, 100 );
						l2->SetLineStyle( 2 );
						l2->SetLineColor( kRed );
						l2->Draw("same" );
					}
					
					rps[ sn ]->savePage();
				
				} // loop centralities
			} // loop charges 
		} // loop eta bins
	} // loop pt bins

}

void PidPhaseSpace::reportAllTof() {

	logger->info( __FUNCTION__ ) << endl;
	double tSigma = tofSigmaIdeal;

	vector<int> charges = { -1, 0, 1 };

	book->cd();
	int nCenBins = nCentralityBins();
	int nChargeBins = charges.size();
	int nEtaBins = binsEta->nBins();
	map<string,  unique_ptr<Reporter> > rps;
	string baseURL = cfg->getString(  nodePath + "output:path", "./" );
	string baseName= cfg->getString(  nodePath + "output.Reports", "" );

	book->cd( "tof" );
	// Make the slew of reporters
	for ( int etaBin = 0; etaBin < nEtaBins; etaBin++ ){
		// Loop over charge, skip if config doesnt include that charge bin
		for ( int charge : charges ){
			
			for ( int iCen = 0; iCen < nCenBins; iCen++ ){
			
				string sn = tofName( centerSpecies, charge, iCen, 1, etaBin );
				
				if ( !book->exists( sn ) ) continue;

				rps[ sn ] = unique_ptr<Reporter>(new Reporter( baseURL + jobPrefix + baseName + sn + ".pdf", 1000, 500 )) ;


			} // loop centralities
		} // loop charges 
	} // loop eta bins

	
	// Loop through pt, then eta then charge
	for ( int ptBin = 0; ptBin < binsPt->nBins(); ptBin++ ){
		for ( int etaBin = 0; etaBin < nEtaBins; etaBin++ ){
			// Loop over charge, skip if config doesnt include that charge bin
			for ( int charge : charges ){
				
				for ( int iCen = 0; iCen < nCenBins; iCen++ ){
				
					double avgP = averageP( ptBin, 0 );	

					vector<double> tMeans = psr->centeredTofMeans( centerSpecies, avgP );
					vector<double> dMeans = psr->centeredDedxMeans( centerSpecies, avgP );				

					string n = tofName( centerSpecies, charge, iCen, ptBin, etaBin );
					string sn = tofName( centerSpecies, charge, iCen, 1, etaBin );
					if ( !book->exists( n ) ) continue;

					string ptLowEdge = dts(binsPt->getBins()[ ptBin ]);
					string ptHiEdge = dts(binsPt->getBins()[ ptBin + 1 ]);
					string etaLowEdge = dts(binsEta->getBins()[ etaBin ]);
					string etaHiEdge = dts(binsEta->getBins()[ etaBin + 1 ]);
					//string range = dts(binsPt->getBins()[ ptBin ]) + " < Pt < " + dts(binsPt->getBins()[ ptBin ]) + " : #eta = " << dts(binsEta->getBins()[ etaBin ])

					if ( !rps[sn] ){
						logger->error(__FUNCTION__) << "NO reporter for : " << sn << endl;
						continue;
					}
					rps[ sn ]->newPage();
					book->style( n )->set( "logY", 1 )->
					set( "title", ptLowEdge + " < Pt < " + ptHiEdge + " : " + etaLowEdge + " < |#eta| < " + etaHiEdge )->
					set( "x", "z_{1/#beta}" )->
					set( "y", "events / " + dts( tofBinWidth ) )->
					set( "draw", "pe" )->
					draw();

					map<string, int> colors;
					colors[ "Pi" ] = kBlue;
					colors[ "K" ] = kGreen;
					colors[ "P" ] = kBlack;
					int i = 0;
					for ( string plc : species ){
						
						double ttMean = tMeans[ i ];
						TLine * l1 = new TLine( ttMean - tSigma * tofCut, 0, ttMean - tSigma* tofCut, 10 );
						TLine * l2 = new TLine( ttMean + tSigma* tofCut, 0, ttMean + tSigma* tofCut, 10 );
						l1->SetLineColor( colors[ plc ] );
						l2->SetLineColor( colors[ plc ] );

						l1->Draw("same");
						l2->Draw("same");
						i++;
					}
					
					double ttMean = tMeans[ 0 ];
					logger->info(__FUNCTION__) << "PiMu = " << ttMean << ", sigma = " << tSigma << ", nSigma = " << nSigBelow << endl; 
					logger->info(__FUNCTION__) << "ptBin = " << ptBin << " electron cut : " << ttMean - tSigma * nSigBelow <<  endl;
					TLine * l1 = new TLine( ttMean - tSigma * nSigBelow, 0, ttMean - tSigma * nSigBelow, 100 );
					l1->SetLineStyle( 2 );
					l1->SetLineColor( kRed );
					l1->Draw( "same" );

					ttMean = tMeans[ 2 ];
					TLine * l2 = new TLine( ttMean + tSigma * nSigAbove, 0, ttMean + tSigma * nSigAbove, 100 );
					l2->SetLineStyle( 2 );
					l2->SetLineColor( kRed );
					l2->Draw("same" );
					
					rps[ sn ]->savePage();
				
				} // loop centralities
			} // loop charges 
		} // loop eta bins
	} // loop pt bins
}

void PidPhaseSpace::reportAllDedx() {
	logger->info( __FUNCTION__ ) << endl;
	book->cd();

	vector<int> charges = { -1, 1 };

	int nCenBins = nCentralityBins();
	int nChargeBins = charges.size();//binsCharge->nBins();
	int nEtaBins = binsEta->nBins();

	vector< unique_ptr<Reporter> > rps;
	
	string baseURL = cfg->getString(  nodePath + "output:path", "./" );
	string baseName= cfg->getString(  nodePath + "output.Reports", "" );
	
	// Make the slew of reporters
	for ( int etaBin = 0; etaBin < nEtaBins; etaBin++ ){
		// Loop over charge, skip if config doesnt include that charge bin
		for ( int charge : charges ){
			
			for ( int iCen = 0; iCen < nCenBins; iCen++ ){
			
				string sn = dedxName( centerSpecies, charge, iCen, 1, etaBin );	
				rps.push_back( unique_ptr<Reporter>(new Reporter( baseURL + jobPrefix + baseName + sn + ".pdf", 1000, 500 )) );


			} // loop centralities
		} // loop charges 
	} // loop eta bins

	book->cd( "dedx" );
	// Loop through pt, then eta then charge
	for ( int ptBin = 0; ptBin < binsPt->nBins(); ptBin++ ){
		for ( int etaBin = 0; etaBin < binsEta->size()-1; etaBin++ ){
			// Loop over charge, skip if config doesnt include that charge bin
			for ( int charge : charges ){
				
				for ( int iCen = 0; iCen < nCentralityBins(); iCen++ ){
				
					double avgP = averageP( ptBin, etaBin );

					vector<double> tMeans = psr->centeredTofMeans( centerSpecies, avgP );
					vector<double> dMeans = psr->centeredDedxMeans( centerSpecies, avgP );									

					int index = iCen + (charge + 1 ) * nCenBins + etaBin * nChargeBins * nCenBins;
					string n = dedxName( centerSpecies, charge, iCen, ptBin, etaBin );

					string ptLowEdge = dts(binsPt->getBins()[ ptBin ]);
					string ptHiEdge = dts(binsPt->getBins()[ ptBin + 1 ]);
					string etaLowEdge = dts(binsEta->getBins()[ etaBin ]);
					string etaHiEdge = dts(binsEta->getBins()[ etaBin + 1 ]);
					//string range = dts(binsPt->getBins()[ ptBin ]) + " < Pt < " + dts(binsPt->getBins()[ ptBin ]) + " : #eta = " << dts(binsEta->getBins()[ etaBin ])

					rps[ index ]->newPage();
					book->style( n )->set( "logY", 1 )->
					set( "title", ptLowEdge + " < Pt < " + ptHiEdge + " : " + etaLowEdge + " < |#eta| < " + etaHiEdge )->
					set( "x", "z_{dEdx}" )->
					set( "y", "events / " + dts( tofBinWidth ) )->
					set( "draw", "pe" )->
					draw();

					map<string, int> colors;
					colors[ "Pi" ] = kBlue;
					colors[ "K" ] = kGreen;
					colors[ "P" ] = kBlack;
					int i = 0;
					for ( string plc : species ){
						
						double ttSigma = dedxSigmaIdeal;
						double ttMean = dMeans[ i ];
						TLine * l1 = new TLine( ttMean - ttSigma * dedxCut, 0, ttMean - ttSigma * dedxCut, 10 );
						TLine * l2 = new TLine( ttMean + ttSigma * dedxCut, 0, ttMean + ttSigma * dedxCut, 10 );
						l1->SetLineColor( colors[ plc ] );
						l2->SetLineColor( colors[ plc ] );

						l1->Draw("same");
						l2->Draw("same");
						i++;
					}
					
					rps[ index ]->savePage();
				
				} // loop centralities
			} // loop charges 
		} // loop eta bins
	} // loop pt bins


}







