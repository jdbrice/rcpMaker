#include "McMaker/TofEffFitter.h"
#include "Common.h"
#include "XmlBinnedData.h"

// ROOT
#include "TGraphAsymmErrors.h"
#include "TFile.h"

#include "Reporter.h"
#include "RooPlotLib.h"
using namespace jdb;


void TofEffFitter::initialize( ){
	HistoAnalyzer::initialize();
	
	outputPath = config.getString( nodePath + ".output:path" );
	book = unique_ptr<HistoBook>( 
				new HistoBook( 
					outputPath + config.getString( nodePath +  ".output.data", "TofEff.root" ), 
					config, 
					"", 
					"" 
				) 
			);	

}



void TofEffFitter::make(){
	INFOC( "" );

	RooPlotLib rpl;

	string params_file =  config.getString( nodePath + ".output.params" );
	if ( "" == params_file ){
		ERROR( "Specifiy an output params file for the parameters" )
		return;
	}


	book->cd();

	INFOC( "Opening " << params_file );
	ofstream out( params_file.c_str() );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;



	vector<string> labels = config.getStringVector( nodePath + ".CentralityLabels" );
	vector< int> cbins = config.getIntVector( nodePath + ".CentralityBins" );
	Reporter rp( config, nodePath + ".Reporter." );

	for ( string plc : Common::species ){
		string fn = config.getString( nodePath + ".input:url" ) + "TofEff_" + plc + ".root";
		INFOC( "Opening " << fn );
		TFile * f = new TFile( fn.c_str(), "READ" );
		INFOC( "Opened? " << fn );

		if ( nullptr == f ||  !f->IsOpen() ){
			ERRORC( "Cannot open " << fn );
			continue;
		}
		
		for ( string cs : Common::sCharges ) {

			out << "\t<" << plc << "_" << cs << ">" << endl;

			// build an efficiency for each centrality
			for ( int b : cbins ){
				INFOC( "Working on " << plc << "_" << cs << "_" << b );
				TH1 * hAll = (TH1*)f->Get( ("inclusive/pt_" + ts( b ) + "_" + cs).c_str() );
				TH1 * hPass = (TH1*)f->Get( ("inclusiveTof/pt_" + ts( b ) + "_" + cs ).c_str() );	

				if ( nullptr == hAll || nullptr == hPass ){
					ERRORC( "Null Histo " );
					continue;
				}

				hAll->Sumw2();
				hPass->Sumw2();

				book->cd();
				TH1 * hEff = (TH1*)hPass->Clone( (plc + "_" + cs + "_" + ts(b)).c_str() );

				// TGraphAsymmErrors g;

				// g.SetName( (plc + "_" + cs + "_" + ts(b)).c_str() );
				// g.BayesDivide( hPass, hAll );
				hEff->Divide( hAll );

				book->add( plc + "_" + cs + "_" + ts(b),  hEff );

				rp.newPage();
				rpl.style( hEff )
					.set( "title", Common::plc_label( plc, cs ) + " : " + labels[ b ] )
					.set( "yr", 0.0, 0.85 )
					.set( "optfit", 111 )
					.set("y", "TOF Matching Efficiency")
					.set( "x", "p_{T} [GeV/c]" )
					.set( config, nodePath + ".Style.TofEff" )
					.draw();
				
				gStyle->SetStatY( 0.9 );
				gStyle->SetStatX( 0.65 );
					
				rp.savePage();

				string imgName = outputPath + "/img/TofEff_" + plc + "_" + cs + "_" + ts(b) + ".png";
				INFO( classname(), "Exporting image to : " << imgName );
				rp.saveImage( imgName );

				imgName = outputPath + "/img/TofEff_" + plc + "_" + cs + "_" + ts(b) + ".pdf";
				INFO( classname(), "Exporting image to : " << imgName );
				rp.saveImage( imgName );

				// exportParams( b, hAll, &g, out );

			} // loop centrality bins
			out << "\t</" << plc << "_" << cs << ">" << endl;
		}
	} // loop over species

	out << "</config>" << endl;
	out.close();


}


void TofEffFitter::exportParams( int bin, TH1 * h, TGraphAsymmErrors * g, ofstream &out ){
	out << "\t\t<TofEffParams bin=\"" << bin << "\" >\n";
	XmlBinnedData xbd( h, g );
	//out << Common::toXml( g, "\t\t\t" );
	out << xbd.toXml( "\t\t\t" );
	out << "\n\t\t</TofEffParams>" << endl;
}