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

void ParamSpectra::makeCentralityHistos()  {
	book->cd();
	lg->info(__FUNCTION__) << endl;
	/**
	 * Make centrality ptHistos
	 */
	for (int iCut = 0; iCut < cuts.size(); iCut++ ){
		for ( int iS = 0; iS < species.size(); iS ++ ){
			string s = species[ iS ];
			for ( int iC = 0; iC < centrals.size(); iC ++ ){
				string hName = histoForCentrality( centrals[ iC ], s, cuts[ iCut ] );
				lg->info( __FUNCTION__ ) << hName << endl;
				book->clone( "ptBase", hName );
			} //centralities
		} // pid species
	} // pid Cuts
	
}


void ParamSpectra::preLoop(){
	InclusiveSpectra::preLoop();
	
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
	Int_t refMult = pico->eventRefMult();
	
	/**
	 * For now skip other eta bins
	 */
	if ( binsEta->findBin( eta ) >= 1 )
		return;
	
	if ( ptBin < binsPt->length() - 1 ){
		double avgPt = (binsPt->bins[ ptBin ] + binsPt->bins[ ptBin + 1 ]) / 2.0;

		//double dedx = psr->nlDedx( centerSpecies, pico->trackDedx( iTrack ), 
		//					pt, avgPt );
		double dedx = pico->trackDedx( iTrack);
		double tof = psr->nlTof( centerSpecies, pico->trackBeta( iTrack ), 
							pt, avgPt );
		

		book->fill( "tof", tof ); 
		//book->fill( "dedx", dedx );
		if ( ptBin >= 0 ){
			book->fill( "tof_"+ts(ptBin), tof ); 
			book->fill( "dedx_"+ts(ptBin), TMath::Log10(dedx) );	
		}
		


		for (int iC = 0; iC < cuts.size(); iC++ ){
			vector<string> pidRes = pid( cuts[ iC ], p, dedx, tof );
			if ( 0 == pidRes.size() )
				book->fill( "pt", pt );
			else {
				for ( int iP = 0; iP < pidRes.size(); iP++ ){
					book->fill( cuts[ iC ] + "Pt" + pidRes[ iP ], pt );


					/**
					 * Centrality cuts
					 */
					 for ( int iCent = 0; iCent < centrals.size(); iCent++ ){
					 	string cent = centrals[ iCent ];
					 	//lg->info( __FUNCTION__ ) << book->get( histoForCentrality( cent, "K" ) )<< endl;
					 	string hName = histoForCentrality( cent, pidRes[ iP ], cuts[ iC ] );
					 	if ( 	refMult >= cutCentrality[ cent ]->min && refMult <= cutCentrality[ cent ]->max ){
					 		book->fill( hName, pt );
					 	}
					 } // loop on centrality cuts
				} // loop in Pid results
			} // Pid exists for this track
		} // loop on cuts
		
	} // ptBin in range


}



/**
 * PID Applications
 * @param  type One of the cut types {tof, dedx, square, ellipse}
 * @param  p    momentum [GeV]
 * @param  dedx dedx raw nl
 * @param  tof  tof raw nl
 * @return      string for pid or "" if no pid
 */
vector<string> ParamSpectra::pid( string type, double p, double dedx, double tof ){

	if ( TOF_CUT == type )
		return pidTof( p, tof );
	else if ( SQUARE_CUT == type )
		return pidSquare( p, dedx, tof );
	else if ( ELLIPSE_CUT == type )
		return pidEllipse( p, dedx, tof );

	return pidDedx( p, dedx );

}

vector<string> ParamSpectra::pidTof( double p, double tof ){

	vector<string> res;
	for ( int iS = 0; iS < species.size(); iS ++ ){
			
		double tofMean = tofParams[ iS ]->mean( p, psr->mass( species[ iS ] ) ) ;
		double tofSigma = tofParams[ iS ]->sigma( p, psr->mass( species[ iS ] ) );
		
		if ( 	tof >= tofMean - tofSigma * nSigmaTof && tof <= tofMean + tofSigma * nSigmaTof ){
			res.push_back( species[ iS ] );
		}
		/*double km = psr->tofGenerator()->mean( p, psr->mass( centerSpecies ) );
		double m = psr->tofGenerator()->mean( p, psr->mass( species[ iS ] ) );
		double rcTof = (tof + km  - m) / 0.0012;
		if (  rcTof <= nSigmaTof && rcTof >= -nSigmaTof )
			res.push_back( species[ iS ]);
		*/
	}

	return res;

}

vector<string> ParamSpectra::pidDedx( double p, double dedx ){

	vector<string> res;
	for ( int iS = 0; iS < species.size(); iS ++ ){
		
		double rcDedx = psr->rDedx( species[ iS ], dedx, p );
		double sigma = 0.06;

		if ( 	rcDedx >= -sigma * nSigmaDedx && rcDedx <=  sigma * nSigmaDedx ){
			res.push_back( species[ iS ] );
		}

	}

	return res;

}

vector<string> ParamSpectra::pidSquare( double p, double dedx, double tof ){

	vector<string> tid = pidTof( p, tof );
	vector<string> did = pidDedx( p, dedx );

	vector<string> res;
	for ( int i = 0; i < did.size(); i++ ){
		if ( tid.end() != find( tid.begin(), tid.end(), did[i] ) )
			res.push_back( did[ i ] ); 
	} 
	return res;
}

vector<string> ParamSpectra::pidEllipse( double p, double dedx, double tof ){

	vector<string> res;
	for ( int iS = 0; iS < species.size(); iS ++ ){
		
		double tofMean = tofParams[ iS ]->mean( p, psr->mass( species[ iS ] ) ) ;
		double tofSigma = tofParams[ iS ]->sigma( p, psr->mass( species[ iS ] ) );

		double rcDedx = psr->rDedx( species[ iS ], dedx, p );
		double sigma = 0.06;
		double nsDedx = (rcDedx / sigma );

		double rcTof = (tof - tofMean);
		double nsTof = (rcTof / tofSigma);

		double cut = TMath::Power((nsTof / nSigmaTof), 2) + TMath::Power((nsDedx / nSigmaDedx), 2);
		if ( cut <= 1 ){
			//lg->info(__FUNCTION__) << species[ iS ] << " : " << cut << endl;
			res.push_back( species[ iS ] );
		}
		//lg->info( __FUNCTION__ ) << "nSig dEdx = " << nSigDedx << ", nSig Tof = " << nSigTof << endl; 

	}

	return res;
}







