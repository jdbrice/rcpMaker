#include "McMaker/TofEffFitter.h"
#include "Common.h"

// ROOT
#include "TGraphAsymmErrors.h"
#include "TFile.h"

#include "Reporter.h"
#include "RooPlotLib.h"
using namespace jdb;


TofEffFitter::TofEffFitter( XmlConfig * _cfg, string _nodePath ){

	cfg = _cfg;
	nodePath = _nodePath;
	outputPath = cfg->getString( nodePath + "output:path" );

	book = unique_ptr<HistoBook>( new HistoBook( outputPath + cfg->getString( nodePath +  "output.data", "TofEff.root" ), cfg, "", "" ) );	

}



void TofEffFitter::make(){


	string params_file =  cfg->getString( nodePath + "output.params" );
	if ( "" == params_file ){
		ERROR( "Specifiy an output params file for the parameters" )
		return;
	}

	ofstream out( params_file.c_str() );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;



	vector< int> cbins = cfg->getIntVector( nodePath + "CentralityBins" );
	Reporter rp( cfg, nodePath + "Reporter." );

	for ( string plc : Common::species ){
		
		

		string fn = cfg->getString( nodePath + "input:url" ) + "TofEff_" + plc + ".root";
		TFile * f = new TFile( fn.c_str(), "READ" );
		

		if ( !f->IsOpen() )
			continue;
		
		for ( string cs : Common::sCharges ) {

			out << "\t<" << plc << "_" << cs << ">" << endl;

			// build an efficiency for each centrality
			for ( int b : cbins ){

				TH1 * hAll = (TH1*)f->Get( ("inclusive/pt_" + ts( b ) + "_" + cs).c_str() );
				TH1 * hPass = (TH1*)f->Get( ("inclusiveTof/pt_" + ts( b ) + "_" + cs ).c_str() );	

				hAll->Sumw2();
				hPass->Sumw2();

				TGraphAsymmErrors g;

				g.SetName( (plc + "_" + cs + "_" + ts(b)).c_str() );
				g.BayesDivide( hPass, hAll );

				book->add( plc + "_" + cs + "_" + ts(b),  &g );

				// do the fit
				// TF1 * fitFunc = new TF1( "effFitFunc", "[0] * exp( -pow( [1] / (x), [2] ) )", 0.2, 0.6 );
				// fitFunc->SetParameters( .6, 0.1, 4.0, 0.6);
				// fitFunc->SetParLimits( 0, 0.2, 0.99 );
				// fitFunc->SetParLimits( 1, 0.0, 0.15 );
				// fitFunc->SetParLimits( 2, 0.0, 30.0 ); 
				
				// g.Fit( fitFunc, "R" );

				// cout << Common::toXml( &g ) << endl;
				//g.Fit( fitFunc, "R" );
				// fitFunc->SetRange( 0, 5 );

				// string fitName = "pt_" + ts( b ) + "_" + cs + "_CL";
				// TH1F * hCL = (TH1F*)Common::fitCL( fitFunc, fitName, 100, 0.2, 5 );

				RooPlotLib rpl;
				rp.newPage();
				rpl.style( &g ).set( "title", plc + "_" + cs + " : centrality bin = " + ts( b ) ).set( "yr", 0.4, 0.85 ).set( "optfit", 111 )
					.set("y", "Efficiency").set( "x", "p_{T} [GeV/c]" ).draw();
					gStyle->SetStatY( 0.9 );
					gStyle->SetStatX( 0.65 );
					
					// hCL->SetFillColorAlpha( kRed, 0.66 );
					// hCL->Draw( "same e3" );

					// fitFunc->SetLineColor( kRed );
					// fitFunc->Draw("same");

				rp.savePage();

				exportParams( b, &g, out );

			} // loop centrality bins
			out << "\t</" << plc << "_" << cs << ">" << endl;
		}
	} // loop over species

	out << "</config>" << endl;
	out.close();


}


void TofEffFitter::exportParams( int bin, TGraphAsymmErrors * g, ofstream &out ){
	out << "\t\t<TofEffParams bin=\"" << bin << "\" >\n";
	out << Common::toXml( g, "\t\t\t" );
	out << "\n\t\t</TofEffParams>" << endl;
}