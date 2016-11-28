#include "McMaker/FeedDownFitter.h"

// Roobarb
#include <RooPlotLib.h>

// ROOT
#include "TGraphAsymmErrors.h"
#include "TLine.h"

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

	formulas =	{ "[0]*pow( x, -abs( [1] ) ) + [2] * exp( -[3] * x )",
				"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
				"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
				"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x )",
				"[0]*exp( -[1] * x ) + [2] * exp( -[3] * x * x )",
				"pow( [0] + [1] * pow( x, [2] ), -1 )" };

	rmb = unique_ptr<HistoBins>( new HistoBins( config, nodePath + ".RefMultBins" ) );

	book = unique_ptr<HistoBook>( new HistoBook( config.getString( nodePath + ".output:path" ) + config.getString( nodePath + ".output.data" ),
													config, config.getString( nodePath + ".input:url" ) ) );



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

	string path = config.getString( nodePath + ".output.param" );
	ofstream out( path.c_str(), ios::out );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;

	gStyle->SetOptFit( 1111 );

	int plcIndex = 0;
	for ( auto k : plcName ){
		

		if ( config.getBool( nodePath + ".Pages:parents", false ) ){
			INFO( classname(), "parents" );
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
				rpl.style( projX ).set( &config, ".Style.logy1D" ).set( "title", hNames[ plcIndex ] + " : " + dts( ptl ) + " < pT < " + dts( pth ) ).draw();

				reporter->savePage();

				if ( config.getBool( nodePath + ".Pages:export", false ) )
					reporter->saveImage( config.getString( nodePath + ".output.export" ) + "/img/feeddown_" + k.second + "_" + dts( ptl ) + "_pT_" + dts( pth ) + ".pdf"  );
			}
		}

		if ( "K_p" == k.second || "K_n" == k.second /*|| "P_n" == k.second || "P_p" == k.second || "Pi_n" == k.second*/ ){
			plcIndex ++;
			continue;
		}
		

		out << "\t<" << k.second << ">" << endl;
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

	rpl.style( g ).set( &config, ".Style.frac_" + name ).set( &config, ".Style.frac" ).set( "title", hNames[ plcIndex ] + " : bin " + ts(bin) ).draw();

	INFO( "Fitting to : " << formulas[ plcIndex ]  )
	TF1 * fracFun = new TF1( "fn", formulas[ plcIndex ].c_str() , 0.01, 4.5 );
	
	if ( "Pi_n" == name ){
		fracFun->SetParLimits( 0, 0, 1 );
		fracFun->SetParLimits( 1, 0, 50 );
		fracFun->SetParLimits( 2, 0, 1 );
		fracFun->SetParLimits( 3, 0, 50 );

		fracFun->SetParameters( .1, 0.005, .01, 0.002, 0.1, 0.002 );

	} else  if ( "Pi_p" == name ){
		INFO( name )
		fracFun->SetParameters( 0.025, 0.75, 0.25, 4 );

		fracFun->SetParLimits( 0, 0.0, 1 );
		fracFun->SetParLimits( 1, 0, 5 );
		fracFun->SetParLimits( 2, 0, 1 );
		fracFun->SetParLimits( 3, 0, 50 );
	} else if ( "P_p" == name ){
		
		fracFun->SetParLimits( 0, 0, 1 );
		fracFun->SetParLimits( 1, 0, 50 );
		fracFun->SetParLimits( 2, 0, 1 );
		fracFun->SetParLimits( 3, 0, 50 );

		fracFun->SetParameters( .1, 0.005, .01, 0.002 );
		
	} else if ( "P_n" == name ){
		
		fracFun->SetParLimits( 0, 0, 2 );
		fracFun->SetParLimits( 1, 0, 10 );
		fracFun->SetParLimits( 2, 0, 2 );
	
		fracFun->SetParameters( 1, 5.0, 1.1 );
		
	}

	g->Fit( fracFun, "RNQ" );
	g->Fit( fracFun, "RNQ" );
	TFitResultPtr fitPointer = g->Fit( fracFun, "QRS" );

	
	TGraphErrors * band = Common::choleskyBands( fitPointer, fracFun, 5000, 200, reporter.get() );

	fracFun->SetRange( 0.0, 5 );
	exportParams( bin, fracFun, fitPointer,  out );

	book->cd( "results" );
	g->SetName( ("y" + name + "_" + ts(bin) ).c_str() );
	book->add( name + "_" + ts(bin), g );
	book->cd("");

	fracFun->Draw("same");

	band->SetFillColorAlpha( kRed, 0.5 );
	band->Draw( "same e3" );


	p2->cd();
	back->Divide( total );
	TH1 * ratio = (TH1*)back->Clone( "ratio" );

	gStyle->SetStatY( 0.9 );
	gStyle->SetStatX( 0.65 );

	
	for ( int i = 2; i < ratio->GetNbinsX() + 1; i++ ){
		float bc = ratio->GetBinCenter( i );
		float fv = fracFun->Eval( bc );
		float bv = g->GetY()[ i - 3 ];
		float be = g->GetErrorYlow( i - 3 ) ;
		
	
		INFO( "stupid", "value = " << (bv/fv) << ", bv=" << bv << ", fv ="<< fv <<", be=" << be );
		if ( isinf( bv ) || isinf( bv/fv ) || isinf( be ) || isinf( be/fv ) || be < 0 )
			continue;
		ratio->SetBinContent( i, bv / fv );

		ratio->SetBinError( i, be / fv );
	}
	ratio->GetYaxis()->SetNdivisions( 5, 2, 0, true );
	ratio->GetYaxis()->SetTickLength( 0.01 );

	rpl.style( ratio ).set( &config, ".Style.ratio" ).draw();
	

	TLine * l = new TLine( 0.0, 1, 3, 1  );
	l->SetLineStyle( kDotted );
	l->Draw("same");
	

	if ( config.getBool( nodePath + "Pages:export", true ) )
		reporter->saveImage( config.getString( nodePath + ".output.export" ) + "/img/feeddown_fit_" + name + "_back_" + ts(bin) + ".pdf" );
	reporter->savePage();
}