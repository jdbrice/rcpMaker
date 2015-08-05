#include "McMaker/FeedDownMaker.h"
#include "Common.h"


// Roobarb
#include <jdb/RooPlotLib.h>

// ROOT
#include "TGraphAsymmErrors.h"
#include "TLine.h"

// STL
#include <math.h>

vector<int> FeedDownMaker::plcID = { 8, 9, 11, 12, 14, 15 };
vector<float> FeedDownMaker::plcMass = { 0.1396, 0.1396, 0.4937, 0.4937, 0.9383, 0.9383 };

FeedDownMaker::FeedDownMaker( XmlConfig * config, string np, string fileList, string prefix ) : TreeAnalyzer( config, np, fileList, prefix  ){

	logger->setClassSpace( "FeedDownMaker" );
	logger->info(__FUNCTION__) << endl;
	 
	logger->debug(__FUNCTION__) << ds << endl;
	if ( ds && ds->getTreeName() == "StMiniMcTree" ){
		INFO( "Using DataStore" )
	} else {
		ERROR( "No Data Source. Specify one at <DataSourcce ... > </DataSource>" )
	}

	// map of GEANT PID -> histogram name
	plcName[ 8 ] = "Pi_p";
	plcName[ 9 ] = "Pi_n";
	plcName[ 11 ] = "K_p";
	plcName[ 12 ] = "K_n";
	plcName[ 14 ] = "P_p";
	plcName[ 15 ] = "P_n";


	// Tracks cuts
    cut_nHitsFit				= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.nHitsFit", 				0, 		INT_MAX ) );
    cut_dca 					= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.dca", 					0, 		INT_MAX ) );
	cut_nHitsFitOverPossible 	= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.nHitsFitOverPossible", 	0, 		INT_MAX ) );
    cut_nHitsDedx 				= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.nHitsDedx", 				0, 		INT_MAX ) );
    cut_pt 						= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.pt", 					0, 		INT_MAX ) );
    cut_ptGlobalOverPrimary 	= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.ptGlobalOverPrimary", 	0.7, 	1.42 ) );
    cut_rapidity				= unique_ptr<ConfigRange>(new ConfigRange( cfg, "TrackCuts.rapidity", 				-0.25, 	0.25 ) );

    formulas =	{ "[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
				"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
				"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
				"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
				"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x * x )",
				"(1-[0]*exp( -[1] * x ) ) * [2] * exp( -[3] * x )" };

	rmb = unique_ptr<HistoBins>( new HistoBins( cfg, nodePath + "RefMultBins" ) );

	// Setup the centrality bins
   	logger->info( __FUNCTION__ ) << "Loading Centrality Map" << endl; 
    centralityBinMap = cfg->getIntMap( np + "CentralityMap" );
    centralityBins = cfg->getIntVector( np + "CentralityBins" );
    logger->info( __FUNCTION__ ) << "c[ 0 ] = " << centralityBinMap[ 0 ] << endl;
}

FeedDownMaker::~FeedDownMaker(){
}



void FeedDownMaker::preEventLoop(){

	TreeAnalyzer::preEventLoop();

	for ( auto k : plcID ){
		for ( int cb : cfg->getIntVector( nodePath + "CentralityBins" ) ){
			string name = plcName[ k ] + "_" + ts(cb);
			book->clone( "spectra", "sig_" + name );
			book->clone( "spectra", "back_" + name );
			book->clone( "spectra", "spectra_" + name );
		}

		string name = plcName[ k ];
		book->clone( "parents", "parents_" + name );
		book->clone( "pre_rapidity", "pre_rapidity_" + name );
		book->clone( "rapidity", "rapidity_" + name );

		book->clone( "spectra", "sig_" + name );
		book->clone( "spectra", "back_" + name );
		book->clone( "spectra", "spectra_" + name );
	}
}



void FeedDownMaker::exportParams( int bin, TF1 * fn, ofstream &out){
	out << "\t\t<FeedDownParams ";
	out << "bin=\"" << bin << "\" ";
	out << Common::toXml( fn );
	out << "/>" << endl;
}


void FeedDownMaker::background( string name, int plcIndex, int bin, ofstream &out ){

	RooPlotLib rpl; 

	vector<string> hNames = { "#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "Proton", "#bar{P}" };

	reporter->newPage();
	reporter->cd(0);

	// Upper pad for the data points +  the fit
	TPad * p1 = new TPad( "fit", "fit", 0.001, 0.3, 0.99, 0.99 );
	p1->SetBottomMargin( 0.0 );
	p1->SetLeftMargin( 0.1 );

	// Lower pad with the data / fit ratio
	TPad * p2 = new TPad( "ratio", "ratio", 0.001, 0.01, 0.99, 0.3 );
	p2->SetTopMargin( 0.0 );
	p2->SetLeftMargin( 0.1 );
	p1->Draw();
	p2->Draw();
	p1->cd();
	
	
	
	TGraphAsymmErrors * g = new TGraphAsymmErrors();


	TH1 * back = book->get( "back_" + name +"_" + ts(bin) );
	TH1 * total = book->get( "spectra_" + name + "_" + ts(bin) );

	//back->Draw();
	//p1->Draw();

	g->BayesDivide( back, total );

	rpl.style( g ).set( cfg, "Style.frac_" + name ).set( cfg, "Style.frac" ).set( "title", hNames[ plcIndex ] + " : bin " + ts(bin) ).draw();

	INFO( "Fitting to : " << formulas[ plcIndex ]  )
	TF1 * fracFun = new TF1( "fn", formulas[ plcIndex ].c_str() , 0.0, 2.0 );
	
	fracFun->SetParLimits( 0, 0, 1 );
	fracFun->SetParLimits( 1, 0, 50 );
	fracFun->SetParLimits( 2, 0, 1 );
	fracFun->SetParLimits( 3, 0, 50 );

	fracFun->SetParameters( .1, 0.005, .01, 0.002, 0.1, 0.002 );
	if ( "P_Plus" == name ){
		fracFun->SetParameters( .1, 0.005, .01, 0.002 );
	} else  if ( "P_Minus" == name ){
		INFO( name )
		fracFun->SetParLimits( 0, 0.001, 1 );
		fracFun->SetParLimits( 1, 0, 50 );
		//fracFun->SetParLimits( 2, 0.05, 0.4 );
		fracFun->SetParLimits( 2, 0, 1 );
		fracFun->SetParLimits( 3, 0, 50 );

		fracFun->SetParameters( .1, 10, 0.1, 0.3, .75 );
		fracFun->SetRange( 0.0, 1.5 );

	} else if ( "P_Plus" == name ){
		fracFun->SetParLimits( 4, 0, 1 );
		fracFun->SetParLimits( 5, 0, 50 );
		fracFun->SetParameters( .1, 0.005, .01, 0.002, 0.1, 0.002, 0.1, 0.002 );
	}

	g->Fit( fracFun, "RN" );
	g->Fit( fracFun, "RN" );
	g->Fit( fracFun, "RN" );

	fracFun->SetRange( 0.0, 5 );
	exportParams( bin, fracFun, out );

	book->cd( "results" );
	g->SetName( ("y" + name + "_" + ts(bin) ).c_str() );
	book->add( name + "_" + ts(bin), g );
	book->cd();




	// Draw the components of the Proton fit
	if ( "P_Plus" == name ){
		TF1 * fc1 = new TF1( "fc1", "[0]*exp( -[1] * x )" );
		double * pars = fracFun->GetParameters();
		fc1->SetParameters( pars[0], pars[1] );
		fc1->SetLineColor( kGreen );
		fc1->Draw("same");

		TF1 * fc2 = new TF1( "fc2", "[0]*exp( -[1] * x * x )" );
		fc2->SetParameters( pars[2], pars[3] );
		fc2->SetLineColor( kBlue );
		fc2->Draw("same");

		TLegend * leg = new TLegend( 0.6, 0.5, 0.9, 0.9 );
		leg->AddEntry( fc1, "ae^{-b pT}", "l" );
		leg->AddEntry( fc2, "ae^{-b pT^{2}}", "l" );
		leg->Draw("same");
	}

	// Draw the components of the Anti-Proton fit
	if ( "P_Minus" == name ){
		TF1 * fc1 = new TF1( "fc1", "1-[0]*exp( -[1] * ( x ) )", 0, 5 );
		double * pars = fracFun->GetParameters();
		fc1->SetParameters( pars[0], pars[1] );
		fc1->SetLineColor( kGreen );
		fc1->Draw("same");

		TF1 * fc2 = new TF1( "fc2", "[0]*exp( -[1] * x )", 0, 5 );
		fc2->SetParameters( pars[2], pars[3] );
		fc2->SetLineColor( kBlue );
		fc2->Draw("same");

		TLegend * leg = new TLegend( 0.6, 0.5, 0.9, 0.9 );
		leg->AddEntry( fc1, "1 - ae^{-bpT}", "l" );
		leg->AddEntry( fc2, "ae^{-bpT}", "l" );
		leg->Draw("same");
	}

	// Draw the components of the Pion fit
	if ( "Pi_Plus" == name || "Pi_Minus" == name ){
		TF1 * fc1 = new TF1( "fc1", "[0]*exp( -[1] * x )", 0, 5 );
		double * pars = fracFun->GetParameters();
		fc1->SetParameters( pars[0], pars[1] );
		fc1->SetLineColor( kGreen );
		fc1->Draw("same");

		TF1 * fc2 = new TF1( "fc2", "[0]*exp( -[1] * x )", 0, 5 );
		fc2->SetParameters( pars[2], pars[3] );
		fc2->SetLineColor( kBlue );
		fc2->Draw("same");
	}
	fracFun->Draw("same");


	p2->cd();
	back->Divide( total );
	TH1 * ratio = (TH1*)back->Clone( "ratio" );

	for ( int i = 2; i < ratio->GetNbinsX() + 1; i++ ){
		float bc = ratio->GetBinCenter( i );
		float fv = fracFun->Eval( bc );
		float bv = g->GetY()[ i - 2 ];
		float be = g->GetErrorYlow( i - 2) ;
		

		ratio->SetBinContent( i, bv / fv );

		ratio->SetBinError( i, be / fv );
	}
	ratio->GetYaxis()->SetNdivisions( 5, 2, 0, true );
	ratio->GetYaxis()->SetTickLength( 0.01 );

	rpl.style( ratio ).set( cfg, "Style.ratio" ).draw();
	

	TLine * l = new TLine( 0.0, 1, 2, 1  );
	l->SetLineStyle( kDotted );
	l->Draw("same");
	

	if ( cfg->getBool( nodePath + "Pages:export", true ) )
		reporter->saveImage( cfg->getString( nodePath + "output:path" ) + "image/" + name + "_back_" + ts(bin) + ".eps" );
	reporter->savePage();
}



void FeedDownMaker::postEventLoop(){

	RooPlotLib rpl; 

	vector<string> hNames = { "#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "Proton", "#bar{P}" };

	string path = cfg->getString( nodePath + "output.param" );
	ofstream out( path.c_str(), ios::out );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;



	int plcIndex = 0;
	for ( auto k : plcName ){
		

		if ( cfg->getBool( nodePath + "Pages:parents", false ) ){
			TH2 * h2d = book->get2D( "parents_" + k.second );
			for ( int i = 1; i < h2d->GetNbinsY(); i++ ){
				
				TH1 * projX = book->get2D( "parents_" + k.second )->ProjectionX( "_px", i, i+1);
				TH1 * projY = book->get2D( "parents_" + k.second )->ProjectionY( "_py");
				float ptl = projY->GetBinLowEdge( i );
				float pth = ptl + projY->GetBinWidth(i);
				// Feeddown - contributions
				reporter->newPage();
				
				addGEANTLabels( projX );
				projX->GetXaxis()->LabelsOption( " " );
				rpl.style( projX ).set( cfg, "Style.logy1D" ).set( "title", hNames[ plcIndex ] + " : " + dts( ptl ) + " < pT < " + dts( pth ) ).draw();

				reporter->savePage();
				reporter->saveImage( cfg->getString( nodePath + "output:path" ) + "image/" + k.second + "_" + dts( ptl ) + "_pT_" + dts( pth ) + ".eps"  );
			}
		}

		if ( "K_Plus" == k.second || "K_Minus" == k.second ){
			plcIndex ++;
			continue;
		}
		

		out << "\t<" << k.second << ">" << endl;
		for ( int b : cfg->getIntVector( nodePath + "CentralityBins" ) ) {
			background( k.second, plcIndex, b, out );	
		}
		out << "\t</" << k.second << ">" << endl;

		plcIndex ++;
	}
	
	out.close();
}



bool FeedDownMaker::keepTrack( Int_t iTrack ){



	float pt 	= ds->get<float>( "mMatchedPairs.mPtPr", iTrack );
	float dca 	= ds->get<float>( "mMatchedPairs.mDcaGl", iTrack );
	float ptg 	= ds->get<float>( "mMatchedPairs.mPtGl", iTrack );
	float eta 	= ds->get<float>( "mMatchedPairs.mEtaPr", iTrack );

	short fitPts = ds->get<short>( "mMatchedPairs.mFitPts", iTrack );
	short dedxPts = ds->get<short>( "mMatchedPairs.mDedxPts", iTrack );
	short possiblePts = ds->get<short>( "mMatchedPairs.mNPossible", iTrack );

	float ptr = ptg / pt;
	float fitr = (float)fitPts / (float)possiblePts;

	// book->fill( "pre_dca", dca );
	// book->fill( "pre_fitPts", fitPts );
	// book->fill( "pre_dedxPts", dedxPts );
	// book->fill( "pre_possiblePts", possiblePts );
	// book->fill( "pre_fitr", fitr );
	// book->fill( "pre_ptr", ptr );


	if ( fitPts < cut_nHitsFit->min )
		return false;
	if ( dedxPts < cut_nHitsDedx->min )
		return false;
	if ( fitr < cut_nHitsFitOverPossible->min )
		return false;
	if ( ptr < cut_ptGlobalOverPrimary->min || ptr > cut_ptGlobalOverPrimary->max )
		return false;
	if ( dca > cut_dca->max )
		return false;

	book->fill( "dca", dca );
	book->fill( "fitPts", fitPts );
	book->fill( "dedxPts", dedxPts );
	book->fill( "possiblePts", possiblePts );
	book->fill( "fitr", fitr );
	book->fill( "ptr", ptr );
	book->fill( "eta", eta );

	return true;
}

void FeedDownMaker::analyzeTrack( Int_t iTrack ){

	int geantID = (int)ds->get<UShort_t>( "mMatchedPairs.mGeantId", iTrack );
	int pgeantID = (int)ds->get<UShort_t>( "mMatchedPairs.mParentGeantId", iTrack );

	float pt = ds->get<float>( "mMatchedPairs.mPtPr", iTrack );
	float eta = ds->get<float>( "mMatchedPairs.mEtaPr", iTrack );

	int refMult = ds->get<int>( "mNUncorrectedPrimaries" );

	auto pos = find( plcID.begin(), plcID.end(), geantID );
	if ( pos != plcID.end() ){

		string name = plcName[ geantID ];
		int index = pos - plcID.begin();
		
		float y = Common::rapidity( pt, eta, plcMass[ index ] );

		book->fill( "pre_rapidity_" + name, y );

		// rapidity cut
		if ( abs( y ) > 0.25 ){
			return;
		}

		book->fill( "rapidity_" + name, y );
		book->fill( "parents_" + name, pgeantID, pt );

		// signal
		if ( pgeantID == 0 ){
			book->fill( "sig_" + name, pt, refMult );
			book->fill( "sig_" + name + "_" + ts(cBin), pt, refMult );
		}
		else if ( pgeantID > 0 ){ // backgrounds
			book->fill( "back_" + name, pt, refMult );
			book->fill( "back_" + name + "_" + ts(cBin), pt, refMult );
		}

		book->fill( "spectra_" + name, pt, refMult );
		book->fill( "spectra_" + name + "_" + ts(cBin), pt, refMult );
		

	}
}

bool FeedDownMaker::keepEvent(){


	int refMult = ds->get<int>( "mNUncorrectedPrimaries" );

	float vX = ds->get<float>( "mVertexX" );
	float vY = ds->get<float>( "mVertexY" ) + 0.89;
	float vZ = ds->get<float>( "mVertexZ" );

	float vR = sqrt( vX * vX + vY * vY);

	book->fill( "vR", vR );
	book->fill( "vX", vX );
	book->fill( "vY", vY );
	book->fill( "vZ", vZ );

	book->fill( "refMult", refMult );
	book->fill( "refMultVz", vZ, refMult );

	if ( refMult < 5 )
		return false;

	return true;
}

void FeedDownMaker::analyzeEvent(){


	// Getting the Centrality bin
	int refMult = ds->get<int>( "mNUncorrectedPrimaries" );
	int bin16 = rmb->findBin( refMult, BinEdge::upper );
	
	int bin9 = (int)bin16 / 2;
	
	if ( bin16 == 15 )
		bin9 = 8;
	else if ( bin16 < 0 )
		bin9 = -1;

	if ( bin9 >= 0 && bin9 <= 8 )
		cBin = centralityBinMap[ bin9 ];
	else
		cBin = -1;

	if ( cBin < 0 || bin16 < 0 )
		return;
	
	book->fill( "refMultBins", refMult, bin9 );
	book->fill( "refMultMappedBins", refMult, cBin );
	

	int nTracks = ds->get<int>( "mNMatchedPair" );

	for ( Int_t iTrack = 0; iTrack < nTracks; iTrack ++ ){

		if ( !keepTrack( iTrack ) )
			continue;
		
		analyzeTrack( iTrack );			
	}

}




