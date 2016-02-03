 // RcpMaker
#include "Spectra/PidHistoMaker.h"
#include "Correction/SpectraCorrecter.h"
#include "RooPlotLib.h"

// ROOT
#include "TLine.h"


void PidHistoMaker::initialize() {

	InclusiveSpectra::initialize();
	
	/**
	 * Phase space padding options
	 */
 	tofPadding 			= config.getDouble( "binning.padding:tof", .2 );
	dedxPadding 		= config.getDouble( "binning.padding:dedx", .25 );
	tofScalePadding 	= config.getDouble( "binning.padding.tofScale", .05 );
	dedxScalePadding 	= config.getDouble( "binning.padding.dedxScale", .05 );

	INFO( classname(), "Tof Padding ( " << tofPadding << ", " << tofScalePadding << " ) " );
	/**
	 * Initialize the Phase Space Recentering Object
	 */
	tofSigmaIdeal 	= config.getDouble( nodePath+".ZRecentering.sigma:tof", 0.011);
	dedxSigmaIdeal 	= config.getDouble( nodePath+".ZRecentering.sigma:dedx", 0.033);
	zr 				= new ZRecentering( dedxSigmaIdeal,
									 	tofSigmaIdeal,
									 	config.getString( nodePath+".Bichsel.table", "dedxBichsel.root"),
									 	config.getInt( nodePath+".Bichsel.method", 0) );

		// method for phase space recentering
	zrMethod 		= config.getString( nodePath + ".ZRecentering.method", "traditional" );
		// alias the centered species for ease of use
	centerSpecies 	= config.getString( nodePath + ".ZRecentering.centerSpecies", "K" );
		// enhanced distro options
	tofCut 			= config.getDouble( nodePath+".Distributions:tof", 1.0 );
	dedxCut 		= config.getDouble( nodePath+".Distributions:dedx", 1.0 );

	// Flags to turn on and off certain histos
	make1D 			= config.getBool( nodePath + ".Distributions:1D", false );
	make2D 			= config.getBool( nodePath + ".Distributions:2D", false );
	makeEnhanced 	= config.getBool( nodePath + ".Distributions:enhanced", false );

	if ( makeEnhanced && !make1D )
		make1D = true;

	// n sigmas for electron rejection
	nSigE 			= config.getDouble( nodePath+".Electrons:nSigE", 3.0 );
	nSigPi 			= config.getDouble( nodePath+".Electrons:nSigPi", 3.0 );
	nSigK 			= config.getDouble( nodePath+".Electrons:nSigK", 3.0 );
	nSigP 			= config.getDouble( nodePath+".Electrons:nSigP", 3.0 );

	// Make the dedx + tof binning 
	// Only the bin width is used for dynamic bins
	dedxBinWidth 	= config.getDouble( "binning.dedx:width", 0.015 );
	tofBinWidth 	= config.getDouble( "binning.tof:width", 0.006 );

	//Make the momentum transverse binning
	binsPt 	= unique_ptr<HistoBins>(new HistoBins( config, "binning.pt" ));

	// Get the list of charges we are looking at
	charges = config.getIntVector( "binning.charges" );


	// Cuts below Pion to remove electrons
	// And above P to remove deuterons
	nSigBelow = config.getDouble( nodePath + ".Distributions:nSigBelow", 3.0 );
	nSigAbove = config.getDouble( nodePath + ".Distributions:nSigAbove", 3.0 );
	
	INFO( classname(), "nSig cuts for e : Pi - " << nSigBelow << " sigma" );
	INFO( classname(), "nSig cuts for d : P + " << nSigAbove << " sigma" );

	// TODO: use charges vector to decide if we want to combine charges
	makeCombinedCharge = false;

	// Efficiency corrector
	sc = unique_ptr<SpectraCorrecter>( new SpectraCorrecter( &config, nodePath ) ); 
	// Read in the options for corrections
	correctTpcEff = config.getBool( nodePath + ".Corrections:tpc", false );
	correctTofEff = config.getBool( nodePath + ".Corrections:tof", false );
	correctFeedDown = config.getBool( nodePath + ".Corrections:fd", false );

	// make the energy loss params
	vector<int> charges = { -1, 1 };
	if ( config.exists( nodePath + ".EnergyLossParams:path" ) ){
		string path = config.getString( nodePath + ".EnergyLossParams:path" );

		for ( int c : charges ){

			string cfgName = path + centerSpecies + "_" + Common::chargeString( c ) + ".xml";
			XmlConfig cfgEL( cfgName );

			for ( int cb : config.getIntVector( nodePath + ".CentralityBins" ) ){
				
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
	INFO( classname(), "" );
	
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
	INFO( classname(), "" );

	if ( config.getBool( nodePath+".MakeQA:tof", false ) )
		reportAll( "" );
	if ( config.getBool( nodePath+".MakeQA:dedx", false ) )
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
	trackPt 		= pt; // saved for whole track calculations

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
		ERROR( classname(), "No Energy Loss Params Given - These must be applied here" )
	} 
	/************ Corrections **********/

	// Must be done after corrections
	int ptBin 	= binsPt->findBin( pt );
	corrTrackPt = pt;
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
	book->fill( "betaRaw", p * charge, 1.0/pico->trackBeta( iTrack ) );
	book->fill( "dedxRaw", p * charge, log( pico->trackDedx( iTrack ) ) );
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

	
	// show the order 
	if ( rejectElectron(avgP, dedx, tof ) ){
		if ( enhanceDistributions(avgP, ptBin, charge, dedx, tof ) ){
			book->cd();
			book->fill( "cutBeta", p * charge, 1.0/pico->trackBeta( iTrack ) );
			book->fill( "cutDedx", p * charge, log( pico->trackDedx( iTrack ) ) );
		}
	}

	book->cd();

	
}

bool PidHistoMaker::rejectElectron( double avgP, double dedx, double tof ){

	map< string, double > tMeans 	= zr->centeredTofMap( centerSpecies, avgP );
	map< string, double > dMeans 	= zr->centeredDedxMap( centerSpecies, avgP );

	double dxe = dMeans["E"] - dedx;
	double dye = tMeans["E"] - tof;
	double dse = sqrt( dxe*dxe / ( dedxSigmaIdeal * dedxSigmaIdeal ) + dye*dye / ( tofSigmaIdeal*tofSigmaIdeal ) );

	double dxpi = dMeans["Pi"] - dedx;
	double dypi = tMeans["Pi"] - tof;
	double dspi = sqrt( dxpi*dxpi / ( dedxSigmaIdeal * dedxSigmaIdeal ) + dypi*dypi / ( tofSigmaIdeal*tofSigmaIdeal ) );

	double dxk = dMeans["K"] - dedx;
	double dyk = tMeans["K"] - tof;
	double dsk = sqrt( dxk*dxk / ( dedxSigmaIdeal * dedxSigmaIdeal ) + dyk*dyk / ( tofSigmaIdeal*tofSigmaIdeal ) );

	double dxp = dMeans["P"] - dedx;
	double dyp = tMeans["P"] - tof;
	double dsp = sqrt( dxp*dxp / ( dedxSigmaIdeal * dedxSigmaIdeal ) + dyp*dyp / ( tofSigmaIdeal*tofSigmaIdeal ) );	

	// if we are within nSigma from a plc peak then keep no matter what
	if ( dspi < nSigPi || dsk < nSigK || dsp < nSigP )
		return true;

	// if we are within nSigma from electron peak but away from those others then reject
	if ( dse < nSigE  ){
		return false;
	}

	// keep everything else
	return true;
}

bool PidHistoMaker::enhanceDistributions( double avgP, int ptBin, int charge, double dedx, double tof ){
	
	// get the centered tof and dedx means
	map< string, double > tMeans 	= zr->centeredTofMap( centerSpecies, avgP );
	map< string, double > dMeans 	= zr->centeredDedxMap( centerSpecies, avgP );


	// Cut either N sigma above protons or N sigma below deuterons
	// since the sigma of the Proton peak is very large at low momentum
	// the N (ideal sigma) cut is far too tight
	double deuteronCut = tMeans[ "P" ] + tofSigmaIdeal * nSigAbove;
	//if ( tMeans[ "D" ] - tofSigmaIdeal * ( nSigAbove * 2 ) > deuteronCut )
	//	deuteronCut = tMeans[ "D" ] - tofSigmaIdeal * ( nSigAbove * 2 );


	double trackWeight = eventWeight;

	// Here we are intentionally using the pre-EnergyLoss pt because that is what the 
	// Efficiencies and FeedDown are parameterized in
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
	if ( make1D ){
		if ( makeCombinedCharge ) book->fill( Common::zbName( centerSpecies, 0, cBin, ptBin ), tof, trackWeight );
		book->fill( Common::zbName( centerSpecies, charge, cBin, ptBin ), tof, trackWeight );
	}

	// enhanced by species
	if ( makeEnhanced ){
		for ( string plc : Common::species ){
			if ( dedx >= dMeans[ plc ] - dedxSigmaIdeal && dedx <= dMeans[ plc ] + dedxSigmaIdeal ){
				if ( makeCombinedCharge ) book->fill( Common::zbName( centerSpecies, 0, cBin, ptBin, plc ), tof, trackWeight );
				book->fill( Common::zbName( centerSpecies, charge, cBin, ptBin, plc ), tof, trackWeight );
			}
		} // loop on species from centered means
	}

	
	// reject electrons
	if ( tof < tMeans[ "Pi" ] - tofSigmaIdeal * nSigBelow )
		return false;
	// reject deuteron
	if ( tof > deuteronCut )
		return false;

	if ( make2D ){
		book->cd( "dedx_tof" );
		// combined charge 
		if ( makeCombinedCharge ) 
			book->fill( Common::speciesName( centerSpecies, 0, cBin, ptBin ), dedx, tof );
		
		book->fill( Common::speciesName( centerSpecies, charge, cBin, ptBin), dedx, tof );
			
	}


	book->cd( "dedx" );
	// unenhanced - all dedx
	// combined charge 
	if ( make1D ){
		if ( makeCombinedCharge ) book->fill( Common::zdName( centerSpecies, 0, cBin, ptBin ), dedx, trackWeight );
		book->fill( Common::zdName( centerSpecies, charge, cBin, ptBin ), dedx, trackWeight );
	}

	// enhanced by species
	if ( makeEnhanced ){
		for ( string plc : Common::species ){
			
			double ttMean = tMeans[ plc ];

			if ( tof >= ttMean - tofSigmaIdeal && tof <= ttMean + tofSigmaIdeal ){
				if ( makeCombinedCharge ) book->fill( Common::zdName( centerSpecies, 0, cBin, ptBin, plc ), dedx, trackWeight );
				book->fill( Common::zdName( centerSpecies, charge, cBin, ptBin, plc ), dedx, trackWeight );
			}
		} // loop on species from centered means	
	}

	return true;
}

void PidHistoMaker::prepareHistograms( string plc ){

	INFO( classname(), "Making Histograms with centering species: " << plc );

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
					book->add( hName, 
						new TH2D( hName.c_str(), title.c_str(), dedxBins.size()-1, dedxBins.data(), tofBins.size()-1, tofBins.data() ) );
				}


				// tof projections
				book->cd( "tof" );
				if ( make1D ){
					string h1Name = Common::zbName( plc, charge, iCen, ptBin );
					book->add( h1Name, 
						new TH1D( h1Name.c_str(), "#beta^{-1}", tofBins.size()-1, tofBins.data() ) );
				}
				
				if ( makeEnhanced ){ 
					for ( string eplc : Common::species ){
						string h1Name = Common::zbName( plc, charge, iCen, ptBin, eplc );
						book->add( h1Name, 
							 new TH1D( h1Name.c_str(), "#beta^{-1}", tofBins.size()-1, tofBins.data() ) );
					} 
				}

				// dedx projections
				book->cd( "dedx" );		
				if ( make1D ){
					string h1Name = Common::zdName( plc, charge, iCen, ptBin );
					book->add( h1Name, 
								new TH1D( h1Name.c_str(), "dEdx", dedxBins.size()-1, dedxBins.data() ) );
				}
				
				// Enhanced
				if ( makeEnhanced ){ 
					for ( string eplc : Common::species ){
						string h1Name = Common::zdName( plc, charge, iCen, ptBin, eplc );
						book->add( h1Name, 
							new TH1D( h1Name.c_str(), "dEdx", dedxBins.size()-1, dedxBins.data() ) );
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

	RooPlotLib rpl;

	INFO( classname(), "" );
	
	double sigma = tofSigmaIdeal;
	if ( "dedx" == type )
		sigma = dedxSigmaIdeal;

	vector<int> charges = { -1, 0, 1 };

	book->cd();
	int nCenBins = nCentralityBins();
	map<string,  unique_ptr<Reporter> > rps;
	string baseURL = config.getString(  nodePath + ".output:path", "./" );
	string baseName= config.getString(  nodePath + ".output.Reports", "" );


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

			rps[ sn ] = unique_ptr<Reporter>(new Reporter( baseURL + jobPostfix + baseName + sn + ".pdf", 1000, 500 )) ;


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
					INFO( classname(), "NO reporter for : " << sn );
					continue;
				}
				rps[ sn ]->newPage();
				rpl.style( book->get( n ) ).set( "logY", 1 ).
				set( "title", pLowEdge + " < P < " + pHiEdge ).
				set( "x", "z_{1/#beta}" ).
				set( "y", "events / " + dts( tofBinWidth ) ).
				set( "draw", "pe" ).
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
					INFO( classname(), "PiMu = " << ttMean << ", sigma = " << sigma << ", nSigma = " << nSigBelow ); 
					INFO( classname(), "pBin = " << pBin << " electron cut : " << ttMean - sigma * nSigBelow );
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
