#include "SimultaneousGaussians.h"

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

SimultaneousGaussians::SimultaneousGaussians( XmlConfig * config, string np ){
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

    reporter = new Reporter( config->getString( np + "output.report" ) );


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


	inFile = new TFile( cfg->getString( np+"input.data:url" ).c_str(), "READ" );

	/**
	 * Make the momentum transverse, eta, charge binning
	 */
	binsPt = new HistoBins( cfg, "binning.pt" );
	binsEta = new HistoBins( cfg, "binning.eta" );
	binsCharge = new HistoBins( cfg, "binning.charge" );

	muRoi 		= 2.5;
	sigmaRoi 	= 3.5;
	roi 		= 2.5;
}

SimultaneousGaussians::~SimultaneousGaussians(){
	delete book;
	delete reporter;
	delete psr;
	delete lg;
	delete binsPt;
	delete binsEta;
	delete binsCharge;
}


void SimultaneousGaussians::make(){

	book->cd();
	book->makeAll( nodePath + "histograms" );
	vector<string> species = PidPhaseSpace::species;
		
	for ( int i = 0; i < binsPt->nBins(); i++ ){

			double avgP = ((*binsPt)[i] + (*binsPt)[i+1] ) / 2.0;

			vector<double> tofMus = psr->centeredTofMeans( centerSpecies, avgP );
			vector<double> dedxMus = psr->centeredDedxMeans( centerSpecies, avgP );

			vector<double> tofSigmas = {0.012, 0.012, 0.012};

			if ( (*binsPt)[ i ] > 1.2 ){
				sigmaRoi = 1.0;
				muRoi = .1;
			}
		for ( int iS = 0; iS < species.size(); iS ++ ){
			string mName = "tofMean" + species[iS];
			string sName = "tofSigma" + species[iS];

			string name = "tof/" + PidPhaseSpace::tofName( centerSpecies, 0, i, 0 );
			TH1D* tofAll = (TH1D*)inFile->Get( name.c_str() );
		
			GaussianFitResult tofGfr =  fitThreeSpecies( tofAll, tofMus, tofSigmas );

			/*book->get( mName )->SetBinContent( i, tofGfr.mu );
			book->get( sName )->SetBinContent( i, tofGfr.sigma );
			book->get( mName )->SetBinError( i, tofGfr.muError );
			book->get( sName )->SetBinError( i, tofGfr.sigmaError );	
			*/
		}
		
	}

	

}



SimultaneousGaussians::GaussianFitResult SimultaneousGaussians::fitThreeSpecies( TH1D* h, vector<double> iMu, vector<double> iSigma ){

	using namespace RooFit;
	RooMsgService::instance().setGlobalKillBelow(ERROR);

	// get some histogram limits
	double x1 = h->GetXaxis()->GetXmin();
	double x2 = h->GetXaxis()->GetXmax();
	
	/**
	 * Make the data hist for RooFit
	 */
	RooRealVar x( "x", "x", x1, x2 );
	RooDataHist * rdh = new RooDataHist( "data", "data", RooArgSet( x ), h  );

	/**
	 * Create the parameters and the gaussian
	 */
	

	/**
	 * Set the region of interest for fitting
	 */
	//x.setRange( "roi", iMu - roi * iSigma, iMu + roi * iSigma );
	




	// build a 3 gaussian model
	vector<RooRealVar*> mean;
	vector<RooRealVar*> sigma;
	vector<RooRealVar> yield;
	vector<RooGaussian> gauss;

	RooArgList *ralGauss = new RooArgList();
	RooArgList *ralYield = new RooArgList();

	vector<string> species = PidPhaseSpace::species;
	
	for ( int i = 0; i < species.size(); i++ ){
		lg->info( __FUNCTION__ ) << " Generating Gaussian for : " << species[ i ] << endl;
		lg->info( __FUNCTION__ ) << species[ i ] << " : " << iMu[ i ] << endl;

		RooRealVar * m = new RooRealVar( 	("mu" + species[ i ]).c_str(), ("mu" + species[ i ]).c_str(), 
											iMu[ i ], iMu[ i ] - muRoi * iSigma[ i ], iMu[ i ] + muRoi * iSigma[ i ] );
		mean.push_back( m );


		RooRealVar * s = new RooRealVar( 	("sig" + species[ i ]).c_str(), ("sig" + species[ i ]).c_str(), 
										iSigma[ i ], iSigma[ i ] / sigmaRoi, iSigma[ i ] * sigmaRoi );
		sigma.push_back( s );
		

		RooRealVar * y = new RooRealVar( 	("yield" + species[ i ]).c_str(), ("yield" + species[ i ]).c_str(), 
										80000, 100, 1000000000000 );

		RooGaussian * g = new RooGaussian( 	("gauss" + species[ i ]).c_str(), ("gauss" + species[ i ]).c_str(), 
										x, *m, *s );


		
		ralYield->add( *y );
		ralGauss->add( *g );

		// set the cut Range
		x.setRange( ("roi"+species[ i ]).c_str(), iMu[ i ] - iSigma[i] * roi, iMu[ i ] + iSigma[ i ] * roi  );


	}

	RooAddPdf *model = new RooAddPdf( "model", "model", *ralGauss, *ralYield ); 
	model->fitTo( *rdh );

	/**
	 * Report the data + gauss
	 */
	reporter->newPage();

	RooPlot * frame = x.frame( Title( h->GetTitle() ) );
	rdh->plotOn( frame );
	
	model->plotOn( frame, Range( "Full" ) );
	//model->plotOn( frame, Range( "Full" ), Components( "gaussK" ), LineColor( kGreen ) );
	//model->plotOn( frame, Range( "Full" ), Components( "gaussPi" ), LineColor( kRed ) );
	//model->plotOn( frame, Range( "Full" ), Components( "gaussP" ), LineColor( kBlack ) );
	
	
	gPad->SetLogy();
	frame->Draw();
	frame->SetMinimum( 1 );

	reporter->savePage();

	GaussianFitResult gfr;


	return gfr;

}








