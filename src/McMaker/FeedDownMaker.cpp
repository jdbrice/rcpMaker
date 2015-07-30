#include "McMaker/FeedDownMaker.h"
#include <math.h>

#include <jdb/RooPlotLib.h>
#include "TGraphAsymmErrors.h"
#include "TLine.h"

vector<int> FeedDownMaker::plcID = { 8, 9, 11, 12, 14, 15 };
vector<float> FeedDownMaker::plcMass = { 0.1396, 0.1396, 0.4937, 0.4937, 0.9383, 0.9383 };

FeedDownMaker::FeedDownMaker( XmlConfig * config, string np, string fileList, string prefix ) : TreeAnalyzer( config, np, fileList, prefix  ){

	logger->setClassSpace( "FeedDownMaker" );
	logger->info(__FUNCTION__) << endl;
	/**
	 * Make the desired PicoDataStore Interface
	 */
	logger->debug(__FUNCTION__) << ds << endl;
	if ( ds && ds->getTreeName() == "StMiniMcTree" ){
		INFO( "Using DataStore" )
	} else {
		ERROR( "No Data Source. Specify one at <DataSourcce ... > </DataSource>" )
	}

	// map of GEANT PID -> histogram name
	plcName[ 8 ] = "Pi_Plus";
	plcName[ 9 ] = "Pi_Minus";
	plcName[ 11 ] = "K_Plus";
	plcName[ 12 ] = "K_Minus";
	plcName[ 14 ] = "P_Plus";
	plcName[ 15 ] = "P_Minus";


	//if ( cfg->exists( np + "RMCParams:config" ) )
	//	rmc = unique_ptr<RefMultCorrection> (new RefMultCorrection( config->getString( np + "RMCParams:config" ) ) );


}

FeedDownMaker::~FeedDownMaker(){

}



void FeedDownMaker::preEventLoop(){

	TreeAnalyzer::preEventLoop();

	for ( auto k : plcID ){

		book->clone( "parents", "parents_" + plcName[ k ] );
		book->clone( "pre_rapidity", "pre_rapidity_" + plcName[ k ] );
		book->clone( "rapidity", "rapidity_" + plcName[ k ] );

		book->clone( "spectra", "sig_" + plcName[ k ] );
		book->clone( "spectra", "back_" + plcName[ k ] );
		book->clone( "spectra", "spectra_" + plcName[ k ] );

		book->clone( "energyLoss", "energyLoss_" + plcName[ k ] );
	}
}



void FeedDownMaker::exportParams(string name, int plcIndex, float cl, float ch, TF1 * fn, string formula){

	INFO( "" )
	INFO( "Writing to " << cfg->getString(nodePath + "output.param") )

	string path = cfg->getString( nodePath + "output:path" ) + cfg->getString( nodePath + "output.param" );
	ofstream out( path.c_str(), ios::out | ios::app );

	double * p = fn->GetParameters();

	// stupid dan didnt make name right
	string tagName = "Pi_p";
	switch ( plcIndex ) {
		case 0 : 
			tagName = "Pi_p";
			break;
		case 1 : 
			tagName = "Pi_n";
			break;
		case 2 : 
			tagName = "K_p";
			break;
		case 3 : 
			tagName = "K_n";
			break;
		case 4 : 
			tagName = "P_p";
			break;
		case 5 : 
			tagName = "P_n";
			break;
	}

	// a bit hacky but
	// start tag on the 0-80% one
	if ( cl == 5 && ch >= 400 )
		out << "<" << tagName << ">" << endl;

	out << "\t<FeedDownParams";
	out << " formula=\"" << formula << "\"";
	out << " cl=\"" << cl << "\"";
	out << " ch=\"" << ch << "\"";
	for ( int i = 0; i < fn->GetNpar(); i++ ){
		out << " p" << i << "=\"" << p[i] << "\"";
	}
	out << "/>" << endl;

	// close it for the last one (0-5%)
	if ( cl > 5 && ch >= 400 )
		out << "</" << tagName << ">" << endl << endl;



	out.close();


}


void FeedDownMaker::background( string name, int plcIndex, int cl, int ch ){

	RooPlotLib rpl; 

	vector<string> func = { "[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
							"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
							"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
							"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
							"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x * x ) ",
							"(1-[0]*exp( -[1] * (x)  ) ) * [3] * exp( -[4] * x )" };

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

	TH1 * back = book->get2D( "back_" + name )->ProjectionX( "px", cl, ch );
	TH1 * total = book->get2D( "spectra_" + name )->ProjectionX( "tpx", cl, ch );

	//back->Draw();
	//p1->Draw();

	g->BayesDivide( back, total );

	rpl.style( g ).set( cfg, "Style.frac_" + name ).set( cfg, "Style.frac" ).set( "title", hNames[ plcIndex ] + " : " + ts(cl) + " #leq M < " + ts(ch) ).draw();

	INFO( "Fitting to : " << func[ plcIndex ]  )
	TF1 * fracFun = new TF1( "fn", func[ plcIndex ].c_str() , 0.0, 2.0 );
	
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
	exportParams( name, plcIndex, cl, ch, fracFun, func[ plcIndex ] );


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
		reporter->saveImage( cfg->getString( nodePath + "output:path" ) + "image/" + name + "_back_" + ts(cl) +"_" + ts(ch) + ".eps" );
	reporter->savePage();
}



void FeedDownMaker::postEventLoop(){


	RooPlotLib rpl; 


	vector<float> rmb = {5,20,59,93,138,200,239,450};
	vector<string> func = { "[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
							"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
							"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
							"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
							"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
							"(1-[0]*exp( -[1] * x ) ) * [2] * exp( -[3] * x )" };
	vector<string> hNames = { "#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "Proton", "#bar{P}" };

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
		

		// plot the background parameterization for 0-80%
		background( k.second, plcIndex, 5, 	500 );
		// Plot the background parameterization for each centrality range
		background( k.second, plcIndex, 5, 	20 );
		background( k.second, plcIndex, 20, 59 );
		background( k.second, plcIndex, 59, 93 );
		background( k.second, plcIndex, 93, 138 );
		background( k.second, plcIndex, 138, 200 );
		background( k.second, plcIndex, 200, 239 );
		background( k.second, plcIndex, 239, 500 );

		
/*
		TF1 * el = new TF1( "el", "[0] + [1] * pow( x, -[2] )", 0.2, 2.0 );
		reporter->newPage();
		rpl.style( book->get( "energyLoss_" + k.second ) ).set( cfg, "Style.energyLoss" ).draw();

		TH1 * px = (TH1*)book->get2D( "energyLoss_" + k.second )->ProfileX();
		rpl.style( px ).set( cfg, "Style.energyLoss1D" ).draw();

		reporter->savePage();
		reporter->newPage();

		px->Fit( el, "R" );
		px->Fit( el, "R" );
		rpl.style( px ).set( cfg, "Style.energyLoss1D" ).draw();	

		reporter->savePage();
		*/
		plcIndex ++;
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

	if ( vR > 1.0 )
		return false;
	if ( refMult < 5 )
		return false;

	return true;
}

void FeedDownMaker::analyzeEvent(){

	int nTracks = ds->get<int>( "mNMatchedPair" );


	for ( Int_t iTrack = 0; iTrack < nTracks; iTrack ++ ){

		if ( !keepTrack( iTrack ) )
			continue;
		
		analyzeTrack( iTrack );	
		
	}

}


bool FeedDownMaker::keepTrack( Int_t iTrack ){



	float pt = ds->get<float>( "mMatchedPairs.mPtPr", iTrack );
	float dca = ds->get<float>( "mMatchedPairs.mDcaGl", iTrack );
	float ptg = ds->get<float>( "mMatchedPairs.mPtGl", iTrack );
	float eta = ds->get<float>( "mMatchedPairs.mEtaPr", iTrack );

	short fitPts = ds->get<short>( "mMatchedPairs.mFitPts", iTrack );
	short dedxPts = ds->get<short>( "mMatchedPairs.mDedxPts", iTrack );
	short possiblePts = ds->get<short>( "mMatchedPairs.mNPossible", iTrack );

	float ptr = ptg / pt;
	float fitr = (float)fitPts / (float)possiblePts;

	book->fill( "pre_dca", dca );
	book->fill( "pre_fitPts", fitPts );
	book->fill( "pre_dedxPts", dedxPts );
	book->fill( "pre_possiblePts", possiblePts );
	book->fill( "pre_fitr", fitr );
	book->fill( "pre_ptr", ptr );


	if ( fitPts < 15 )
		return false;
	if ( dedxPts < 10 )
		return false;
	if ( fitr < 0.52 )
		return false;
	if ( ptr < (7.0 / 10.0) || ptr > ( 10.0 / 7.0 ) )
		return false;
	if ( dca > 1.0 )
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
	float ptMc = ds->get<float>( "mMatchedPairs.mPtMc", iTrack );
	float eta = ds->get<float>( "mMatchedPairs.mEtaPr", iTrack );

	int refMult = ds->get<int>( "mNUncorrectedPrimaries" );

	auto pos = find( plcID.begin(), plcID.end(), geantID );
	if ( pos != plcID.end() ){

		string name = plcName[ geantID ];
		int index = pos - plcID.begin();
		
		float y = rapidity( pt, eta, plcMass[ index ] );

		book->fill( "pre_rapidity_" + name, y );

		// rapidity cut
		if ( abs( y ) > 0.25 ){
			return;
		}

		book->fill( "rapidity_" + name, y );
		book->fill( "parents_" + name, pgeantID, pt );

		// signal
		if ( pgeantID == 0 )
			book->fill( "sig_" + name, pt, refMult );
		else if ( pgeantID > 0 ) // backgrounds
			book->fill( "back_" + name, pt, refMult );
		book->fill( "spectra_" + name, pt, refMult );

		book->fill( "energyLoss_" + name, pt, pt - ptMc );

	}



}













