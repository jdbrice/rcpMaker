/**
 *
 *
 * 
 */
#include "PidPhaseSpace.h"
#include "TLine.h"

vector<string> PidPhaseSpace::species = { "Pi", "K", "P" };

PidPhaseSpace::PidPhaseSpace( XmlConfig* config, string np ) : InclusiveSpectra( config, np ) {

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
	psrMethod = config->getString( np+"PhaseSpaceRecentering.method", "traditional" );

	// alias the centered species for ease of use
	centerSpecies = cfg->getString( np+"PhaseSpaceRecentering.centerSpecies", "K" );

	tofCut = cfg->getDouble( np+"enhanceDistributions:tof", 1.0 );
	dedxCut = cfg->getDouble( np+"enhanceDistributions:dedx", 1.0 );
 }

void PidPhaseSpace::preLoop() {

	preparePhaseSpaceHistograms( centerSpecies );
	book->cd();
}

void PidPhaseSpace::postLoop() {
	lg->info(__FUNCTION__) << endl;
}


void PidPhaseSpace::analyzeTrack( int iTrack ){

	book->cd();

	double pt = pico->trackPt( iTrack );
	double p = pico->trackP( iTrack );
	double eta = pico->trackEta( iTrack );

	int ptBin = binsPt->findBin( pt );
	int etaBin = binsEta->findBin( TMath::Abs( eta ) );

	if ( ptBin < 0 || etaBin < 0)
		return;

	book->fill( "eta", eta );
	double avgP = ( binsPt->bins[ ptBin ] + binsPt->bins[ ptBin+1] ) / 2.0;
	//lg->info(__FUNCTION__) << "Pt: " << binsPt->bins[ ptBin ] << " -> " << binsPt->bins[ ptBin+1 ] <<  ", <P> = " << avgP << endl;

	string hName = speciesName( centerSpecies, 0, ptBin, etaBin );

	double tof=psr->rTof(centerSpecies, pico->trackBeta(iTrack), p );
	double dedx=psr->rDedx(centerSpecies, pico->trackDedx(iTrack), p );

	book->fill( "trBeta", p, tof );
	book->fill( "beta", p, 1.0/pico->trackBeta( iTrack ) );

	book->fill( "trDedx", p, dedx );
	book->fill( "dedx", p, pico->trackDedx( iTrack ) );

	double tofNL=psr->nlTof(centerSpecies, pico->trackBeta(iTrack), p, avgP );
	double dedxNL=psr->nlDedx(centerSpecies, pico->trackDedx(iTrack), p, avgP );

	book->fill( "nlBeta", p, tofNL );
	book->fill( "nlDedx", p, dedxNL );

	if ( "nonlinear" == psrMethod ){
		tof = tofNL;
		dedx = dedxNL;
	} 

	book->cd( "dedx_tof" );
	book->fill( hName, dedx, tof );

	enhanceDistributions(ptBin, etaBin, dedx, tof );

	book->cd();
}

void PidPhaseSpace::preparePhaseSpaceHistograms( string plc){

	lg->info(__FUNCTION__) << "Making Histograms with centering spceies: " << plc << endl;

	book->cd();
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


		
	lg->info(__FUNCTION__) << "Made All Bins" << endl;

	// Loop through pt, then eta then charge
	for ( int ptBin = 0; ptBin < binsPt->size()-1; ptBin++ ){

		double p = (binsPt->bins[ ptBin ] + binsPt->bins[ ptBin + 1 ]) / 2.0;

		double tofLow, tofHigh, dedxLow, dedxHigh;
		autoViewport( plc, p, psr, &tofLow, &tofHigh, &dedxLow, &dedxHigh, tofPadding, dedxPadding, tofScalePadding, dedxScalePadding );
		
		
		vector<double> tofBins = HistoBook::makeFixedWidthBins( tofBinWidth, tofLow, tofHigh );
		vector<double> dedxBins = HistoBook::makeFixedWidthBins( dedxBinWidth, dedxLow, dedxHigh );

		for ( int etaBin = 0; etaBin < binsEta->size()-1; etaBin++ ){
			for ( int charge = -1; charge <= 1; charge++ ){

				
				int cBin = binsCharge->findBin( charge );
				if ( cBin < 0 )
					continue;

				// the name of the histogram
				string hName = speciesName( plc, charge, ptBin, etaBin );

				//lg->info(__FUNCTION__) << "Making : " << hName << endl;

				string title = "dedx vs. tof; dedx; 1/#beta";

				book->cd( "dedx_tof" );
				// make it and keep it in the HistoBook
				book->make2D( hName, title, 
					dedxBins.size()-1, dedxBins.data(),
					tofBins.size()-1, tofBins.data() );

				book->cd( "tof" );
				string tName = tofName( plc, charge, ptBin, etaBin );
				book->make1D( tName, "#beta^{-1}", tofBins.size()-1, tofBins.data() );
				for ( int iS = 0; iS < species.size(); iS++ ){
					tName = tofName( plc, charge, ptBin, etaBin, species[ iS ] );
					book->make1D( tName, "#beta^{-1}", tofBins.size()-1, tofBins.data() );
				} 

				book->cd( "dedx" );
				string dName = dedxName( plc, charge, ptBin, etaBin );
				book->cd( "dedx" );
				book->make1D( dName, "dEdx", dedxBins.size()-1, dedxBins.data() );
				for ( int iS = 0; iS < species.size(); iS++ ){
					book->cd( "dedx" );
					dName = dedxName( plc, charge, ptBin, etaBin, species[ iS ] );
					book->make1D( dName, "dEdx", dedxBins.size()-1, dedxBins.data() );
				} 


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



void PidPhaseSpace::enhanceDistributions( int ptBin, int etaBin, double dedx, double tof ){

	double avgP = ( binsPt->bins[ ptBin ] + binsPt->bins[ ptBin+1] ) / 2.0;
	
	double dSigma = dedxSigmaIdeal * dedxCut;
	double tSigma = tofSigmaIdeal * tofCut;

	vector<double> tMeans = psr->centeredTofMeans( centerSpecies, avgP );
	vector<double> dMeans = psr->centeredDedxMeans( centerSpecies, avgP );
	vector<string> mSpecies = psr->allSpecies();

	book->cd( "dedx" );
	// unenhanced - all dedx
	string dName = dedxName( centerSpecies, 0, ptBin, etaBin );
	book->fill( dName, dedx );
	for ( int iS = 0; iS < mSpecies.size(); iS++ ){
		dName = dedxName( centerSpecies, 0, ptBin, etaBin, mSpecies[ iS ] );
		if ( tof >= tMeans[ iS ] -tSigma && tof <= tMeans[ iS ] + tSigma ){
			book->fill( dName, dedx );
		}
	} // loop on species from centered means
	

	book->cd( "tof" );
	// unenhanced - all tof tracks
	string tName = tofName( centerSpecies, 0, ptBin, etaBin );
	book->fill( tName, tof );

	for ( int iS = 0; iS < mSpecies.size(); iS++ ){
		tName = tofName( centerSpecies, 0, ptBin, etaBin, mSpecies[ iS ] );
		if ( dedx >= dMeans[ iS ] -dSigma && dedx <= dMeans[ iS ] + dSigma ){
			book->fill( tName, tof );
		}
	} // loop on species from centered means	
}













