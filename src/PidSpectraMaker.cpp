#include "PidSpectraMaker.h"

#include "SpectraCorrecter.h"

PidSpectraMaker::PidSpectraMaker( XmlConfig* config, string np, string fl, string jp ) : InclusiveSpectra( config, np, fl, jp ) {

	book->setLogLevel( "warning" );
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
	binsCharge = unique_ptr<HistoBins>(new HistoBins( cfg, "bin.charge" ));


	/**
	 * Cuts below Pion to remove electrons
	 * And above P to remove deuterons
	 */
	nSigBelow = cfg->getDouble( nodePath + "enhanceDistributions:nSigBelow", 3.0 );
	nSigAbove = cfg->getDouble( nodePath + "enhanceDistributions:nSigAbove", 3.0 );
	logger->info(__FUNCTION__) << "nSig cuts for e, d : " << nSigBelow << ", " << nSigAbove << endl;


	ppm = new PidProbabilityMapper( cfg, nodePath + "PidProbabilityMapper." );

	logger->info(__FUNCTION__) << "Making correcter" << endl;      
	sc = unique_ptr<SpectraCorrecter>( new SpectraCorrecter( "config/15GeV/efficiency.xml" ) ); 

	//logger->info(__FUNCTION__) << "reweight = " << sc->reweight( "Pi", 1, 15, 0.15 ) << endl;

 }

 PidSpectraMaker::~PidSpectraMaker(){

 }

void PidSpectraMaker::preEventLoop() {
	logger->info(__FUNCTION__) << endl;
	
	InclusiveSpectra::preEventLoop();

	book->cd();
	prepareHistograms();
	
}

void PidSpectraMaker::postEventLoop() {
	logger->info(__FUNCTION__) << endl;

	book->cd();

}

void PidSpectraMaker::analyzeTrack( int iTrack ){
	
	using namespace TMath;

	book->cd();


	double pt = pico->trackPt( iTrack );
	double p = pico->trackP( iTrack );
	double eta = pico->trackEta( iTrack );

	vector<double> yPlc;
	for ( string plc : PidPhaseSpace::species ){
		double m = psr->mass( plc );

		double a = Sqrt( m*m + pt*pt*CosH( eta )*CosH( eta ) ) + pt * SinH( eta );
		double b = Sqrt( m*m + pt*pt );
		yPlc.push_back( Log( a / b ) );
	}	



	//int ptBin = binsPt->findBin( pt );
	int ptBin = binsPt->findBin( p );
	int etaBin = binsEta->findBin( TMath::Abs( 0 ) );
	int charge = pico->trackCharge( iTrack );

	double avgP = averageP( ptBin, etaBin );

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

	if ( "nonlinear" == psrMethod ){
		tof = tofNL;
		dedx = dedxNL;
	} 


	// get the cut values in terms of ideal sigma
	double dSigma = dedxSigmaIdeal * dedxCut;
	double tSigma = tofSigmaIdeal * tofCut;

	// get the centered tof and dedx means
	vector<double> tMeans = psr->centeredTofMeans( centerSpecies, avgP );
	vector<double> dMeans = psr->centeredDedxMeans( centerSpecies, avgP );


	// n sigma below pi to reject electrons
	// and n sigma above proton to reject deuteron
	if ( 	tof < tMeans[ 0 ] - tofSigmaIdeal * nSigBelow 
			|| tof > tMeans[ 2 ] + tofSigmaIdeal * nSigAbove )
		return;

	//cout << "Pi y = " << yPlc[ 0 ] << endl; 
	map< string, double> weights = ppm->pidWeights( charge, cBin, p/*TODO*/, 0, tof, dedx );

	double tofEffWeight = sc->reweight( "tof", 0, refMult, pt );

	int i = 0;
	for ( string plc : PidPhaseSpace::species ){

		if ( Abs(yPlc[ i ]) > 0.25 && weights[ plc ] > 0 )
			continue;
		
		double effWeight = sc->reweight( plc, charge, refMult, pt );

		book->cd( plc );
		string cName = "pt_" + ts( cBin ) + "_" + PidPhaseSpace::chargeString( charge );
		
		int bin = book->get( cName )->GetXaxis()->FindBin( pt );
		double bWidth = book->get( cName )->GetXaxis()->GetBinWidth( bin );

		logger->debug(__FUNCTION__) << "Filling " << cName << " = " << eventWeight * weights[ plc ] / bWidth << endl;
		logger->debug(__FUNCTION__) << "Weight = " << weights[ plc ] << endl;
		logger->debug(__FUNCTION__) << "EventWeight = " << eventWeight << endl;
		logger->debug(__FUNCTION__) << "bWidth = " << bWidth << endl;

		double fullWeight = eventWeight * weights[ plc ] * effWeight * tofEffWeight * ( 1.0 / bWidth);
		book->fill( cName, pt, fullWeight );

		i++;
	}

	
	//enhanceDistributions(avgP, ptBin, etaBin, charge, dedx, tof );

	book->cd();
}

void PidSpectraMaker::prepareHistograms(  ){

	logger->info(__FUNCTION__) <<  endl;


	for ( string plc : PidPhaseSpace::species ){
		book->cd( plc );
		for ( int iCen = 0; iCen < nCentralityBins(); iCen++ ){
			book->clone( "/", "ptBase", plc, "pt_"+ts(iCen)+"_p" );
			book->clone( "/", "ptBase", plc, "pt_"+ts(iCen)+"_n" );
		}	
	}
	

}










