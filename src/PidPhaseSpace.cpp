/**
 *
 *
 * 
 */
#include "PidPhaseSpace.h"
#include "TLine.h"

vector<string> PidPhaseSpace::species = { "Pi", "K", "P" };

PidPhaseSpace::PidPhaseSpace( XmlConfig* config, string np, string fl, string jp ) : InclusiveSpectra( config, np, fl, jp ) {

	// Phase space padding options
 	tofPadding = cfg->getDouble( "binning.padding:tof", .2 );
	dedxPadding = cfg->getDouble( "binning.padding:dedx", .25 );
	tofScalePadding = cfg->getDouble( "binning.padding.tofScale", .05 );
	dedxScalePadding = cfg->getDouble( "binning.padding.dedxScale", .05 );

	lg->info(__FUNCTION__) << "Tof Padding ( " << tofPadding << ", " << tofScalePadding << " ) " << endl;

	// Initialize the Phase Space Recentering Object
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
	binsPt = new HistoBins( cfg, "binning.pt" );

	/**
	 * Make the eta binning
	 */
	binsEta = new HistoBins( cfg, "binning.eta" );

	/**
	 * Make charge bins
	 */
	binsCharge = new HistoBins( cfg, "binning.charge" );
 }

void PidPhaseSpace::preLoop() {

	book->cd();
	preparePhaseSpaceHistograms( centerSpecies );
	
}

void PidPhaseSpace::postLoop() {
	lg->info(__FUNCTION__) << endl;
}


void PidPhaseSpace::analyzeTrack( int iTrack ){

	book->cd();

	UInt_t refMult = pico->eventRefMult();
	double pt = pico->trackPt( iTrack );
	double p = pico->trackP( iTrack );
	double eta = pico->trackEta( iTrack );

	int ptBin = binsPt->findBin( pt );
	int etaBin = binsEta->findBin( TMath::Abs( eta ) );
	int charge = pico->trackCharge( iTrack );

	double avgP = ( binsPt->bins[ ptBin ] + binsPt->bins[ ptBin+1] ) / 2.0;

	if ( ptBin < 0 || etaBin < 0)
		return;

	double tof = psr->rTof(centerSpecies, pico->trackBeta(iTrack), p );
	double dedx = psr->rDedx(centerSpecies, pico->trackDedx(iTrack), p );

	double tofNL = psr->nlTof(centerSpecies, pico->trackBeta(iTrack), p, avgP );
	double dedxNL = psr->nlDedx(centerSpecies, pico->trackDedx(iTrack), p, avgP );

	book->fill( "trBeta", p, tof );
	book->fill( "betaRaw", p, 1.0/pico->trackBeta( iTrack ) );

	book->fill( "trDedx", p, dedx );
	book->fill( "dedxRaw", p, pico->trackDedx( iTrack ) );
	book->fill( "eta", eta );

	book->fill( "nlBeta", p, tofNL );
	book->fill( "nlDedx", p, dedxNL );

	if ( "nonlinear" == psrMethod ){
		tof = tofNL;
		dedx = dedxNL;
	} 

	if ( make2D ){
		book->cd( "dedx_tof" );
		// Loop over centralities and fill any corresponding histos
		for ( int iCen = 0; iCen < centrals.size(); iCen ++  ){
			
			if ( refMult < cutCentrality[ centrals[ iCen ] ]->min || refMult > cutCentrality[ centrals[ iCen ] ]->max )
				continue;

			book->fill( speciesName( centerSpecies, 0, centrals[ iCen ], ptBin, etaBin ), dedx, tof );
			book->fill( speciesName( centerSpecies, charge, centrals[ iCen ], ptBin, etaBin ), dedx, tof );
		}	
	}
	
	enhanceDistributions(avgP, ptBin, etaBin, charge, dedx, tof );

	book->cd();
}

void PidPhaseSpace::preparePhaseSpaceHistograms( string plc ){

	lg->info(__FUNCTION__) << "Making Histograms with centering spceies: " << plc << endl;

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
			for ( int charge = -1; charge <= 1; charge++ ){
				int cBin = binsCharge->findBin( charge );
				if ( cBin < 0 )
					continue;

				for ( int iCen = 0; iCen < centrals.size(); iCen++ ){

					// the name of the 2D histogram
					string hName = speciesName( plc, charge, centrals[ iCen ], ptBin, etaBin  );

					string title = "dE/dx Vs. #beta^{-1}; dE/dx; #beta^{-1}";

					// 2d dedx X tof 
					// 2D for NMF 
					if ( make2D ){
						book->cd( "dedx_tof" );
						book->make2D( hName, title, dedxBins.size()-1, dedxBins.data(), tofBins.size()-1, tofBins.data() );
					}

					// tof projections
					book->cd( "tof" );
					book->make1D( tofName( plc, charge, centrals[ iCen ], ptBin, etaBin ), 
						"#beta^{-1}", tofBins.size()-1, tofBins.data() );
					
					if ( makeEnhanced ){ 
						for ( int iS = 0; iS < species.size(); iS++ ){
							book->make1D( tofName( plc, charge, centrals[ iCen ], ptBin, etaBin, species[ iS ] ), 
								"#beta^{-1}", tofBins.size()-1, tofBins.data() );
						} 
					}

					// dedx projections
					book->cd( "dedx" );		
					book->make1D( dedxName( plc, charge, centrals[ iCen ], ptBin, etaBin ), 
									"dEdx", dedxBins.size()-1, dedxBins.data() );
					
					// Enhanced
					if ( makeEnhanced ){ 
						for ( int iS = 0; iS < species.size(); iS++ ){
							book->make1D( dedxName( plc, charge, centrals[ iCen ], ptBin, etaBin, species[ iS ] ), 
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
	double dedxCenter = dedxGen->mean10( p, lpsr->mass( pType ), -1, 1000 );

	vector<double> tofMean;
	vector<double> dedxMean;

	for ( int i = 0; i < species.size(); i ++ ){
		tofMean.push_back(  tofGen->mean( p, lpsr->mass( species[ i ] ) ) );
		dedxMean.push_back( dedxGen->mean10( p, lpsr->mass( species[ i ] ), -1, 1000 ) );
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
	
	UInt_t refMult = pico->eventRefMult();

	// get the cut values in terms of ideal sigma
	double dSigma = dedxSigmaIdeal * dedxCut;
	double tSigma = tofSigmaIdeal * tofCut;

	// get the centered tof and dedx means
	vector<double> tMeans = psr->centeredTofMeans( centerSpecies, avgP );
	vector<double> dMeans = psr->centeredDedxMeans( centerSpecies, avgP );
	vector<string> mSpecies = psr->allSpecies();

	for ( int iCen = 0; iCen < centrals.size(); iCen ++  ){
		if ( refMult < cutCentrality[ centrals[ iCen ] ]->min || refMult > cutCentrality[ centrals[ iCen ] ]->max )
			continue;

		string cen = centrals[ iCen ];

		book->cd( "dedx" );
		// unenhanced - all dedx
		book->fill( dedxName( centerSpecies, 0, cen, ptBin, etaBin ), dedx );
		book->fill( dedxName( centerSpecies, charge, cen, ptBin, etaBin ), dedx );
		

		// enhanced by species
		if ( makeEnhanced ){
			for ( int iS = 0; iS < mSpecies.size(); iS++ ){
				if ( tof >= tMeans[ iS ] -tSigma && tof <= tMeans[ iS ] + tSigma ){
					book->fill( dedxName( centerSpecies, 0, cen, ptBin, etaBin, mSpecies[ iS ] ), dedx );
					book->fill( dedxName( centerSpecies, charge, cen, ptBin, etaBin, mSpecies[ iS ] ), dedx );
				}
			} // loop on species from centered means	
		}

		book->cd( "tof" );
		// unenhanced - all tof tracks
		book->fill( tofName( centerSpecies, 0, cen, ptBin, etaBin ), tof );
		book->fill( tofName( centerSpecies, charge, cen, ptBin, etaBin ), tof );

		// enhanced by species
		if ( makeEnhanced ){
			for ( int iS = 0; iS < mSpecies.size(); iS++ ){
				if ( dedx >= dMeans[ iS ] -dSigma && dedx <= dMeans[ iS ] + dSigma ){
					book->fill( tofName( centerSpecies, 0, cen, ptBin, etaBin, mSpecies[ iS ] ), tof );
					book->fill( tofName( centerSpecies, charge, cen, ptBin, etaBin, mSpecies[ iS ] ), tof );
				}
			} // loop on species from centered means
		}

	} // loop centralities
}













