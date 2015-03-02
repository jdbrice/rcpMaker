
#include "SimultaneousPid.h"


#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooProdPdf.h"
#include "RooAbsReal.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "RooDataHist.h"
#include "RooConstVar.h"
#include "RooPlot.h"
#include "RooCategory.h"
#include "RooSimultaneous.h"

#include "PidPhaseSpace.h"


SimultaneousPid::SimultaneousPid( XmlConfig * config, string np ) : HistoAnalyzer( config, np ){
	
  	// Initialize the Phase Space Recentering Object
	tofSigmaIdeal = cfg->getDouble( np+"PhaseSpaceRecentering.sigma:tof", 0.0012);
	dedxSigmaIdeal = cfg->getDouble( np+"PhaseSpaceRecentering.sigma:dedx", 0.06);
	psr = new PhaseSpaceRecentering( dedxSigmaIdeal,
									 tofSigmaIdeal,
									 cfg->getString( np+"Bichsel.table", "dedxBichsel.root"),
									 cfg->getInt( np+"Bichsel.method", 0) );

	psrMethod = config->getString( np+"PhaseSpaceRecentering.method", "traditional" );
	// alias the centered species for ease of use
	centerSpecies = cfg->getString( np+"PhaseSpaceRecentering.centerSpecies", "K" );

	/**
	 * Make the momentum transverse, eta, charge binning
	 */
	binsPt = new HistoBins( cfg, "binning.pt" );
	binsEta = new HistoBins( cfg, "binning.eta" );
	binsCharge = new HistoBins( cfg, "binning.charge" );

	/**
	 * Setup the PID Params
	 */
	vector<string> species = { "Pi", "K", "P" };
	for ( int i = 0; i < species.size(); i++ ){

		logger->info(__FUNCTION__) << species[ i ] << endl;

		TofPidParams * tpp = new TofPidParams( cfg, np + "TofPidParams." + species[ i ] + "." );
		tofParams.push_back( tpp );

		DedxPidParams * dpp = new DedxPidParams( cfg, np + "DedxPidParams." + species[ i ] + "." );
		dedxParams.push_back( dpp );
	}

}

SimultaneousPid::~SimultaneousPid(){

}

void SimultaneousPid::make(){


	int charge = -1;
	logger->info( __FUNCTION__ ) << endl;
	vector<string> species = PidPhaseSpace::species;

	for ( int i = 1; i < binsPt->nBins(); i++ ){

		double avgP = ((*binsPt)[i] + (*binsPt)[i+1] ) / 2.0;

		vector<double> tofMus = psr->centeredTofMeans( centerSpecies, avgP );
		vector<double> dedxMus = psr->centeredDedxMeans( centerSpecies, avgP );
		vector<double> tofSigmas;
		vector<double> dedxSigmas;

		for ( int iS = 0; iS < species.size(); iS ++ ){
			tofSigmas.push_back( tofParams[ iS ]->sigma( avgP, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) );
			dedxSigmas.push_back( dedxParams[ iS ]->sigma( avgP ) );
		}
		
		// get the histo paths
		TH1D* tofAll = (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( centerSpecies, charge, "0to5", i, 0 )).c_str() );
		TH1D* tofPi = (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( centerSpecies, charge, "0to5", i, 0, "Pi" )).c_str() );
		TH1D* tofK = (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( centerSpecies, charge, "0to5", i, 0, "K" )).c_str() );
		TH1D* tofP = (TH1D*)inFile->Get( ("tof/" + PidPhaseSpace::tofName( centerSpecies, charge, "0to5", i, 0, "P" )).c_str() );
		
		TH1D* dedxAll = (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( centerSpecies, charge, "0to5", i, 0 )).c_str() );
		TH1D* dedxPi = (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( centerSpecies, charge, "0to5", i, 0, "Pi" )).c_str() );
		TH1D* dedxK = (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( centerSpecies, charge, "0to5", i, 0, "K" )).c_str() );
		TH1D* dedxP = (TH1D*)inFile->Get( ("dedx/" + PidPhaseSpace::dedxName( centerSpecies, charge, "0to5", i, 0, "P" )).c_str() );

		reporter->newPage( 2, 2 );
		gPad->SetLogy(1);
		tofAll->Draw();
		reporter->next();
		gPad->SetLogy(1);
		tofPi->Draw();
		reporter->next();
		gPad->SetLogy(1);
		tofK->Draw();
		reporter->next();
		gPad->SetLogy(1);
		tofP->Draw();
		reporter->savePage();

		reporter->newPage( 2, 2 );
		gPad->SetLogy(1);
		dedxAll->Draw();
		reporter->next();
		gPad->SetLogy(1);
		dedxPi->Draw();
		reporter->next();
		gPad->SetLogy(1);
		dedxK->Draw();
		reporter->next();
		gPad->SetLogy(1);
		dedxP->Draw();
		reporter->savePage();

		RooRealVar x( "x", "x", -5, 5 );

		RooAddPdf * model1 = (RooAddPdf*)gaussModel( 3, &x, "tof" );
		RooAddPdf * model2 = (RooAddPdf*)gaussModel( 3, &x, "dedx" );

		model1->getParameters( x )->Print();
		model2->getParameters( x )->Print();
		

	}

	return;

}


// need to add the ability to create model with shared components for sim fitting
RooAbsPdf* SimultaneousPid::gaussModel( int nGauss, RooRealVar * x, string prefix ){

	logger->info(__FUNCTION__) << x << endl;
	if ( NULL == x ){
		logger->error(__FUNCTION__) << "No indep variable x given" << endl;
		return 0;
	}
	x->Print();

	double x1 = x->getMin();
	double x2 = x->getMax();

	RooArgList pdfList, yieldList;
	for ( int i = 0; i < nGauss; i++ ){

		RooRealVar *mu = new RooRealVar( 	(prefix+"_mu"+ts(i)).c_str(), 
											"mu", 0, x1, x2 );
		RooRealVar *sig = new RooRealVar( 	(prefix+"_sig"+ts(i)).c_str(), 
											"sig", 1, 0.0, 10 );
		RooGaussian *g = new RooGaussian( 	(prefix+"_gauss"+ts(i)).c_str(), 
											"gaussian", *x, *mu, *sig);


		pdfList.add( *g );

		RooRealVar *yield = new RooRealVar( (prefix+"_yield"+ts(i)).c_str(), 
											"yield", 1, 0, 10 );

		yieldList.add( *yield ); 

	}

	RooAddPdf * model = new RooAddPdf( (prefix+"_model").c_str(), "multi gauss model", pdfList, yieldList );

	return model;

}





