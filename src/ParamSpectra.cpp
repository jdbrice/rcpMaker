#include "ParamSpectra.h"


/**
 * Static const definitions of the nSigma Cut Types
 */
string ParamSpectra::DEDX_CUT 		= "dedx";
string ParamSpectra::TOF_CUT 		= "tof";
string ParamSpectra::SQUARE_CUT 	= "square";
string ParamSpectra::ELLIPSE_CUT 	= "ellipse";


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
	
	cuts = { TOF_CUT, DEDX_CUT, SQUARE_CUT, ELLIPSE_CUT};



	book->cd();

}


void ParamSpectra::preLoop(){

	lg->info(__FUNCTION__) << endl;
	
	for ( int iS = 0; iS < species.size(); iS ++ ){
		book->clone( "mean", "mean" + species[ iS ] );
		book->clone( "sigma", "sigma" + species[ iS ] );

		for ( int iC = 0; iC < cuts.size(); iC ++ )
			book->clone( "pt", cuts[iC] + "Pt" + species[ iS ] );
	}

	for ( int iBin = 0; iBin < binsPt->size(); iBin++ ){
		book->clone( "tof", "tof_" + ts(iBin) );
		book->clone( "dedx", "dedx_" + ts(iBin) );
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
	double eta = pico->trackEta( iTrack );
	double p = pico->trackP( iTrack );
	int ptBin = binsPt->findBin( pt );
	
	/**
	 * For now skip other eta bins
	 */
	if ( binsEta->findBin( eta ) >= 1 )
		return;
	
	if ( ptBin < binsPt->length() - 1 ){
		double avgPt = (binsPt->bins[ ptBin ] + binsPt->bins[ ptBin + 1 ]) / 2.0;

		double dedx = psr->nlDedx( centerSpecies, pico->trackDedx( iTrack ), 
							pt, avgPt );
		double tof = psr->nlTof( centerSpecies, pico->trackBeta( iTrack ), 
							pt, avgPt );
		

		book->fill( "tof", tof ); 
		book->fill( "dedx", dedx );
		if ( ptBin >= 0 ){
			book->fill( "tof_"+ts(ptBin), tof ); 
			book->fill( "dedx_"+ts(ptBin), dedx );	
		}
		


		for (int iC = 0; iC < cuts.size(); iC++ ){
			string pidRes = pid( cuts[ iC ], p, dedx, tof );
			if ( "" == pidRes )
				book->fill( "pt", pt );
			else 
				book->fill( cuts[ iC ] + "Pt" + pidRes, pt );
		} 
		
	}


}



/**
 * PID Applications
 * @param  type One of the cut types {tof, dedx, square, ellipse}
 * @param  p    momentum [GeV]
 * @param  dedx dedx raw nl
 * @param  tof  tof raw nl
 * @return      string for pid or "" if no pid
 */
string ParamSpectra::pid( string type, double p, double dedx, double tof ){

	if ( TOF_CUT == type )
		return pidTof( p, tof );
	else if ( SQUARE_CUT == type )
		return pidSquare( p, dedx, tof );
	else if ( ELLIPSE_CUT == type )
		return pidEllipse( p, dedx, tof );

	return pidDedx( p, dedx );

}

string ParamSpectra::pidTof( double p, double tof ){

	for ( int iS = 0; iS < species.size(); iS ++ ){
			
		double tofMean = tofParams[ iS ]->mean( p, psr->mass( species[ iS ] ) ) ;
		double tofSigma = tofParams[ iS ]->sigma( p, psr->mass( species[ iS ] ) );
		
		if ( 	tof >= tofMean - tofSigma * ( nSigmaTof / 2.0 ) && 
				tof <= tofMean + tofSigma * ( nSigmaTof / 2.0 ) ){
			return species[ iS ];
		}

	}

	return "";

}

string ParamSpectra::pidDedx( double p, double dedx ){

	for ( int iS = 0; iS < species.size(); iS ++ ){
			
		double mean = psr->dedxGenerator()->mean10( p, psr->mass( species[iS]), -1, 1000 ) ;
		double sigma = 0.06;
		
		if ( 	dedx >= mean - sigma * ( nSigmaDedx / 2.0 ) && 
				dedx <= mean + sigma * ( nSigmaDedx / 2.0 ) ){
			return species[ iS ];
		}

	}

	return "";

}

string ParamSpectra::pidSquare( double p, double dedx, double tof ){

	string tofPid = pidTof( p, tof );
	string dedxPid = pidDedx( p, dedx );

	if ( tofPid == dedxPid )
		return tofPid;

	return "";
}

string ParamSpectra::pidEllipse( double p, double dedx, double tof ){

	string tofPid = pidTof( p, tof );
	string dedxPid = pidDedx( p, dedx );

	if ( tofPid == dedxPid )
		return tofPid;

	return "";
}








