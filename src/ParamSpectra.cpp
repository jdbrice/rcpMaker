#include "ParamSpectra.h"

string ParamSpectra::DEDX_CUT = "dedx";
string ParamSpectra::TOF_CUT = "tof";
string ParamSpectra::SQUARE_CUT = "square";
string ParamSpectra::ELLIPSE_CUT = "ellipse";


ParamSpectra::ParamSpectra( XmlConfig * config, string np) 
	: InclusiveSpectra( config, np ) {

	species = cfg->getStringVector( np+"PidSpecies" );

	lg->info(__FUNCTION__) << "Setting up Tof Pid Params for : " << endl;
	// make the list of tofPidParams
	for ( int i = 0; i < species.size(); i++ ){

		lg->info(__FUNCTION__) << species[ i ] << endl;

		TofPidParams * tpp = new TofPidParams( cfg, np + "TofPidParams." + species[ i ] + "." );
		tofParams.push_back( tpp );
	}

	// Initialize the Phase Space Recentering Object
	psr = new PhaseSpaceRecentering( cfg->getDouble( np+"PhaseSpaceRecentering.sigma:dedx", 0.06),
									 cfg->getDouble( np+"PhaseSpaceRecentering.sigma:tof", 0.0012),
									 cfg->getString( np+"Bichsel.table", "dedxBichsel.root"),
									 cfg->getInt( np+"Bichsel.method", 0) );
	// alias the centered species for ease of use
	centerSpecies = cfg->getString( np+"PhaseSpaceRecentering.centerSpecies", "K" );

	// get the Bins for pt and eta
	binsPt = new HistoBins( cfg, "binning.pt" );
	binsEta = new HistoBins( cfg, "binning.eta" );

	// initialize the pid cut
	nSigmaTof = cfg->getDouble( np+"nSigmaCut:tof", 1.0 );
	nSigmaDedx = cfg->getDouble( np+"nSigmaCut:dedx", 1.0 );
	nSigmaType = cfg->getString( np+"nSigmaCut:type", SQUARE_CUT );

	book->cd();

}


void ParamSpectra::preLoop(){

	lg->info(__FUNCTION__) << endl;
	
	for ( int iS = 0; iS < species.size(); iS ++ ){
		book->clone( "mean", "mean" + species[ iS ] );
		book->clone( "sigma", "sigma" + species[ iS ] );

		book->clone( "ptBase", "pt" + species[ iS ] );
	}


	for ( double ip = .2; ip < 4; ip+=.025 ){
		for ( int iS = 0; iS < species.size(); iS ++ ){
			double tofMean = tofParams[ iS ]->mean( ip, psr->mass( species[ iS ] ) ) ;
			double tofSigma = tofParams[ iS ]->sigma( ip, psr->mass( species[ iS ] ) ) ;
			book->get( "mean" + species[ iS ] )->SetBinContent( binsPt->findBin( ip ) + 1, tofMean );
			book->get( "sigma" + species[ iS ] )->SetBinContent( binsPt->findBin( ip ) + 1, tofSigma );
		}	
	}
	

}


void ParamSpectra::analyzeTrack( Int_t iTrack ){

	// first apply the phase space recentering
	
	//TODO not really correct
	double pt = pico->trackPt( iTrack );
	int ptBin = binsPt->findBin( pt );
	
	
	if ( ptBin < binsPt->length() - 1 ){
		double avgPt = (binsPt->bins[ ptBin ] + binsPt->bins[ ptBin + 1 ]) / 2.0;

		double dedx = psr->nlDedx( centerSpecies, pico->trackDedx( iTrack ), 
							pt, avgPt );
		double tof = psr->nlTof( centerSpecies, pico->trackBeta( iTrack ), 
							pt, avgPt );
		
		book->fill( "tof", tof ); 

		for ( int iS = 0; iS < species.size(); iS ++ ){
			
			double tofMean = tofParams[ iS ]->mean( pt, psr->mass( species[ iS ] ) ) ;
			double tofSigma = tofParams[ iS ]->sigma( pt, psr->mass( species[ iS ] ) );
			
			if ( 	tof >= tofMean - tofSigma * ( nSigmaTof / 2.0 ) && 
					tof <= tofMean + tofSigma * ( nSigmaTof / 2.0 ) ){
				//cout << species[ iS ] << " : " ;
				//cout << "\t" << tofMean << " +/- " << tofSigma << endl;
				
				book->fill( "pt" + species[ iS ], pt );

			}

		}

	}


}