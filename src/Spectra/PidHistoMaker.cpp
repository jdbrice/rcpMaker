// RcpMaker
#include "Spectra/PidHistoMaker.h"
#include "Correction/SpectraCorrecter.h"

// ROOT
#include "TLine.h"


PidHistoMaker::PidHistoMaker( XmlConfig* config, string np, string fl, string jp ) : InclusiveSpectra( config, np, fl, jp ) {

	/**
	 * Phase space padding options
	 */
 	tofPadding 			= cfg->getDouble( "binning.padding:tof", .2 );
	dedxPadding 		= cfg->getDouble( "binning.padding:dedx", .25 );
	tofScalePadding 	= cfg->getDouble( "binning.padding.tofScale", .05 );
	dedxScalePadding 	= cfg->getDouble( "binning.padding.dedxScale", .05 );

	logger->info(__FUNCTION__) << "Tof Padding ( " << tofPadding << ", " << tofScalePadding << " ) " << endl;
	/**
	 * Initialize the Phase Space Recentering Object
	 */
	tofSigmaIdeal 	= cfg->getDouble( np+"ZRecentering.sigma:tof", 0.011);
	dedxSigmaIdeal 	= cfg->getDouble( np+"ZRecentering.sigma:dedx", 0.033);
	zr 				= new ZRecentering( dedxSigmaIdeal,
									 	tofSigmaIdeal,
									 	cfg->getString( np+"Bichsel.table", "dedxBichsel.root"),
									 	cfg->getInt( np+"Bichsel.method", 0) );

		// method for phase space recentering
	zrMethod 		= config->getString( np + "ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
	centerSpecies 	= cfg->getString( np + "ZRecentering.centerSpecies", "K" );
		// enhanced distro options
	tofCut 			= cfg->getDouble( np+"Distributions:tof", 1.0 );
	dedxCut 		= cfg->getDouble( np+"Distributions:dedx", 1.0 );

	// Flags to turn on and off certain histos
	make2D 			= cfg->getBool( np + "Distributions:2D", false );
	makeEnhanced 	= cfg->getBool( np + "Distributions:enhanced", false );

	// Make the dedx + tof binning 
	// Only the bin width is used for dynamic bins
	dedxBinWidth 	= cfg->getDouble( "binning.dedx:width", 0.015 );
	tofBinWidth 	= cfg->getDouble( "binning.tof:width", 0.006 );

	//Make the momentum transverse binning
	binsPt 	= unique_ptr<HistoBins>(new HistoBins( cfg, "binning.pt" ));

	// Get the list of charges we are looking at
	charges = cfg->getIntVector( "binning.charges" );


	// Cuts below Pion to remove electrons
	// And above P to remove deuterons
	nSigBelow = cfg->getDouble( nodePath + "Distributions:nSigBelow", 3.0 );
	nSigAbove = cfg->getDouble( nodePath + "Distributions:nSigAbove", 3.0 );
	logger->info(__FUNCTION__) << "nSig cuts for e : Pi - " << nSigBelow << " sigma" << endl;
	logger->info(__FUNCTION__) << "nSig cuts for d : P + " << nSigAbove << " sigma" << endl;

	// TODO: use charges vector to decide if we want to combine charges
	makeCombinedCharge = false;

	// Efficiency corrector
	sc = unique_ptr<SpectraCorrecter>( new SpectraCorrecter( cfg, nodePath ) ); 
	// Read in the options for corrections
	correctTpcEff = cfg->getBool( nodePath + "Corrections:tpc", false );
	correctTofEff = cfg->getBool( nodePath + "Corrections:tof", false );
	correctFeedDown = cfg->getBool( nodePath + "Corrections:fd", false );

	// make the energy loss params
	vector<int> charges = { -1, 1 };
	if ( cfg->exists( np + "EnergyLossParams:path" ) ){
		string path = cfg->getString( np + "EnergyLossParams:path" );

		for ( int c : charges ){

			string cfgName = path + centerSpecies + "_" + Common::chargeString( c ) + ".xml";
			XmlConfig cfgEL( cfgName );

			for ( int cb : cfg->getIntVector( nodePath + "CentralityBins" ) ){
				
				// Name like 'Pi_p_0' ... 'Pi_n_6' to be used for quick lookup
				string name = centerSpecies + "_" + Common::chargeString( c ) +"_" + ts( cb );
				elParams[ name ] = unique_ptr<EnergyLossParams>( new EnergyLossParams(&cfgEL, "EnergyLossParams[" + ts(cb) + "]") );
			}

		}	
	}
}

PidHistoMaker::~PidHistoMaker(){

}

void PidHistoMaker::preEventLoop() {
	logger->info(__FUNCTION__) << endl;
	
	InclusiveSpectra::preEventLoop();

	book->cd();
	prepareHistograms( centerSpecies );

	// make the energy loss histograms
	book->cd( "energyLoss" );
	for ( int c : charges ){
		for ( int cb : centralityBins ) {
			book->clone( "/", "corrPt", "energyLoss", "corrPt_" + Common::chargeString(c) + "_" + ts(cb) );
		}
	}	
}

void PidHistoMaker::postEventLoop() {
	logger->info(__FUNCTION__) << endl;

	if ( cfg->getBool( nodePath+"MakeQA:tof", false ) )
		reportAll( "" );
	if ( cfg->getBool( nodePath+"MakeQA:dedx", false ) )
		reportAll( "dedx" );

	book->cd();

}

void PidHistoMaker::analyzeTofTrack( int iTrack ){
	InclusiveSpectra::analyzeTofTrack( iTrack );
	
	book->cd();

	// Alias Track Traits
	int charge 		= pico->trackCharge( iTrack );
	double pt 		= pico->trackPt( iTrack );
	double p 		= pico->trackP( iTrack );
	double eta 		= pico->trackEta( iTrack );


	/************ Corrections **********/
	// Apply Energy Loss Corrections if given
	string elName = centerSpecies + "_" + Common::chargeString( charge ) +"_" + ts( cBin );
	if ( elParams.count( elName ) ){

		double corrPt = pt - elParams[ elName ]->eval( pt );
		// QA
		book->cd( "energyLoss" );
		book->fill( "corrPt_" + Common::chargeString( charge ) + "_" + ts(cBin), pt, pt - corrPt );
		book->cd(  );
		
		// recalc the p from corrPt and eta
		p = Common::p( corrPt, eta );
		pt = corrPt;
	} else {
		ERROR( "No Energy Loss Params Given - These must be applied here" )
	} 

	
	//if ( correctFeedDown && book->exists( "fdWeight_" + Common::chargeString(charge) ) )
	//	book->get3D( "fdWeight_" + Common::chargeString(charge) )->Fill( pt, fdWeight, cBin );
	/************ Corrections **********/

	// Must be done after corrections
	//double y 	= Common::rapidity( pt, eta, zr->mass( centerSpecies ) );
	int ptBin 	= binsPt->findBin( pt );
	trackPt = pt;
	double avgP = binAverageP( ptBin );

	// Require valid p bin
	if ( ptBin < 0 )
		return;

	book->cd();

	// Traditionally Recentered values
	double tof 		= zr->rTof(centerSpecies, pico->trackBeta(iTrack), p );
	double dedx 	= zr->rDedx(centerSpecies, pico->trackDedx(iTrack), p );
	// Non-Linearly Recentered values
	double tofNL 	= zr->nlTof(centerSpecies, pico->trackBeta(iTrack), p, avgP );
	double dedxNL 	= zr->nlDedx(centerSpecies, pico->trackDedx(iTrack), p, avgP );

	// Genral QA
	book->fill( "betaRaw", p, 1.0/pico->trackBeta( iTrack ) );
	book->fill( "dedxRaw", p, pico->trackDedx( iTrack ) );
	book->fill( "eta", eta );

	book->fill( "trBeta", p, tof );
	book->fill( "trDedx", p, dedx );
	book->fill( "nlBeta", p, tofNL );
	book->fill( "nlDedx", p, dedxNL );

	// Choose recentered values
	if ( "nonlinear" == zrMethod ){
		tof = tofNL;
		dedx = dedxNL;
	} 

	if ( make2D ){
		book->cd( "dedx_tof" );

		// combined charge 
		if ( makeCombinedCharge ) 
			book->fill( Common::speciesName( centerSpecies, 0, cBin, ptBin ), dedx, tof );
		
		book->fill( Common::speciesName( centerSpecies, charge, cBin, ptBin), dedx, tof );
			
	}
	
	enhanceDistributions(avgP, ptBin, charge, dedx, tof );

	book->cd();
}

void PidHistoMaker::enhanceDistributions( double avgP, int ptBin, int charge, double dedx, double tof ){
	
	double tSigma = tofSigmaIdeal;
	double dSigma = dedxSigmaIdeal;

	// get the centered tof and dedx means
	vector<double> tMeans 	= zr->centeredTofMeans( centerSpecies, avgP );
	vector<double> dMeans 	= zr->centeredDedxMeans( centerSpecies, avgP );
	vector<string> mSpecies = zr->allSpecies();

	double trackWeight = eventWeight;
	// TODO: Add options to turn on/off corrections in track weight
	if ( correctTpcEff ){
		trackWeight = trackWeight * sc->tpcEffWeight( centerSpecies, trackPt, cBin, charge ) ;
	}
	if ( correctTofEff ){
		trackWeight = trackWeight * sc->tofEffWeight( centerSpecies, trackPt, cBin, charge ) ;
	}
	if ( correctFeedDown ){
		trackWeight = trackWeight * sc->feedDownWeight( centerSpecies, trackPt, cBin, charge ) ;
	}



	book->cd( "tof" );
	// unenhanced - all tof tracks
	// combined charge 
	if ( makeCombinedCharge ) book->fill( Common::zbName( centerSpecies, 0, cBin, ptBin ), tof, trackWeight );
	book->fill( Common::zbName( centerSpecies, charge, cBin, ptBin ), tof, trackWeight );

	// enhanced by species
	if ( makeEnhanced ){
		for ( int iS = 0; iS < mSpecies.size(); iS++ ){
			if ( dedx >= dMeans[ iS ] - dSigma && dedx <= dMeans[ iS ] + dSigma ){
				if ( makeCombinedCharge ) book->fill( Common::zbName( centerSpecies, 0, cBin, ptBin, mSpecies[ iS ] ), tof, trackWeight );
				book->fill( Common::zbName( centerSpecies, charge, cBin, ptBin, mSpecies[ iS ] ), tof, trackWeight );
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
	if ( makeCombinedCharge ) book->fill( Common::zdName( centerSpecies, 0, cBin, ptBin ), dedx, trackWeight );
	book->fill( Common::zdName( centerSpecies, charge, cBin, ptBin ), dedx, trackWeight );

	// enhanced by species
	if ( makeEnhanced ){
		for ( int iS = 0; iS < mSpecies.size(); iS++ ){
			
			double ttMean = tMeans[ iS ];

			if ( tof >= ttMean - tSigma && tof <= ttMean + tSigma ){
				if ( makeCombinedCharge ) book->fill( Common::zdName( centerSpecies, 0, cBin, ptBin, mSpecies[ iS ] ), dedx, trackWeight );
				book->fill( Common::zdName( centerSpecies, charge, cBin, ptBin, mSpecies[ iS ] ), dedx, trackWeight );
			}
		} // loop on species from centered means	
	}
}

void PidHistoMaker::prepareHistograms( string plc ){

	logger->info(__FUNCTION__) << "Making Histograms with centering species: " << plc << endl;

	book->cd();

	// Loop through pt then charge
	for ( int ptBin = 0; ptBin < binsPt->nBins(); ptBin++ ){

		double p = binAverageP( ptBin );

		double tofLow, tofHigh, dedxLow, dedxHigh;
		autoViewport( plc, p, zr, &tofLow, &tofHigh, &dedxLow, &dedxHigh, tofPadding, dedxPadding, tofScalePadding, dedxScalePadding );
		
		vector<double> tofBins 	= HistoBins::makeFixedWidthBins( tofBinWidth, tofLow, tofHigh );
		vector<double> dedxBins = HistoBins::makeFixedWidthBins( dedxBinWidth, dedxLow, dedxHigh );

		// Loop over charge, skip if config doesnt include that charge bin
		for ( int charge : charges ){

			for ( int iCen = 0; iCen < nCentralityBins(); iCen++ ){

				// the name of the 2D histogram
				string hName = Common::speciesName( plc, charge, iCen, ptBin );

				string title = "dE/dx Vs. #beta^{-1}; dE/dx; #beta^{-1}";

				// 2d dedx X tof 
				// 2D for NMF 
				if ( make2D ){
					book->cd( "dedx_tof" );
					book->make2D( hName, title, dedxBins.size()-1, dedxBins.data(), tofBins.size()-1, tofBins.data() );
				}

				// tof projections
				book->cd( "tof" );
				book->make1D( Common::zbName( plc, charge, iCen, ptBin ), 
					"#beta^{-1}", tofBins.size()-1, tofBins.data() );
				
				if ( makeEnhanced ){ 
					for ( int iS = 0; iS < Common::species.size(); iS++ ){
						book->make1D( Common::zbName( plc, charge, iCen, ptBin, Common::species[ iS ] ), 
							"#beta^{-1}", tofBins.size()-1, tofBins.data() );
					} 
				}

				// dedx projections
				book->cd( "dedx" );		
				book->make1D( Common::zdName( plc, charge, iCen, ptBin ), 
								"dEdx", dedxBins.size()-1, dedxBins.data() );
				
				// Enhanced
				if ( makeEnhanced ){ 
					for ( int iS = 0; iS < Common::species.size(); iS++ ){
						book->make1D( Common::zdName( plc, charge, iCen, ptBin, Common::species[ iS ] ), 
							"dEdx", dedxBins.size()-1, dedxBins.data() );
					}
				}

			} // loop on centrality
		}// loop on charge
	} // loop on ptBins
}

void PidHistoMaker::autoViewport( 	string pType, double p, ZRecentering * lpsr, double * tofLow, double* tofHigh, double * dedxLow, double * dedxHigh, double tofPadding, double dedxPadding, double tofScaledPadding, double dedxScaledPadding  ){


	TofGenerator * tofGen = lpsr->tofGenerator();
	Bichsel * dedxGen = lpsr->dedxGenerator();

	double tofCenter = tofGen->mean( p, lpsr->mass( pType ) );
	double dedxCenter = dedxGen->meanLog( p, lpsr->mass( pType ), -1, 1000 );

	vector<double> tofMean;
	vector<double> dedxMean;

	for ( int i = 0; i < Common::species.size(); i ++ ){
		tofMean.push_back(  tofGen->mean( p, lpsr->mass( Common::species[ i ] ) ) );
		dedxMean.push_back( dedxGen->meanLog( p, lpsr->mass( Common::species[ i ] ), -1, 1000 ) );
	}


	double tHigh = (tofMean[ 0 ] - tofCenter);
	double tLow = (tofMean[ 0 ] - tofCenter);
	double dHigh = (dedxMean[ 0 ] - dedxCenter);
	double dLow = (dedxMean[ 0 ] - dedxCenter);
	for ( int i = 0; i < Common::species.size(); i++ ){
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

void PidHistoMaker::reportAll( string type ){



	logger->info( __FUNCTION__ ) << endl;
	
	double sigma = tofSigmaIdeal;
	if ( "dedx" == type )
		sigma = dedxSigmaIdeal;

	vector<int> charges = { -1, 0, 1 };

	book->cd();
	int nCenBins = nCentralityBins();
	map<string,  unique_ptr<Reporter> > rps;
	string baseURL = cfg->getString(  nodePath + "output:path", "./" );
	string baseName= cfg->getString(  nodePath + "output.Reports", "" );


	book->cd( "tof" );
	if ( "dedx" == type )
		book->cd( "dedx" );
	// Make the slew of reporters
	
	// Loop over charge, skip if config doesnt include that charge bin
	for ( int charge : charges ){
		
		for ( int iCen = 0; iCen < nCenBins; iCen++ ){
		
			string sn = Common::zbName( centerSpecies, charge, iCen, 1 );
			if ( "dedx" == type )
				sn = Common::zdName( centerSpecies, charge, iCen, 1 );
			
			if ( !book->exists( sn ) ) continue;

			rps[ sn ] = unique_ptr<Reporter>(new Reporter( baseURL + jobPrefix + baseName + sn + ".pdf", 1000, 500 )) ;


		} // loop centralities
	} // loop charges 
	

	
	// Loop through pt, then eta then charge
	for ( int pBin = 0; pBin < binsPt->nBins(); pBin++ ){
		// Loop over charge, skip if config doesnt include that charge bin
		for ( int charge : charges ){
			
			for ( int iCen = 0; iCen < nCenBins; iCen++ ){
			
				double avgP = binAverageP( pBin );	

				vector<double> tMeans = zr->centeredTofMeans( centerSpecies, avgP );
				vector<double> dMeans = zr->centeredDedxMeans( centerSpecies, avgP );				
				vector<double> means = tMeans;
				if ( "dedx" == type )
					means = dMeans;


				string n = Common::zbName( centerSpecies, charge, iCen, pBin );
				if ( "dedx" == type )
					n = Common::zdName( centerSpecies, charge, iCen, pBin );
				
				string sn = Common::zbName( centerSpecies, charge, iCen, 1 );
				if ( "dedx" == type )
					sn = Common::zdName( centerSpecies, charge, iCen, 1 );
				if ( !book->exists( n ) ) continue;

				string pLowEdge 	= dts(binsPt->getBins()[ pBin ]);
				string pHiEdge 		= dts(binsPt->getBins()[ pBin + 1 ]);
				//string range = dts(binsPt->getBins()[ pBin ]) + " < Pt < " + dts(binsPt->getBins()[ pBin ]) + " : #eta = " << dts(binsEta->getBins()[ etaBin ])

				if ( !rps[sn] ){
					logger->error(__FUNCTION__) << "NO reporter for : " << sn << endl;
					continue;
				}
				rps[ sn ]->newPage();
				book->style( n )->set( "logY", 1 )->
				set( "title", pLowEdge + " < P < " + pHiEdge )->
				set( "x", "z_{1/#beta}" )->
				set( "y", "events / " + dts( tofBinWidth ) )->
				set( "draw", "pe" )->
				draw();

				map<string, int> colors;
				colors[ "Pi" ] = kBlue;
				colors[ "K" ] = kGreen;
				colors[ "P" ] = kBlack;
				int i = 0;
				for ( string plc : Common::species ){
					
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
					logger->info(__FUNCTION__) << "pBin = " << pBin << " electron cut : " << ttMean - sigma * nSigBelow <<  endl;
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
	
	} // loop pt bins

}
