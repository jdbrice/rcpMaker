#include "PidParamMaker.h"

#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooConstVar.h"
#include "RooProdPdf.h"
#include "RooAbsReal.h"
#include "RooAddPdf.h"
#include "RooDataHist.h"
#include "RooConstVar.h"
#include "RooPlot.h"

#include "PidPhaseSpace.h"

PidParamMaker::PidParamMaker( XmlConfig * config, string np ){
	//Set the Root Output Level
	gErrorIgnoreLevel=kSysError;

	// Save Class Members
	cfg = config;
	nodePath = np;

	// make the Logger
	lg = LoggerConfig::makeLogger( cfg, np + "Logger" );

	lg->info(__FUNCTION__) << "Got config with nodePath = " << np << endl;

    // create the book
    lg->info(__FUNCTION__) << " Creating book "<< endl;
    book = new HistoBook( config->getString( np + "output.data" ), config );

    lg->info(__FUNCTION__) << " Creating reporter "<< endl;
    reporter = new Reporter( config->getString( np + "output.report" ) );


    lg->info(__FUNCTION__) << " Creating PhaseSpaceRecentering "<< endl;
    // Initialize the Phase Space Recentering Object
	tofSigmaIdeal = cfg->getDouble( np+"PhaseSpaceRecentering.sigma:tof", 0.0012);
	dedxSigmaIdeal = cfg->getDouble( np+"PhaseSpaceRecentering.sigma:dedx", 0.06);
	psr = new PhaseSpaceRecentering( dedxSigmaIdeal,
									 tofSigmaIdeal,
									 cfg->getString( np+"Bichsel.table", "dedxBichsel.root"),
									 cfg->getInt( np+"Bichsel.method", 0) );
	psrMethod = config->getString( np+"PhaseSpaceRecentering.method", "traditional" );

	lg->info(__FUNCTION__) << " Getting Center Species "<< endl;
	// alias the centered species for ease of use
	centerSpecies = cfg->getString( np+"PhaseSpaceRecentering.centerSpecies", "K" );

	lg->info(__FUNCTION__) << " Opening input "<< endl;
	inFile = new TFile( cfg->getString( np+"input.data:url" ).c_str(), "READ" );

	/**
	 * Make the momentum transverse, eta, charge binning
	 */
	lg->info(__FUNCTION__) << " Creating pt, eta, charge binning "<< endl;
	binsPt = new HistoBins( cfg, "binning.pt" );
	binsEta = new HistoBins( cfg, "binning.eta" );
	binsCharge = new HistoBins( cfg, "binning.charge" );

	muRoi 		= 2.5;
	sigmaRoi 	= 3.5;
	roi 		= 2.5;
	lg->info(__FUNCTION__) << " Setup Complete "<< endl;
}

PidParamMaker::~PidParamMaker(){
	delete book;
	delete reporter;
	delete psr;
	delete lg;
	delete binsPt;
	delete binsEta;
	delete binsCharge;
}


void PidParamMaker::make(){
	lg->info(__FUNCTION__) << endl;

	book->cd();
	book->makeAll( nodePath + "histograms" );
	vector<string> species = PidPhaseSpace::species;

	/**
	 * Less than lovely mathematica export
	 */
	vector<string> mdMu;
	vector<string> mdPlotMu;
	vector<string> mdSigma;
	vector<string> mdPlotSigma;

	vector<string> dMu;
	vector<string> dPlotMu;
	vector<string> dSigma;
	vector<string> dPlotSigma;
	for ( int iS = 0; iS < species.size(); iS ++ ){

		// For tof
		string mName = "tofMean" + species[iS];
		string sName = "tofSigma" + species[iS];
		book->clone( "mean", mName );
		book->clone( "sigma", sName );

		mdMu.push_back( (mName + " = { ") );
		mdSigma.push_back( sName + " = { ");
		mdPlotMu.push_back( (mName + "Error = { ") );
		mdPlotSigma.push_back( sName + "Error = { ");

		// For Dedx
		mName = "dedxMean" + species[iS];
		sName = "dedxSigma" + species[iS];
		book->clone( "mean", mName );
		book->clone( "sigma", sName );

		dMu.push_back( (mName + " = { ") );
		dSigma.push_back( sName + " = { ");
		dPlotMu.push_back( (mName + "Error = { ") );
		dPlotSigma.push_back( sName + "Error = { ");
	}

	int centralityBin = 1;
	lg->info(__FUNCTION__) << "Starting fit loop "<< endl;

		
	for ( int i = 0; i < binsPt->nBins(); i++ ){

			double avgP = ((*binsPt)[i] + (*binsPt)[i+1] ) / 2.0;

			vector<double> tofMus = psr->centeredTofMeans( centerSpecies, avgP );
			vector<double> dedxMus = psr->centeredDedxMeans( centerSpecies, avgP );

		for ( int iS = 0; iS < species.size(); iS ++ ){
			string mName = "tofMean" + species[iS];
			string sName = "tofSigma" + species[iS];

			string name = "tof/" + PidPhaseSpace::tofName( centerSpecies, 0, centralityBin, i, 0, species[ iS ] );
			TH1D* tofAll = (TH1D*)inFile->Get( name.c_str() );
		
			lg->info(__FUNCTION__) << "tofAll " << tofAll <<  endl;

			GaussianFitResult tofGfr =  fitSingleSpecies( tofAll, tofMus[ iS ], tofSigmaIdeal );

			book->get( mName )->SetBinContent( i, tofGfr.mu );
			book->get( sName )->SetBinContent( i, tofGfr.sigma );
			book->get( mName )->SetBinError( i, tofGfr.muError );
			book->get( sName )->SetBinError( i, tofGfr.sigmaError );	

			mdMu[ iS ] += ("{ " + dts(avgP*1000) + ", " + dts(tofGfr.mu) + " }" );
			mdPlotMu[ iS ] += ("{{ " + dts(avgP*1000) + ", " + dts(tofGfr.mu) + " }, ErrorBar[" + dts( tofGfr.muError ) + " ] }" );
			if ( i != binsPt->nBins() - 1 )
				mdMu[ iS ] += ", ";
			if ( i != binsPt->nBins() - 1 )
				mdPlotMu[ iS ] += ", ";

			mdSigma[ iS ] += ("{ " + dts(avgP*1000) + ", " + dts(tofGfr.sigma) + " }" );
			if ( i != binsPt->nBins() - 1 )
				mdSigma[ iS ] += ", ";
			mdPlotSigma[ iS ] += ("{{ " + dts(avgP*1000) + ", " + dts(tofGfr.sigma) + " }, ErrorBar[" + dts( tofGfr.sigmaError ) + " ] }" );
			if ( i != binsPt->nBins() - 1 )
				mdPlotSigma[ iS ] += ", ";

			/**
			 * Fit dedx now
			 */
			mName = "dedxMean" + species[iS];
			sName = "dedxSigma" + species[iS];

			name = "dedx/" + PidPhaseSpace::dedxName( centerSpecies, 0, centralityBin, i, 0, species[ iS ] );
			TH1D* hDedx = (TH1D*)inFile->Get( name.c_str() );
		
			GaussianFitResult dedxGfr =  fitSingleSpecies( hDedx, dedxMus[ iS ], dedxSigmaIdeal );

			book->get( mName )->SetBinContent( i, dedxGfr.mu );
			book->get( sName )->SetBinContent( i, dedxGfr.sigma );
			book->get( mName )->SetBinError( i, dedxGfr.muError );
			book->get( sName )->SetBinError( i, dedxGfr.sigmaError );

			dMu[ iS ] += ("{ " + dts(avgP*1000) + ", " + ts( dedxGfr.mu ) + " }" );
			dPlotMu[ iS ] += ("{{ " + dts(avgP*1000) + ", " + dts(dedxGfr.mu) + " }, ErrorBar[" + dts( dedxGfr.muError ) + " ] }" );
			if ( i != binsPt->nBins() - 1 )
				dMu[ iS ] += ", ";
			if ( i != binsPt->nBins() - 1 )
				dPlotMu[ iS ] += ", ";

			dSigma[ iS ] += ("{ " + dts(avgP*1000) + ", " + dts(dedxGfr.sigma) + " }" );
			if ( i != binsPt->nBins() - 1 )
				dSigma[ iS ] += ", ";
			dPlotSigma[ iS ] += ("{{ " + dts(avgP*1000) + ", " + dts(dedxGfr.sigma) + " }, ErrorBar[" + dts( dedxGfr.sigmaError ) + " ] }" );
			if ( i != binsPt->nBins() - 1 )
				dPlotSigma[ iS ] += ", ";	
			
		}
		
		
		
	}

	for ( int iS = 0; iS < species.size(); iS++ ){
		mdMu[ iS ] += " };";
		mdPlotMu[ iS ] += " };";
		mdSigma[ iS ] += " };";
		mdPlotSigma[ iS ] += " };";
		cout << mdMu[ iS ] << endl << endl << endl;
		cout << mdPlotMu[ iS ] << endl << endl << endl;
		cout << mdSigma[ iS ] << endl << endl << endl;
		cout << mdPlotSigma[ iS ] << endl << endl << endl;

		dMu[ iS ] += " };";
		dPlotMu[ iS ] += " };";
		dSigma[ iS ] += " };";
		dPlotSigma[ iS ] += " };";

		cout << dMu[ iS ] << endl << endl << endl;
		cout << dPlotMu[ iS ] << endl << endl << endl;
		cout << dSigma[ iS ] << endl << endl << endl;
		cout << dPlotSigma[ iS ] << endl << endl << endl;
	}

	

}

PidParamMaker::GaussianFitResult PidParamMaker::fitSingleSpecies( TH1D* h, double iMu, double iSigma ){

	using namespace RooFit;
	RooMsgService::instance().setGlobalKillBelow(ERROR);

	// get some histogram limits
	double x1 = h->GetXaxis()->GetXmin();
	double x2 = h->GetXaxis()->GetXmax();
	
	/**
	 * Make the data hist for RooFit
	 */
	RooRealVar x( "x", "x", iMu - 5 * roi * iSigma, iMu + 5 * roi * iSigma );
	RooDataHist * rdh = new RooDataHist( "data", "data", RooArgSet( x ), h  );

	/**
	 * Create the parameters and the gaussian
	 */
	RooRealVar mu( "mu", "mu", iMu, iMu - muRoi * iSigma, iMu + muRoi * iSigma );
	RooRealVar sigma( "sig", "sig", iSigma, iSigma / sigmaRoi, sigmaRoi * iSigma );
	RooGaussian gauss( "gauss", "gauss", x, mu, sigma );

	RooRealVar emu( "emu", "emu", iMu, iMu - muRoi * iSigma, iMu + muRoi * iSigma );
	RooRealVar esigma( "esig", "esig", iSigma, iSigma / sigmaRoi, sigmaRoi * iSigma );
	RooGaussian egauss( "egauss", "egauss", x, emu, esigma );

	/**
	 * Set the region of interest for fitting
	 */
	x.setRange( "roi", iMu - roi * iSigma, iMu + roi * iSigma );
	gauss.fitTo( *rdh, Range( "roi" ), RooFit::PrintLevel(-1) );

	x.setRange( "roiError", iMu - 2.0*roi * iSigma, iMu + 2.0* roi * iSigma );
	egauss.fitTo( *rdh, Range( "roiError" ), RooFit::PrintLevel(-1) );

	/**
	 * Report the data + gauss
	 */
	reporter->newPage();

	RooPlot * frame = x.frame( Title( h->GetTitle() ) );
	rdh->plotOn( frame );
	gauss.plotOn( frame );
	egauss.plotOn( frame, LineColor(kRed) );
	
	gPad->SetLogy();
	frame->Draw();
	frame->SetMinimum( 1 );

	reporter->savePage();

	GaussianFitResult gfr;

	gfr.mu = mu.getVal();
	double mDiff = (emu.getVal() - mu.getVal());
	double sDiff = (esigma.getVal() - sigma.getVal());
	double eSig = TMath::Sqrt( sDiff*sDiff + sigma.getError()*sigma.getError() );
	gfr.muError = TMath::Sqrt( mDiff*mDiff + mu.getError()*mu.getError() );//mu.getError();
	gfr.sigma = sigma.getVal();
	gfr.sigmaError = eSig;

	return gfr;

}








