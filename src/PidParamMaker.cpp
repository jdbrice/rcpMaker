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
#include "TGraphErrors.h"


void PidParamMaker::mathematicaExport( string prefix, string s, vector<GaussianFitResult> vals ){

	double min = cfg->getDouble( nodePath+"FitRange."+s+":min", 0 );
	double max = cfg->getDouble( nodePath+"FitRange."+s+":max", 5000 );


	/*Export Mu*/
	ofstream out( ( cfg->getString( nodePath+"output.report" ) + prefix + s + "Mu.dat").c_str() );
	out << prefix << "Mean" << s << " = { ";
	int i = 0;
	for ( GaussianFitResult gfr : vals ){

		if ( mmtms[ i ]/1000.0 < min || mmtms[ i ]/1000.0 > max ){
			i++;
			continue;
		}

		out << "{";
		out << mmtms[ i ] << ", " << gfr.mu << "}";
		if ( gfr.mu != vals[ vals.size()-1].mu
			&& i+1 < mmtms.size() && mmtms[ i + 1] / 1000.0 <= max )
			out << ", ";
		i++;
	}
	out << " };" << endl;
	out << endl << endl << prefix << "Mean" << s << "Error = { ";
	i = 0;
	for ( GaussianFitResult gfr : vals ){

		out << "{{";
		out << mmtms[ i ] << ", " << gfr.mu << "}, ";
		out << "ErrorBar[ " << gfr.muError << " ] }";
		if ( gfr.mu != vals[ tofFit[s].size()-1].mu )
			out << ", ";
		i++;
	}
	out << " };" << endl;
	out.close();


	/*Export Sigma*/
	ofstream outS( ( cfg->getString( nodePath+"output.report" ) +prefix + s + "Sigma.dat").c_str() );
	outS << prefix << "Sigma" << s << " = { ";
	i = 0;
	for ( GaussianFitResult gfr : vals ){

		if ( mmtms[ i ]/1000.0 < min || mmtms[ i ]/1000.0 > max ){
			i++;
			continue;
		}

		outS << "{";
		outS << mmtms[ i ] << ", " << gfr.sigma << "}";
		if ( gfr.sigma != vals[ tofFit[s].size()-1].mu
			&& i+1 < mmtms.size() && mmtms[ i + 1] / 1000.0 <= max )
			outS << ", ";
		i++;
	}
	outS << " };" << endl;
	outS << endl << endl << prefix << "Sigma" << s << "Error = { ";
	i = 0;
	for ( GaussianFitResult gfr : vals ){

		outS << "{{";
		outS << mmtms[ i ] << ", " << gfr.sigma << "}, ";
		outS << "ErrorBar[ " << gfr.sigmaError << " ] }";
		if ( gfr.sigma != vals[ tofFit[s].size()-1].mu )
			outS << ", ";
		i++;
	}
	outS << " };" << endl;
	outS.close();
}


double PidParamMaker::meanFunction( double *x, double *p ){

	double uT = p[ 0 ]; 	// timing offsets
	double uP = p[ 1 ]; 	// momentum
	double ueP = p[ 2 ]; 	// momentum
	double m = p[ 3 ]; 		// mass of species
	double mr = p[ 4 ]; 	// mass of centering species

	double mmtm = x[ 0 ];
	// characteristic determined by B field and Tof radius
	double x0 = 175.5 / mmtm;

	double a = 1.0 - x0 / ( TMath::ASin( x0 ) * TMath::Sqrt( 1 - x0*x0 ) ) * uP;
	double b = m*m / ( mmtm * TMath::Sqrt( mmtm * mmtm + m * m ) ) * ueP;
	double c = mr*mr / ( mmtm * TMath::Sqrt( mmtm * mmtm + mr * mr ) ) * ueP;

	return uT + a + b + c; 

}

double PidParamMaker::sigmaFunction( double *x, double *p ){

	double uT = p[ 0 ]; 	// timing offsets
	double uP = p[ 1 ]; 	// momentum
	double ueP = p[ 2 ]; 	// momentum
	double m = p[ 3 ]; 		// mass of species
	double mr = p[ 4 ]; 	// mass of centering species

	double mmtm = x[ 0 ];
	// characteristic determined by B field and Tof radius
	double x0 = 175.5 / mmtm;

	double a = 1.0 - x0 / ( TMath::ASin( x0 ) * TMath::Sqrt( 1.0 - x0*x0 ) ) * uP;
	double b = m*m / (mmtm * TMath::Sqrt( mmtm * mmtm + m * m ) ) * ueP;
	double c = mr*mr / (mmtm * TMath::Sqrt( mmtm * mmtm + mr * mr ) ) * ueP;

	return TMath::Sqrt( uT*uT + a*a + b*b + c*c );  

}



PidParamMaker::PidParamMaker( XmlConfig * config, string np ){
	//Set the Root Output Level
	gErrorIgnoreLevel=kSysError;


	// Save Class Members
	cfg = config;
	nodePath = np;

	assert( cfg );

	// make the Logger
	lg = LoggerConfig::makeLogger( cfg, np + "Logger" );

	lg->info(__FUNCTION__) << "Got config with nodePath = " << np << endl;

    // create the book
    lg->info(__FUNCTION__) << " Creating book "<< endl;
    book = new HistoBook( config->getString( np + "output.data" ), config );


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
	window 		= 1.5;
	lg->info(__FUNCTION__) << " Setup Complete "<< endl;
}

PidParamMaker::~PidParamMaker(){
	delete book;
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
	 * Make the histograms and the reporters
	 */
	for ( string plc : species ){

		book->clone( "mean", "tofMean" + plc );
		book->clone( "sigma", "tofSigma" + plc );
		book->clone( "mean", "dedxMean" + plc );
		book->clone( "sigma", "dedxSigma" + plc );

		// make a reporter for this species
		tofReport.push_back( 
			unique_ptr<Reporter>( 
				new Reporter( 	cfg->getString( nodePath + "output.report" ) + "tof_" + plc + ".pdf",
								800, 500 ) ) );
		dedxReport.push_back( 
			unique_ptr<Reporter>( 
				new Reporter( 	cfg->getString( nodePath + "output.report" ) + "dedx_" + plc + ".pdf",
								800, 500 ) ) );
	}

	int centralityBin = 1;

	TaskProgress tp( "Fitting Species", binsPt->nBins() * species.size() );
	for ( int i = 0; i < binsPt->nBins(); i++ ){


			double avgP = ((*binsPt)[i] + (*binsPt)[i+1] ) / 2.0;
			string title = "<p> = " + dts( avgP ) + " [GeV]";
			mmtms.push_back( avgP*1000 );

			vector<double> tofMus = psr->centeredTofMeans( centerSpecies, avgP );
			vector<double> dedxMus = psr->centeredDedxMeans( centerSpecies, avgP );

		for ( int iS = 0; iS < species.size(); iS ++ ){

			tp.showProgress( i * (iS+1) );

			string mName = "tofMean" + species[iS];
			string sName = "tofSigma" + species[iS];

			string name = "tof/" + PidPhaseSpace::tofName( centerSpecies, 0, centralityBin, i, 0, species[ iS ] );
			TH1D* tofAll = (TH1D*)inFile->Get( name.c_str() );
		
			GaussianFitResult tofGfr =  fitSingleSpecies( tofAll, tofMus[ iS ], tofSigmaIdeal, (*tofReport[ iS ]), "#beta^{-1} : " + title );
			tofFit[ species[ iS ] ].push_back( tofGfr );
			
			/* Fill Histos */
			book->get( mName )->SetBinContent( i, tofGfr.mu );
			book->get( sName )->SetBinContent( i, tofGfr.sigma );
			book->get( mName )->SetBinError( i, tofGfr.muError );
			book->get( sName )->SetBinError( i, tofGfr.sigmaError );	


			/**
			 * Fit dedx now
			 */
			mName = "dedxMean" + species[iS];
			sName = "dedxSigma" + species[iS];

			name = "dedx/" + PidPhaseSpace::dedxName( centerSpecies, 0, centralityBin, i, 0, species[ iS ] );
			TH1D* hDedx = (TH1D*)inFile->Get( name.c_str() );
		
			GaussianFitResult dedxGfr =  fitSingleSpecies( hDedx, dedxMus[ iS ], dedxSigmaIdeal, (*dedxReport[ iS ]), "dEdx : " + title );
			dedxFit[ species[ iS ] ].push_back( dedxGfr );
			/* Fill Histos */
			book->get( mName )->SetBinContent( i, dedxGfr.mu );
			book->get( sName )->SetBinContent( i, dedxGfr.sigma );
			book->get( mName )->SetBinError( i, dedxGfr.muError );
			book->get( sName )->SetBinError( i, dedxGfr.sigmaError );
			
		} // loop species
			
	} // loop pt bins


	for ( int i = 0; i < species.size(); i++ ){
		reportTofFitResult( species[ i ], (*tofReport[ i ]) );
		reportDedxFitResult( species[ i ], (*dedxReport[ i ]) );
		
		mathematicaExport( "tof", species[ i ], tofFit[ species[ i ] ] );
		mathematicaExport( "dedx", species[ i ], dedxFit[ species[ i ] ] );
	}


}

void PidParamMaker::reportTofFitResult( string s, Reporter &rp ){


	// convert the vector of GFR to vectors of mean / sigma
	vector<double> means, sigmas;
	vector<double> mE, sE;
	double maxM = -100, minM = 1000;
	double maxS = -100, minS = 1000;
	for ( int i = 0; i < tofFit[ s ].size(); i++ ){
		means.push_back( tofFit[ s ][ i ].mu );
		mE.push_back( tofFit[ s ][ i ].muError );
		sigmas.push_back( tofFit[ s ][ i ].sigma );
		sE.push_back( tofFit[ s ][ i ].sigmaError );

		if ( means[ i ] > maxM )
			maxM = means[ i ];
		if ( means[ i ] < minM )
			minM = means[ i ];
		if ( sigmas[ i ] > maxS )
			maxS = sigmas[ i ];
		if ( sigmas[ i ] < minS )
			minS = sigmas[ i ];
	
	}


	double min = cfg->getDouble( nodePath+"FitRange."+s+":min", 0 );
	double max = cfg->getDouble( nodePath+"FitRange."+s+":max", 5000 );
	double plotMin = cfg->getDouble( nodePath+"PlotRange.mu"+s+":min", 0 );
	double plotMax = cfg->getDouble( nodePath+"PlotRange.mu"+s+":max", 5000 );

	TF1 * mfn = new TF1( "mfn", PidParamMaker::meanFunction, min, max, 5 );
	mfn->FixParameter( 3, psr->mass( s ) );
	mfn->FixParameter( 4, psr->mass( centerSpecies ) );
	mfn->SetParameters( -0.00575185, -0.0037288, 0.00355843 );
	//mfn->SetParLimits( 1, 0.0, 0.99 );
	//mfn->SetParLimits( 2, 0.0, 0.990 );

	TF1 * sfn = new TF1( "sfn", PidParamMaker::sigmaFunction, 200, 1200, 5 );
	sfn->FixParameter( 3, psr->mass( s ) );
	sfn->FixParameter( 4, psr->mass( centerSpecies ) );
	sfn->SetParameters( 0.0111876, 0.113247, 0.00408183 );
	sfn->SetParLimits( 0, 0.01, 0.019 );
	//sfn->SetParLimits( 1, 0.10, 0.15 );
	//sfn->SetParLimits( 2, 0.00006, 0.20 );

	/**
	 * Plot means
	 */
	gStyle->SetOptStat( 0 );
	TH1F * mFrame = new TH1F( ("tof_" + s + "_mean").c_str(), ("Mean of " + s).c_str(), 10, mmtms[ 0 ], mmtms[ mmtms.size() -1 ] );
	TGraphErrors * mGraph = new TGraphErrors( means.size(), mmtms.data(), means.data(), 0, mE.data() );

	rp.newPage();
	mFrame->Draw();
	mFrame->GetYaxis()->SetRangeUser( plotMin, plotMax );
	mGraph->Draw( "same pe" );
	mGraph->Fit( mfn );
	rp.savePage();



	/**
	 * Plot Sigmas
	 */
	plotMin = cfg->getDouble( nodePath+"PlotRange.simga"+s+":min", 0 );
	plotMax = cfg->getDouble( nodePath+"PlotRange.sigma"+s+":max", 5000 );

	TH1F * sFrame = new TH1F( ("tof_" + s + "_sigma").c_str(), ("Sigma of " + s).c_str(), 10, mmtms[ 0 ], mmtms[ mmtms.size() -1 ] );
	TGraphErrors * sGraph = new TGraphErrors( sigmas.size(), mmtms.data(), sigmas.data(), 0, sE.data() );

	rp.newPage();
	sFrame->Draw();
	sFrame->GetYaxis()->SetRangeUser( plotMin, plotMax );
	sGraph->Draw( "same pe" );
	sGraph->Fit( sfn );
	
	rp.savePage();

}


void PidParamMaker::reportDedxFitResult( string s, Reporter &rp ){


	// convert the vector of GFR to vectors of mean / sigma
	vector<double> means, sigmas;
	double maxM = -100, minM = 1000;
	double maxS = -100, minS = 1000;
	for ( int i = 0; i < dedxFit[ s ].size(); i++ ){
		means.push_back( dedxFit[ s ][ i ].mu );
		sigmas.push_back( dedxFit[ s ][ i ].sigma );

		if ( means[ i ] > maxM )
			maxM = means[ i ];
		if ( means[ i ] < minM )
			minM = means[ i ];
		if ( sigmas[ i ] > maxS )
			maxS = sigmas[ i ];
		if ( sigmas[ i ] < minS )
			minS = sigmas[ i ];
	
	}

	/**
	 * Plot means
	 */
	gStyle->SetOptStat( 0 );
	TH1F * mFrame = new TH1F( ("dedx_" + s + "_mean").c_str(), ("Mean of " + s).c_str(), 10, mmtms[ 0 ], mmtms[ mmtms.size() -1 ] );
	TGraph * mGraph = new TGraph( means.size(), mmtms.data(), means.data() );

	rp.newPage();
	mFrame->Draw();
	mFrame->GetYaxis()->SetRangeUser( minM * 1.1, maxM * 1.1 );
	mGraph->Draw( "same" );
	rp.savePage();

	/**
	 * Plot Sigmas
	 */
	
	TH1F * sFrame = new TH1F( ("dedx_" + s + "_sigma").c_str(), ("Sigma of " + s).c_str(), 10, mmtms[ 0 ], mmtms[ mmtms.size() -1 ] );
	TGraph * sGraph = new TGraph( sigmas.size(), mmtms.data(), sigmas.data() );

	rp.newPage();
	sFrame->Draw();
	sFrame->GetYaxis()->SetRangeUser( minS * 1.1, maxS * 1.1 );
	sGraph->Draw( "same" );
	rp.savePage();

}



PidParamMaker::GaussianFitResult PidParamMaker::fitSingleSpecies( TH1D* h, double iMu, double iSigma, Reporter &rp, string title ){

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

	x.setRange( "roiError", iMu - window*roi * iSigma, iMu + window* roi * iSigma );
	egauss.fitTo( *rdh, Range( "roiError" ), RooFit::PrintLevel(-1) );

	/**
	 * Report the data + gauss
	 */
	rp.newPage();

	RooPlot * frame = x.frame( Title( title.c_str() ) );
	rdh->plotOn( frame );
	gauss.plotOn( frame );
	egauss.plotOn( frame, LineColor(kRed) );
	
	gPad->SetLogy();
	frame->Draw();
	frame->SetMinimum( 1 );

	rp.savePage();

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








