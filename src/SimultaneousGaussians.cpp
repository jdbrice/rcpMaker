#include "SimultaneousGaussians.h"

#include "RooRealVar.h"
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

    reporter = new Reporter( config->getString( np + "output.report" ), 400, 400 );


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

	/*
	muRoi 		= 1.5;
	sigmaRoi 	= 1.5;
	roi 		= 2.5;
	*/

	muRoi 		= 1.5;
	sigmaRoi 	= 1.5;
	roi 		= 4.5;

	vector<string> species = { "Pi", "K", "P" };
	for ( int i = 0; i < species.size(); i++ ){

		lg->info(__FUNCTION__) << species[ i ] << endl;

		TofPidParams * tpp = new TofPidParams( cfg, np + "TofPidParams." + species[ i ] + "." );
		tofParams.push_back( tpp );

		DedxPidParams * dpp = new DedxPidParams( cfg, np + "DedxPidParams." + species[ i ] + "." );
		dedxParams.push_back( dpp );


	}

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

	vector<double> yields = { 180000, 100, 10};

	for ( int iS = 0; iS < species.size(); iS ++ ){
		book->clone( "yield", "yield_n_cen_"+species[ iS ] );
		book->clone( "yield", "yield_n_per_"+species[ iS ] );

		book->clone( "yield", "yield_p_cen_"+species[ iS ] );
		book->clone( "yield", "yield_p_per_"+species[ iS ] );
	}
		
	for ( int i = 0; i < binsPt->nBins(); i++ ){

		double avgP = ((*binsPt)[i] + (*binsPt)[i+1] ) / 2.0;

		vector<double> tofMus = psr->centeredTofMeans( centerSpecies, avgP );
		vector<double> dedxMus = psr->centeredDedxMeans( centerSpecies, avgP );
		vector<double> tofSigmas;
		vector<double> dedxSigmas;

		for ( int iS = 0; iS < species.size(); iS ++ ){
			tofSigmas.push_back( tofParams[ iS ]->sigma( avgP, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) );
			dedxSigmas.push_back( dedxParams[ iS ]->sigma( avgP ) );
		}

		/*if ( (*binsPt)[ i ] > 1.2 ){
			sigmaRoi = 1.0;
			muRoi = .1;
		}*/

		vector<string> cen = { "per", "cen" };
		vector<string> charge = { "p_", "n_" };
		for ( int iCharge = 0; iCharge < charge.size(); iCharge++ ){
			for ( int iCen = 0; iCen < cen.size(); iCen++  ){


				string tname = "tof/" + PidPhaseSpace::tofName( centerSpecies, 0, i, 0 )+cen[iCen];
				if ( 0 == iCharge )
					tname = "tof/" + PidPhaseSpace::tofName( centerSpecies, 1, i, 0 )+cen[iCen];
				else 
					tname = "tof/" + PidPhaseSpace::tofName( centerSpecies, -1, i, 0 )+cen[iCen];

				TH1D * tofAll = (TH1D*)inFile->Get( tname.c_str() );
				string draw = "";
				for ( int iS = 0; iS < species.size(); iS ++ ){
					string mName = "tofMean" + species[iS];
					string sName = "tofSigma" + species[iS];

					if ( iS == 2){
						
						

						if ( 0 == iCharge )
							tname = "tof/" + PidPhaseSpace::tofName( centerSpecies, 1, i, 0, species[iS] )+cen[iCen];
						else 
							tname = "tof/" + PidPhaseSpace::tofName( centerSpecies, -1, i, 0, species[iS] )+cen[iCen];

						TH1D * tofAll = (TH1D*)inFile->Get( tname.c_str() );


						GaussianFitResult gfr = fitSingleSpecies( tofAll, tofMus[ iS ], tofSigmas[iS], draw, iS+1);
						//yields[ iS ] = yld;

						book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinContent( i+1, gfr.mu );
						book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinError( i+1, gfr.muError );
					} else if ( iS <= 1) {

						if ( i<= 10  ){

							if ( 0 == iCharge )
								tname = "tof/" + PidPhaseSpace::tofName( centerSpecies, 1, i, 0, species[iS] )+cen[iCen];
							else 
								tname = "tof/" + PidPhaseSpace::tofName( centerSpecies, -1, i, 0, species[iS] )+cen[iCen];

							//TH1D * tofAll = (TH1D*)inFile->Get( tname.c_str() );

							GaussianFitResult gfr = fitSingleSpecies( tofAll, tofMus[ iS ], tofSigmas[iS], draw, iS+1);
							book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinContent( i+1, gfr.mu );
						book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinError( i+1, gfr.muError );
						} else if ( i > 10 && iS == 0 && i < 40 ){
							string dr = "";
							GaussianFitResult gfr = fitTwoSpecies( tofAll, tofMus, tofSigmas );
							book->get("yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinContent( i+1, gfr.mu );
							book->get("yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinError( i+1, gfr.muError );
							if ( i < 40 ){
								book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS+1] )->SetBinContent( i+1, gfr.sigma );
							book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS+1] )->SetBinError( i+1, gfr.sigmaError );		
							}
							
						} 
						

					}
				}
				
			}
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
	model->fitTo( *rdh /*, Range( "roiPi,roiK,roiP" )*/ );

	/**
	 * Report the data + gauss
	 */
	reporter->newPage();

	RooPlot * frame = x.frame( Title( h->GetTitle() ) );
	rdh->plotOn( frame );
	
	model->plotOn( frame, Range( "Full" ) );
	model->plotOn( frame, Range( "Full" ), Components( "gaussK" ), LineColor( kGreen ) );
	model->plotOn( frame, Range( "Full" ), Components( "gaussPi" ), LineColor( kRed ) );
	model->plotOn( frame, Range( "Full" ), Components( "gaussP" ), LineColor( kBlack ) );
	
	
	gPad->SetLogy();
	frame->Draw();
	frame->SetMinimum( 1 );

	reporter->savePage();

	GaussianFitResult gfr;


	return gfr;

}


SimultaneousGaussians::GaussianFitResult SimultaneousGaussians::fitSingleSpecies( TH1D* h, double iMu, double iSigma, string drOpt, int color ){

	using namespace RooFit;
	RooMsgService::instance().setGlobalKillBelow(ERROR);

	/**
	 * Make the data hist for RooFit
	 */
	// get some histogram limits
	double x1 = h->GetXaxis()->GetXmin();
	double x2 = h->GetXaxis()->GetXmax();
	RooRealVar x( "x", "x", x1, x2 );
	RooDataHist * rdh = new RooDataHist( "data", "data", RooArgSet( x ), h  );

	/**
	 * Create the parameters and the gaussian
	 */
	RooRealVar mu( "mu", "mu", iMu, iMu - muRoi * iSigma, iMu + muRoi * iSigma );
	RooRealVar sigma( "sig", "sig", iSigma, iSigma / sigmaRoi, sigmaRoi * iSigma );
	RooGaussian gauss( "gauss", "gauss", x, mu, sigma );

	RooRealVar n( "n", "n", 100000, 0, 20000000000000);
	RooExtendPdf eGauss( "eGauss", "eGauss", gauss, n );

	/**
	 * Set the region of interest for fitting
	 */
	x.setRange( "roi", iMu - roi * iSigma, iMu + roi * iSigma );
	eGauss.fitTo( *rdh, Range( "roi" ), RooFit::PrintLevel(-1) );
	cout << " N = "<< n.getVal() << endl;


	/**
	 * Report the data + gauss
	 */
	reporter->newPage();

	RooPlot * frame = x.frame( Title( h->GetTitle() ) );
	rdh->plotOn( frame );
	eGauss.plotOn( frame, Range( "Full" ), LineColor( color ) );
	
	gPad->SetLogy();
	frame->Draw( "same" );
	frame->SetMinimum( 1 );

	//reporter->savePage();

	GaussianFitResult gfr;

	gfr.mu = n.getVal();
	gfr.muError = n.getError();
	gfr.sigma = sigma.getVal();

	return gfr;

}




void SimultaneousGaussians::sim1( TH1D* tof, TH1D*dedx, double avgP, vector<double> iYields ){


	using namespace RooFit;
	RooMsgService::instance().setGlobalKillBelow(ERROR);

	// get some histogram limits
	double x1 = tof->GetXaxis()->GetXmin();
	double x2 = tof->GetXaxis()->GetXmax();
	double y1 = dedx->GetXaxis()->GetXmin();
	double y2 = dedx->GetXaxis()->GetXmax();
	
	vector<double> tofMus = psr->centeredTofMeans( centerSpecies, avgP );
	vector<double> dedxMus = psr->centeredDedxMeans( centerSpecies, avgP );
	vector<double> tofSigmas;
	vector<double> dedxSigmas;


	vector<string> species = PidPhaseSpace::species;
	for ( int iS = 0; iS < species.size(); iS ++ ){
		tofSigmas.push_back( tofParams[ iS ]->sigma( avgP, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) );
		dedxSigmas.push_back( dedxParams[ iS ]->sigma( avgP ) );
	}

	/**
	 * Make the data hist for RooFit
	 */
	RooRealVar x( "x", "x", x1, x2 );
	RooDataHist * rdh1 = new RooDataHist( "dataTof", "dataTof", RooArgSet( x ), tof  );
	RooRealVar y( "y", "y", y1, y2 );
	RooDataHist * rdh2 = new RooDataHist( "dataDedx", "dataDedx", RooArgSet( y ), dedx  );


	RooCategory sample("sample","sample") ;
	sample.defineType("tof") ;
	sample.defineType("dedx") ;
	RooDataHist * rdh = new RooDataHist( "tofXdedx", "tofXdedx", RooArgSet( x, y), Index( sample ), Import( "tof", *rdh1), Import( "dedx", *rdh2) );


	// build a 3 gaussian model
	vector<RooRealVar*> tm;
	vector<RooRealVar*> dm;
	vector<RooRealVar*> ts;
	vector<RooRealVar*> ds;
	vector<RooRealVar*> yield;
	vector<RooGaussian*> tgauss;
	vector<RooGaussian*> dgauss;

	RooArgList *ralTGauss = new RooArgList();
	RooArgList *ralDGauss = new RooArgList();
	RooArgList *ralYield = new RooArgList();

	

	cout << "tofMus : " << tofMus.size() <<endl;
	cout << "tofSigmas : " << tofSigmas.size() <<endl;
	for ( int i = 0; i < species.size(); i++ ){

		RooRealVar * rrtm = new RooRealVar( 
			("tm" + species[ i ]).c_str(), ("tm" + species[ i ]).c_str(), 
			tofMus[ i ], 
			tofMus[ i ] - muRoi * tofSigmas[ i ], 
			tofMus[ i ] + muRoi * tofSigmas[ i ] );
		//tm.push_back( rrtm );
		RooRealVar * rrdm = new RooRealVar( 	("dm" + species[ i ]).c_str(), ("dm" + species[ i ]).c_str(), 
											dedxMus[ i ], dedxMus[ i ] - muRoi * dedxSigmas[ i ], dedxMus[ i ] + muRoi * dedxSigmas[ i ] );
	
		//dm.push_back( rrdm );


		RooRealVar * rrts = new RooRealVar( 	("ts" + species[ i ]).c_str(), ("ts" + species[ i ]).c_str(), 
										tofSigmas[ i ], tofSigmas[ i ] / sigmaRoi, tofSigmas[ i ] * sigmaRoi );
		ts.push_back( rrts );
		RooRealVar * rrds = new RooRealVar( 	("ds" + species[ i ]).c_str(), ("ds" + species[ i ]).c_str(), 
										dedxSigmas[ i ], dedxSigmas[ i ] / sigmaRoi, dedxSigmas[ i ] * sigmaRoi );
		ds.push_back( rrds );
		

		RooRealVar * yld = new RooRealVar( 	("yield" + species[ i ]).c_str(), ("yield" + species[ i ]).c_str(), 
										iYields[ i ], 1, 10000000000 );

		yield.push_back( yld );

		RooGaussian * rrtg = new RooGaussian(
			("tgauss" + species[ i ]).c_str(), ("tgauss" + species[ i ]).c_str(), x, *rrtm, *rrts );

		RooGaussian * rrdg = new RooGaussian( 	
			("dgauss" + species[ i ]).c_str(), ("dgauss" + species[ i ]).c_str(), y, *rrdm, *rrds );


		
		ralYield->add( *yld );
		ralTGauss->add( *rrtg );
		ralDGauss->add( *rrdg );
		


	}
	
	cout << "danny" << endl;

	RooAddPdf *tModel = new RooAddPdf( "tModel", "tModel", *ralTGauss, *ralYield ); 
	RooAddPdf *dModel = new RooAddPdf( "dModel", "dModel", *ralDGauss, *ralYield ); 

	RooSimultaneous simPdf("simPdf","simultaneous pdf",sample) ;
	simPdf.addPdf( *tModel, "tof" );
	simPdf.addPdf( *dModel, "dedx" );

	//simPdf.fitTo( *rdh );
	tModel->fitTo( *rdh1 );


	RooPlot * xFrame = x.frame( Title("#beta^{-1}") );
	RooPlot * yFrame = y.frame( Title("dE/dx") );

	rdh1->plotOn( xFrame );
	tModel->plotOn( xFrame );

	dModel->fitTo( *rdh2 );
	rdh2->plotOn( yFrame );
	dModel->plotOn( yFrame );

	reporter->newPage(1, 2);
	reporter->cd(1, 1);
	xFrame->Draw();

	reporter->cd(1, 2);
	yFrame->Draw();

	reporter->savePage();
	


}


SimultaneousGaussians::GaussianFitResult SimultaneousGaussians::fitTwoSpecies( TH1D* h, vector<double> iMu, vector<double> iSigma ){

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
	vector<RooRealVar*> yield;
	vector<RooGaussian> gauss;

	RooArgList *ralGauss = new RooArgList();
	RooArgList *ralYield = new RooArgList();

	vector<string> species = { "Pi", "K"};
	
	for ( int i = 0; i < species.size(); i++ ){
		lg->info( __FUNCTION__ ) << " Generating Gaussian for : " << species[ i ] << endl;
		lg->info( __FUNCTION__ ) << species[ i ] << " : " << iMu[ i ] << endl;

		RooRealVar * m = new RooRealVar( 	("mu" + species[ i ]).c_str(), ("mu" + species[ i ]).c_str(), 
											iMu[ i ], iMu[ i ] - muRoi * iSigma[ i ], iMu[ i ] + muRoi * iSigma[ i ] );
		mean.push_back( m );


		RooRealVar * s = new RooRealVar( 	("sig" + species[ i ]).c_str(), ("sig" + species[ i ]).c_str(), 
										iSigma[ i ]/*, iSigma[ i ] / sigmaRoi, iSigma[ i ] * sigmaRoi*/);
		sigma.push_back( s );
		

		RooRealVar * y = new RooRealVar( 	("yield" + species[ i ]).c_str(), ("yield" + species[ i ]).c_str(), 
										80000, 100, 1000000000000 );
		yield.push_back( y );
		RooGaussian * g = new RooGaussian( 	("gauss" + species[ i ]).c_str(), ("gauss" + species[ i ]).c_str(), 
										x, *m, *s );


		
		ralYield->add( *y );
		ralGauss->add( *g );

		// set the cut Range
		x.setRange( ("roi"+species[ i ]).c_str(), iMu[ i ] - iSigma[i] * roi, iMu[ i ] + iSigma[ i ] * roi  );


	}

	x.setRange( "roi", iMu[0] - iSigma[0]*roi, iMu[1] + iSigma[1]*roi ); 

	RooAddPdf *model = new RooAddPdf( "model", "model", *ralGauss, *ralYield ); 
	model->fitTo( *rdh , Range( "roi" ) );

	/**
	 * Report the data + gauss
	 */
	reporter->newPage();

	RooPlot * frame = x.frame( Title( h->GetTitle() ) );
	rdh->plotOn( frame );
	
	model->plotOn( frame, Range( "Full" ) );
	model->plotOn( frame, Range( "Full" ), Components( "gaussK" ), LineColor( kGreen ) );
	model->plotOn( frame, Range( "Full" ), Components( "gaussPi" ), LineColor( kRed ) );
	
	
	gPad->SetLogy();
	frame->Draw();
	frame->SetMinimum( 1 );

	model->paramOn(frame, Format("NELU", AutoPrecision(2)), Layout(0.1, 0.4, 0.9) );


	reporter->savePage();

	GaussianFitResult gfr;
	gfr.mu = yield[ 0 ]->getVal();
	gfr.muError = yield[ 0 ]->getError();
	gfr.sigma = yield[ 1 ]->getVal();
	gfr.sigmaError = yield[ 1 ]->getError();


	return gfr;

}





void SimultaneousGaussians::make2(){

	book->cd();
	book->makeAll( nodePath + "histograms" );
	vector<string> species = PidPhaseSpace::species;

	vector<double> yields = { 180000, 100, 10};

	for ( int iS = 0; iS < species.size(); iS ++ ){
		book->clone( "yield", "yield_n_cen_"+species[ iS ] );
		book->clone( "yield", "yield_n_per_"+species[ iS ] );

		book->clone( "yield", "yield_p_cen_"+species[ iS ] );
		book->clone( "yield", "yield_p_per_"+species[ iS ] );
	}
		
	for ( int i = 0; i < binsPt->nBins(); i++ ){

		double avgP = ((*binsPt)[i] + (*binsPt)[i+1] ) / 2.0;

		//vector<double> tofMus = psr->centeredTofMeans( centerSpecies, avgP );
		vector<double> tofMus = psr->centeredDedxMeans( centerSpecies, avgP );
		vector<double> tofSigmas;
		vector<double> dedxSigmas;

		for ( int iS = 0; iS < species.size(); iS ++ ){
			//tofSigmas.push_back( tofParams[ iS ]->sigma( avgP, psr->mass( species[ iS ] ), psr->mass( centerSpecies ) ) );
			tofSigmas.push_back( dedxParams[ iS ]->sigma( avgP ) );
		}

		/*if ( (*binsPt)[ i ] > 1.2 ){
			sigmaRoi = 1.0;
			muRoi = .1;
		}*/

		vector<string> cen = { "per", "cen" };
		vector<string> charge = { "p_", "n_" };
		for ( int iCharge = 0; iCharge < charge.size(); iCharge++ ){
			for ( int iCen = 0; iCen < cen.size(); iCen++  ){


				string tname = "" + PidPhaseSpace::dedxName( centerSpecies, 0, i, 0 )+cen[iCen];
				if ( 0 == iCharge )
					tname = "" + PidPhaseSpace::dedxName( centerSpecies, 1, i, 0 )+cen[iCen];
				else 
					tname = "" + PidPhaseSpace::dedxName( centerSpecies, -1, i, 0 )+cen[iCen];

				TH1D * tofAll = (TH1D*)inFile->Get( tname.c_str() );
				string draw = "";
				for ( int iS = 0; iS < species.size(); iS ++ ){
					string mName = "tofMean" + species[iS];
					string sName = "tofSigma" + species[iS];

					if ( iS == 2){
						
						

						if ( 0 == iCharge )
							tname = "" + PidPhaseSpace::dedxName( centerSpecies, 1, i, 0, species[iS] )+cen[iCen];
						else 
							tname = "" + PidPhaseSpace::dedxName( centerSpecies, -1, i, 0, species[iS] )+cen[iCen];

						TH1D * tofAll = (TH1D*)inFile->Get( tname.c_str() );


						GaussianFitResult gfr = fitSingleSpecies( tofAll, tofMus[ iS ], tofSigmas[iS], draw, iS+1);
						//yields[ iS ] = yld;

						book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinContent( i+1, gfr.mu );
						book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinError( i+1, gfr.muError );
					} else if ( iS <= 1) {

						if ( i<= 5  ){

							if ( 0 == iCharge )
								tname = "tof/" + PidPhaseSpace::tofName( centerSpecies, 1, i, 0, species[iS] )+cen[iCen];
							else 
								tname = "tof/" + PidPhaseSpace::tofName( centerSpecies, -1, i, 0, species[iS] )+cen[iCen];

							//TH1D * tofAll = (TH1D*)inFile->Get( tname.c_str() );

							GaussianFitResult gfr = fitSingleSpecies( tofAll, tofMus[ iS ], tofSigmas[iS], draw, iS+1);
							book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinContent( i+1, gfr.mu );
						book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinError( i+1, gfr.muError );
						} else if ( i > 5 && iS == 0 && i < 34 ){

							//sigmaRoi = 1.0;
							string dr = "";
							GaussianFitResult gfr = fitTwoSpecies( tofAll, tofMus, tofSigmas );
							book->get("yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinContent( i+1, gfr.mu );
							book->get("yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS]  )->SetBinError( i+1, gfr.muError );
							book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS+1] )->SetBinContent( i+1, gfr.sigma );
							book->get( "yield_"+charge[iCharge]+cen[iCen]+"_"+species[iS+1] )->SetBinError( i+1, gfr.sigmaError );	
						} 
						

					}
				}
				
			}
		}
		
	}

	

}



