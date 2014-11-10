#include "ParamSpectra.h"

/**
 * Root Includes
 */
#include "TLine.h"

/**
 * Static const definitions of the nSigma Cut Types
 */
string ParamSpectra::DEDX_CUT 		= "dedx";
string ParamSpectra::TOF_CUT 		= "tof";
string ParamSpectra::SQUARE_CUT 	= "square";
string ParamSpectra::ELLIPSE_CUT 	= "ellipse";



ParamSpectra::ParamSpectra( XmlConfig * config, string np, string fl, string jp ) 
	: InclusiveSpectra( config, np, fl, jp ) {

	species = cfg->getStringVector( np+"PidSpecies" );


	lg->info(__FUNCTION__) << "Setting up Tof Pid Params for : " << endl;

	// make the list of pid Params
	for ( int i = 0; i < species.size(); i++ ){

		lg->info(__FUNCTION__) << species[ i ] << endl;

		TofPidParams * tpp = new TofPidParams( cfg, np + "TofPidParams." + species[ i ] + "." );
		tofParams.push_back( tpp );

		DedxPidParams * dpp = new DedxPidParams( cfg, np + "DedxPidParams." + species[ i ] + "." );
		dedxParams.push_back( dpp );
	}


	// get the ideal plateau sigmas
	tofSigmaIdeal = cfg->getDouble( np+"PhaseSpaceRecentering.sigma:tof", 0.0012);
	dedxSigmaIdeal = cfg->getDouble( np+"PhaseSpaceRecentering.sigma:dedx", 0.033);

	// Initialize the Phase Space Recentering Object
	psr = new PhaseSpaceRecentering( dedxSigmaIdeal,
									 tofSigmaIdeal,
									 cfg->getString( np+"Bichsel.table", "dedxBichsel.root"),
									 cfg->getInt( np+"Bichsel.method", 0) );
	
	// get the recentering method for the phase space
	psrMethod = config->getString( np+"PhaseSpaceRecentering.method", "traditional" );
	
	// alias the centered species for ease of use
	centerSpecies = cfg->getString( np+"PhaseSpaceRecentering.centerSpecies", "K" );

	// get the Bins for pt and eta
	binsPt = new HistoBins( cfg, "binning.pt" );
	binsEta = new HistoBins( cfg, "binning.eta" );
	binsEta = new HistoBins( cfg, "binning.charge" );

	// initialize the pid cut
	nSigmaTof = cfg->getDouble( np+"nSigmaCut:tof", 1.0 );
	nSigmaDedx = cfg->getDouble( np+"nSigmaCut:dedx", 1.0 );
	
	// make a list of cuts we want to explore
	cuts = { TOF_CUT, DEDX_CUT, SQUARE_CUT, ELLIPSE_CUT};

	book->cd();
}

ParamSpectra::~ParamSpectra(){
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

				hName = histoForCentrality( centrals[ iC ], s, cuts[ iCut ], 1 );
				lg->info( __FUNCTION__ ) << hName << endl;
				book->clone( "ptBase", hName );

				hName = histoForCentrality( centrals[ iC ], s, cuts[ iCut ], -1 );
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

		book->clone( "dedxSigma", "dedxSigma" + species[ iS ] );

		for ( int iC = 0; iC < cuts.size(); iC ++ )
			book->clone( "pt", cuts[iC] + "Pt" + species[ iS ] );
	}

	for ( int iBin = 0; iBin < binsPt->size(); iBin++ ){
		book->clone( "tof", "tof_" + ts(iBin) );
		book->clone( "dedx", "dedx_" + ts(iBin) );
	}


	for ( double ip = .2; ip < 4; ip+=.025 ){
		for ( int iS = 0; iS < species.size(); iS ++ ){
			double tofMean = tofParams[ iS ]->mean( ip, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) ;
			double tofSigma = tofParams[ iS ]->sigma( ip, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) ;
			double dedxSigma = dedxParams[ iS ]->sigma( ip ) ;
			//if ( "Pi" == species[ iS ] )
			//	tofMean *= -1;
			book->get( "mean" + species[ iS ] )->SetBinContent( binsPt->findBin( ip ) + 1, tofMean );
			book->get( "sigma" + species[ iS ] )->SetBinContent( binsPt->findBin( ip ) + 1, tofSigma );
			book->get( "dedxSigma" + species[ iS ] )->SetBinContent( binsPt->findBin( ip ) + 1, dedxSigma );
		}	
	}
	

}

void ParamSpectra::postLoop() {

	gStyle->SetOptStat(0);
	book->cd();
	for (int i= 0; i < binsPt->nBins(); i++ ){
		
		double p = ( (*binsPt)[ i] + (*binsPt)[ i+1 ] )/2.0;
		lg->info( __FUNCTION__ ) << "Saving Bin " << i << endl;

		reporter->newPage();
		TH1D* h = (TH1D*)book->get( "tof_" + ts(i) );
		h->Sumw2();
		h->Scale( 1.0 / h->Integral() );
		h->SetTitle(  dts(binsPt->bins[i]).c_str()  );

		//gPad->SetLogy(1);

		//h->Draw( "pe" );
		book->style( "tof_" + ts(i) )->set( nodePath + "style.tof" )->draw();

		vector<double> tofMus = psr->centeredTofMeans( centerSpecies, p );
		for ( int iS = 0; iS < species.size(); iS ++ ){
			
			double tofMean = tofParams[ iS ]->mean( p, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) ;
			double tofSigma = tofParams[ iS ]->sigma( p, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) ;


			// if ( "Pi" == species[ iS ] && i < 1500){
			// 	tofMean = tofMus[ iS ];
			// }

			lg->info(__FUNCTION__) << species[ iS ] << "p = " << p << " mu = " << tofMean << endl;
			TLine * l1 = new TLine( tofMean - nSigmaTof*tofSigma , h->GetMinimum(), tofMean -nSigmaTof*tofSigma, h->GetMaximum() );
			l1->SetLineColor( iS+1 );
			l1->Draw();

			TLine * l2 = new TLine( tofMean + nSigmaTof*tofSigma, h->GetMinimum(), tofMean + nSigmaTof*tofSigma, h->GetMaximum() );
			l2->SetLineColor( iS+1 );
			l2->Draw();

		}
		
		reporter->savePage();

		vector<double> dedxMus = psr->centeredDedxMeans( centerSpecies, p );

		reporter->newPage();
		h = (TH1D*)book->get( "dedx_" + ts(i) );
		gPad->SetLogy(1);
		h->Draw();

		for ( int iS = 0; iS < species.size(); iS ++ ){
			
			lg->info(__FUNCTION__) << species[ iS ] << "p = " << p << " mu = " << dedxMus[iS] << endl;
			TLine * l1 = new TLine( dedxMus[ iS ] - nSigmaDedx*dedxSigmaIdeal , h->GetMinimum(), dedxMus[ iS ] -nSigmaDedx*dedxSigmaIdeal, h->GetMaximum() );
			l1->SetLineColor( iS+1 );
			l1->Draw();

			TLine * l2 = new TLine( dedxMus[ iS ] + nSigmaDedx*dedxSigmaIdeal, h->GetMinimum(), dedxMus[ iS ] + nSigmaDedx*dedxSigmaIdeal, h->GetMaximum() );
			l2->SetLineColor( iS+1 );
			l2->Draw();

		}


		reporter->savePage();


	}

}


void ParamSpectra::analyzeTrack( Int_t iTrack ){

	// first apply the phase space recentering
	
	//TODO not really correct
	double pt = pico->trackPt( iTrack );
	double eta = TMath::Abs( pico->trackEta( iTrack ) );
	double p = pico->trackP( iTrack );
	int ptBin = binsPt->findBin( pt );
	int etaBin = binsEta->findBin( TMath::Abs( eta ) );
	int charge = pico->trackCharge( iTrack  );
	
	Int_t refMult = pico->eventRefMult();
	
	/**
	 * For now skip other eta bins
	 */
	if ( ptBin < 0 || etaBin < 0)
		return;

	
	if ( ptBin < binsPt->nBins() ){
		double avgP = ( binsPt->bins[ ptBin ] + binsPt->bins[ ptBin+1] ) / 2.0;

		//double dedx = psr->nlDedx( centerSpecies, pico->trackDedx( iTrack ), 
		//					pt, avgPt );
		//double dedx = pico->trackDedx( iTrack);
		double dedx = psr->nlDedx( centerSpecies, pico->trackDedx( iTrack ), p, avgP );
		double tof = psr->nlTof( centerSpecies, pico->trackBeta( iTrack ), p, avgP );
		

		book->fill( "tof", tof ); 
		book->fill( "dedx", dedx );
		if ( ptBin >= 0 ){
			book->fill( "tof_"+ts(ptBin), tof ); 
			book->fill( "dedx_"+ts(ptBin), dedx );	
		}
		


		for (int iC = 0; iC < cuts.size(); iC++ ){
			vector<string> pidRes = pid( cuts[ iC ], p, dedx, tof, avgP );
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
					 	string hcName = histoForCentrality( cent, pidRes[ iP ], cuts[ iC ], charge );
					 	if ( 	refMult >= cutCentrality[ cent ]->min && refMult <= cutCentrality[ cent ]->max ){
					 		book->fill( hName, pt );
					 		book->fill( hcName, pt );
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
vector<string> ParamSpectra::pid( string type, double p, double dedx, double tof, double avgP ){

	if ( TOF_CUT == type )
		return pidTof( p, tof );
	else if ( SQUARE_CUT == type )
		return pidSquare( p, dedx, tof );
	else if ( ELLIPSE_CUT == type )
		return pidEllipse( p, dedx, tof, avgP );

	return pidDedx( p, dedx );

}

vector<string> ParamSpectra::pidTof( double p, double tof ){

	vector<string> res;
	for ( int iS = 0; iS < species.size(); iS ++ ){
			
		double tofMean = tofParams[ iS ]->mean( p, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) ;
		double tofSigma = tofParams[ iS ]->sigma( p, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) );

		if ( tof >= tofMean - tofSigma * nSigmaTof && tof <= tofMean + tofSigma * nSigmaTof ){
			res.push_back( species[ iS ] );
		}
	}

	return res;

}

vector<string> ParamSpectra::pidDedx( double p, double dedx ){

	vector<string> res;
	vector<double> means = psr->centeredDedxMeans( centerSpecies, p );
	for ( int iS = 0; iS < species.size(); iS ++ ){
		
		double rcDedx = psr->rDedx( species[ iS ], dedx, p );
		double sigma = dedxParams[ iS ]->sigma( p );

		if (	dedx >= means[ iS ] -sigma * nSigmaDedx && dedx <=  means[ iS ] + sigma * nSigmaDedx ){
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

vector<string> ParamSpectra::pidEllipse( double p, double dedx, double tof, double avgP ){

	vector<string> res;

	vector<double> dedxMeans = psr->centeredDedxMeans( centerSpecies, avgP );

	for ( int iS = 0; iS < species.size(); iS ++ ){
		
		double tofMean = tofParams[ iS ]->mean( avgP, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) ;
		double tofSigma = tofParams[ iS ]->sigma( avgP, psr->mass( species[ iS ]), psr->mass( centerSpecies ) );
		double dedxSigma = dedxParams[ iS ]->sigma( avgP );

		double rcDedx = dedx - dedxMeans[ iS ];
		double nsDedx = (rcDedx / dedxSigma );

		double rcTof = (tof - tofMean);
		double nsTof = (rcTof / tofSigma);

		double cut = TMath::Power((nsTof / nSigmaTof), 2) + TMath::Power((nsDedx / nSigmaDedx), 2);
		if ( cut <= 1 ){
			res.push_back( species[ iS ] );
		}
		

	}

	return res;
}







