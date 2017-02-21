#include "McMaker/FeedDownFitter.h"

// Roobarb
#include <RooPlotLib.h>
#include "FitConfidence.h"

// ROOT
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TLatex.h"




// STL
#include <math.h>


vector<int> FeedDownFitter::plcID = { 8, 9, 11, 12, 14, 15 };
vector<float> FeedDownFitter::plcMass = { 0.1396, 0.1396, 0.4937, 0.4937, 0.9383, 0.9383 };

void FeedDownFitter::init( XmlConfig &_config, string _nodePath ){
	TaskRunner::init( _config, _nodePath );

	// map of GEANT PID -> histogram name
	plcName[ 8 ] = "Pi_p";
	plcName[ 9 ] = "Pi_n";
	plcName[ 11 ] = "K_p";
	plcName[ 12 ] = "K_n";
	plcName[ 14 ] = "P_p";
	plcName[ 15 ] = "P_n";

	formulas =	{ "[0] * exp( -pow([1]/x, [2]))",
				  "[0] * exp( -pow([1]/x, [2]))",
				  "[0] * exp( -pow([1]/x, [2]))",
				  "[0] * exp( -pow([1]/x, [2]))",
				  "[0] * exp( -pow([1]/x, [2]))",
				  "[0] * exp( -pow([1]/x, [2]))" };

				// formulas =	{ "[0] * pow( x, - abs([1])) + [2] * exp( -[3] * x )",
				// "[0] * pow( x, - abs([1])) + [2] * exp( -[3] * x )",
				// "([0]*exp( -[1] * x ) + [2] * exp( -[3] * x ))",
				// "([0]*exp( -[1] * x ) + [2] * exp( -[3] * x ))",
				// "([0]*exp( -[1] * x ) + [2] * exp( -[3] * x * x ))",
				// "(pow( [0] + [1] * pow( x, [2] ), -1 ))" };

	rmb = unique_ptr<HistoBins>( new HistoBins( config, nodePath + ".RefMultBins" ) );

	book = unique_ptr<HistoBook>( new HistoBook( config.getXString( nodePath + ".output:path" ) + config.getXString( nodePath + ".output.data" ),
													config, config.getXString( nodePath + ".input:url" ) ) );



	// Setup the centrality bins
   	INFO( classname(),  "Loading Centrality Map" );
    centralityBinMap = config.getIntMap( nodePath + ".CentralityMap" );
    centralityBins = config.getIntVector( nodePath + ".CentralityBins" );
    INFO( classname(), "c[ 0 ] = " << centralityBinMap[ 0 ] );


    reporter = unique_ptr<Reporter>( new Reporter( config, nodePath + ".Reporter." ) );

}


void FeedDownFitter::make(){
	
	RooPlotLib rpl; 

	vector<string> hNames = { "#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "Proton", "#bar{P}" };

	string path = config.getXString( nodePath + ".output.param" );
	ofstream out( path.c_str(), ios::out );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;

	gStyle->SetOptFit( 1111 );

	int plcIndex = 0;
	for ( auto k : plcName ){
		

		if ( config.getBool( nodePath + ".Pages:parents", false ) ){
			INFO( classname(), "parents" );
			TH2 * h2d = book->get2D( "parents_" + k.second );
			for ( int i = 1; i <= h2d->GetNbinsY(); i++ ){
				
				TH1 * projX = book->get2D( "parents_" + k.second )->ProjectionX( "_px", i, i+1);
				TH1 * projY = book->get2D( "parents_" + k.second )->ProjectionY( "_py");
				float ptl = projY->GetBinLowEdge( i );
				float pth = ptl + projY->GetBinWidth(i);
				// Feeddown - contributions
				reporter->newPage();
				
				addGEANTLabels( projX );
				projX->GetXaxis()->LabelsOption( " " );
				rpl.style( projX ).set( config, "Style.logy1D" ).set( "title", hNames[ plcIndex ] + " : " + dts( ptl ) + " < pT < " + dts( pth ) ).draw();

				reporter->savePage();

				if ( config.getBool( nodePath + ".Pages:export", false ) )
					reporter->saveImage( config.getXString( nodePath + ".output.export" ) + "/img/feeddown_" + k.second + "_" + dts( ptl ) + "_pT_" + dts( pth ) + ".pdf"  );
			}
		}

		if ( "K_p" == k.second || "K_n" == k.second /*|| "P_n" == k.second || "P_p" == k.second || "Pi_n" == k.second*/ ){
			plcIndex ++;
			continue;
		}
		

		out << "\t<" << k.second << ">" << endl;
		background( k.second, plcIndex, -1, out );	
		for ( int b : config.getIntVector( nodePath + ".CentralityBins" ) ) {
			background( k.second, plcIndex, b, out );	
		}
		out << "\t</" << k.second << ">" << endl;

		plcIndex ++;
	}
	
	out << "</config>" << endl;
	out.close();
}

void FeedDownFitter::exportParams( int bin, TF1 * fn, TFitResultPtr result,  ofstream &out){
	out << "\t\t<FeedDownParams ";
	out << "bin=\"" << bin << "\" ";
	out << Common::toXml( fn, result );
	out << "/>" << endl;
}

void FeedDownFitter::background( string name, int plcIndex, int bin, ofstream &out ){
	INFO( classname(), "(name=" << name << ", plcIndex=" << plcIndex << ", bin=" << bin << ")" );

	gStyle->SetOptFit( 11 );


	RooPlotLib rpl; 

	vector<string> hNames = { "#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "Proton", "#bar{P}" };

	reporter->newPage();
	reporter->cd(0);

	// Upper pad for the data points +  the fit
	TPad * p1 = new TPad( "fit", "fit", 0.001, 0.3, 0.99, 0.99 );
	p1->SetBottomMargin( 0.1 );
	p1->SetLeftMargin( 0.1 );

	// Lower pad with the data / fit ratio
	TPad * p2 = new TPad( "ratio", "ratio", 0.001, 0.01, 0.99, 0.3 );
	p2->SetTopMargin( 0.0 );
	p2->SetLeftMargin( 0.1 );
	p1->Draw();
	p2->Draw();
	p1->cd();
	
	
	
	TGraphAsymmErrors * g = new TGraphAsymmErrors();



	TH1 * back = nullptr;
	TH1 * total = nullptr;

	if ( bin >= 0 ){
		back  = book->get( "sig_" + name +"_" + ts(bin) );
		total = book->get( "spectra_" + name + "_" + ts(bin) );
	} else {
		back  = book->get( "sig_" + name  );
		total = book->get( "spectra_" + name  );
	}
	//back->Draw();
	//p1->Draw();

	// back->Rebin(2);
	// total->Rebin(2);

	HistoBins ptFeedDown( config, "binning.ptFeedDown" );
	back = back->Rebin( ptFeedDown.nBins(), "", ptFeedDown.getBins().data() );
	total = total->Rebin( ptFeedDown.nBins(), "", ptFeedDown.getBins().data() );

	// back->Sumw2();
	// total->Sumw2();

	g->Divide( back, total );

	rpl.style( g ).set( config, "Style.frac_" + name ).set( config, "Style.frac" ).set( "title", hNames[ plcIndex ] + " : bin " + ts(bin) ).draw();

	INFO( "Fitting to : " << formulas[ plcIndex ]  )
	TF1 * fracFun = new TF1( "fn", formulas[ plcIndex ].c_str() , 0.01, 4.5 );
	
	// fracFun->SetParLimits( 0, 0, 10 );
	fracFun->SetParameters( config.getDouble( nodePath + ".FitRange:p0", 1 ), 
							config.getDouble( nodePath + ".FitRange:p1", 1 ), 
							config.getDouble( nodePath + ".FitRange:p2", 1 ));

	fracFun->SetParLimits( 0, 0, 1.0 );
	fracFun->SetParLimits( 1, 0, 10 );
	fracFun->SetParLimits( 2, 0, 1000 );
	

	double fMin = config.getDouble( nodePath + ".FitRange:min", 0.0 );
	double fMax = config.getDouble( nodePath + ".FitRange:max", 1.5 );
	g->Fit( fracFun, "RNQ", "", fMin, fMax );
	g->Fit( fracFun, "RNQ", "", fMin, fMax );
	TFitResultPtr fitPointer = g->Fit( fracFun, "QRSM", "", fMin, fMax );

	
	// TGraphErrors * band = Common::choleskyBands( fitPointer, fracFun, 500, 100, reporter.get() );
	// TH1 * band = FitConfidence::fitCL( fracFun, "uncer_band", 0.95 );
	TGraphAsymmErrors * band = FitConfidence::fitUncertaintyBand( fracFun, 0.05, 0.05, 100 );

	fracFun->SetRange( 0.0, 5 );
	
	// dont export the inclusive centrality one
	if ( bin >= 0 )
		exportParams( bin, fracFun, fitPointer,  out );

	book->cd( "results" );
	g->SetName( ("y" + name + "_" + ts(bin) ).c_str() );
	book->add( name + "_" + ts(bin), g );
	book->cd("");

	fracFun->Draw("same");

	band->SetFillColorAlpha( kRed, 0.5 );
	band->Draw( "same e3" );

	TLatex latex;
	latex.SetTextSize(0.08);
	latex.SetTextAlign(13);  //align at top
	latex.DrawLatex(1,0.21,"f(p_{T}) = p0 e^{-(p1/p_{T})^{p2}}");


	p2->cd();
	back->Divide( total );
	TH1 * ratio = (TH1*)back->Clone( "ratio" );

	gStyle->SetStatY( 0.5 );
	gStyle->SetStatH( 0.2 );
	gStyle->SetStatX( 0.85 );


	TGraphAsymmErrors *gRatio = new TGraphAsymmErrors( g->GetN() );
	
	float mfE = 0.05; // min enforced fit error
	for ( int iG = 0; iG < g->GetN(); iG++ ){
		float x = g->GetX()[iG];
		float y = g->GetY()[iG];
		float ym = fracFun->Eval( x ); 

		gRatio->SetPoint( iG, x, y / ym );
		gRatio->SetPointEYlow( iG, sqrt( mfE*mfE + g->GetEYhigh()[iG]*g->GetEYhigh()[iG] ) );
		gRatio->SetPointEYhigh( iG, sqrt( mfE*mfE + g->GetEYhigh()[iG]*g->GetEYhigh()[iG] ) );
	}

	rpl.style( gRatio ).set( config, "Style.ratio" ).draw();
	gRatio->GetYaxis()->SetNdivisions( 5, 2, 0, true );
	gRatio->GetYaxis()->SetTickLength( 0.01 );
	

	TLine * l = new TLine( 0.0, 1, 2.0, 1  );
	l->SetLineStyle( kDotted );
	l->Draw("same");
	

	if ( config.getBool( nodePath + "Pages:export", true ) )
		reporter->saveImage( config.getXString( nodePath + ".output.export" ) + "/img/feeddown_fit_" + name + "_back_" + ts(bin) + ".pdf" );
	reporter->savePage();
}