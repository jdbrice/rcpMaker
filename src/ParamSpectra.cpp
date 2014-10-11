#include "ParamSpectra.h"

ParamSpectra::ParamSpectra( XmlConfig * config, string np) 
	: InclusiveSpectra( config, np ) {

	species = cfg->getStringVector( np+"PidSpecies" );

	lg->info(__FUNCTION__) << "Setting up Pid Params for : " << endl;
	// make the list of pidParams
	for ( int i = 0; i < species.size(); i++ ){

		lg->info(__FUNCTION__) << species[ i ] << endl;

		PidParams * pp = new PidParams( cfg, np + "PidParams." + species[ i ] + "." );
		pidParams.push_back( pp );
	}

	// Initialize the Phase Space Recentering Object
	psr = new PhaseSpaceRecentering( cfg->getDouble( np+"PhaseSpaceRecentering.sigma:dedx", 0.06),
									 cfg->getDouble( np+"PhaseSpaceRecentering.sigma:tof", 0.0012),
									 cfg->getString( np+"Bichsel.table", "dedxBichsel.root"),
									 cfg->getInt( np+"Bichsel.method", 0) );
	centerSpecies = cfg->getString( np+"PhaseSpaceRecentering.centerSpecies", "K" );

	binsPt = new HistoBins( cfg, "binning.pt" );
	binsEta = new HistoBins( cfg, "binning.eta" );

}


void ParamSpectra::analyzeTrack( Int_t iTrack ){

	// first apply the phase space recentering
	double pt = pico->trackPt( iTrack );
	int ptBin = binsPt->findBin( pt );
	cout << "Pt Bin For " << pt << " = " << ptBin << endl;

	//lg->(__FUNCTION__) << " Mean[ " << 


}