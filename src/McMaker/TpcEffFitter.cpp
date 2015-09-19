#include "McMaker/TpcEffFitter.h"
#include "Common.h"

// ROOT
#include "TGraphAsymmErrors.h"
#include "TFile.h"

#include "Logger.h"
#include "Reporter.h"
#include "RooPlotLib.h"
using namespace jdb;


TpcEffFitter::TpcEffFitter( XmlConfig * _cfg, string _nodePath ){
	DEBUG( "( " << _cfg << ", " << _nodePath << " )" )
	cfg = _cfg;
	nodePath = _nodePath;
	outputPath = cfg->getString( nodePath + "output:path", "" );

	book = unique_ptr<HistoBook>( new HistoBook( outputPath + cfg->getString( nodePath +  "output.data", "TpcEff.root" ), cfg, "", "" ) );	
}



void TpcEffFitter::make(){
	DEBUG("")

	gStyle->SetOptFit( 111 );
	string params_file =  cfg->getString( nodePath + "output.params" );
	if ( "" == params_file ){
		ERROR( "Specifiy an output params file for the parameters" )
		return;
	}

	ofstream out( params_file.c_str() );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;


	vector<string> labels = cfg->getStringVector( nodePath + "CentralityLabels" );
	vector< int> cbins = cfg->getIntVector( nodePath + "CentralityBins" );
	Reporter rp( cfg, nodePath + "Reporter." );

	DEBUG( "Starting plc loop" )
	for ( string plc : Common::species ){
		if ( "E" == plc || "D" == plc )
			continue;
		for ( string c : Common::sCharges ){

			out << "\t<" << plc << "_" << c << ">" << endl;

			string fnMc = cfg->getString( nodePath + "input:url" ) + "TpcEff_" + plc + "_" + c + "_mc" + ".root";
			TFile * fmc = new TFile( fnMc.c_str(), "READ" );
			string fnRc = cfg->getString( nodePath + "input:url" ) + "TpcEff_" + plc + "_" + c + "_rc" + ".root";
			TFile * frc = new TFile( fnRc.c_str(), "READ" );

			DEBUG( "Mc File = " << fmc )
			DEBUG( "Rc File = " << frc )
			if ( !fmc->IsOpen() || !frc->IsOpen() )
				continue;
			

			// build an efficiency for each centrality
			for ( int b : cbins ){

				TH1 * hMc = (TH1*)fmc->Get( ("inclusive/pt_" + ts( b ) + "_" + c ).c_str() );
				TH1 * hRc = (TH1*)frc->Get( ("inclusive/pt_" + ts( b ) + "_" + c ).c_str() );

				INFO( tag, "N bins MC = " << hMc->GetNbinsX() );
				INFO( tag, "N bins RC = " << hRc->GetNbinsX() );

				for ( int i = 0; i <= hMc->GetNbinsX() + 1; i++ ){
					if ( hMc->GetBinContent( i ) < hRc->GetBinContent( i ) ){
						// set to 100%
						if ( i > 5 )
							hMc->SetBinContent( i, hRc->GetBinContent( i ) );
						else{
							hRc->SetBinContent( i, 0 );
							hMc->SetBinContent( i, 0 );
						}
					}
				}
					

				hMc->Sumw2();
				hRc->Sumw2();

				TGraphAsymmErrors g;

				g.SetName( (plc + "_" + c + "_" + ts(b)).c_str() );
				g.BayesDivide( hRc, hMc );

				book->add( plc + "_" + c + "_" + ts(b),  &g );

				// do the fit
				TF1 * fitFunc = new TF1( "effFitFunc", "[0] * exp( - pow( [1] / x, [2] ) )", 0.0, 5.0 );
				fitFunc->SetParameters( .85, 0.05, 5.0, -0.05 );
				fitFunc->SetParLimits( 0, 0.5, 1.0 );
				fitFunc->SetParLimits( 1, 0.0, 0.5 );
				fitFunc->SetParLimits( 2, 0.0, 100000 );

				// fist fit shape
				TFitResultPtr fitPointer = g.Fit( fitFunc, "RSWW" );

				// fitFunc->FixParameter( 1, fitFunc->GetParameter( 1 ) );
				// fitFunc->FixParameter( 2, fitFunc->GetParameter( 2 ) );
				fitPointer = g.Fit( fitFunc, "RS" );

				// fitFunc->ReleaseParameter( 1 );
				// fitFunc->ReleaseParameter( 2 );

				INFO( tag, "FitPointer = " << fitPointer );
				TGraphErrors * band = Common::choleskyBands( fitPointer, fitFunc, 5000, 200, &rp );



				RooPlotLib rpl;
				rp.newPage();
				rpl.style( &g ).set( "title", Common::plc_label( plc, c ) + " : " + labels[ b ] + ", 68%CL (Red)" ).set( "yr", 0, 1.1 ).set( "optfit", 111 )
					.set( "xr", 0, 4.5 )
					.set("y", "Efficiency x Acceptance").set( "x", "p_{T}^{MC} [GeV/c]" ).draw();


				INFO( tag, "Stat Box" );
				gStyle->SetStatY( 0.5 );
				gStyle->SetStatX( 0.85 );
				
				fitFunc->SetLineColor( kRed );
				fitFunc->Draw("same");	

				INFO( tag, "Drawing CL band" );
				// TH1 * band2 = Common::fitCL( fitFunc, "bands", 0.95 );
				// band2->SetFillColorAlpha( kBlue, 0.5 );
				band->SetFillColorAlpha( kRed, 0.5 );
				// band2->Draw( "same e3" );
				band->Draw( "same e3" );


				rp.savePage();

				INFO( tag, "Exporting Params" );
				exportParams( b, fitFunc, fitPointer, out );

			} // loop centrality bins

			out << "\t</" << plc << "_" << c << ">" << endl;

		}
	}

	out << "</config>" << endl;
	out.close();


}

void TpcEffFitter::exportParams( int bin, TF1 * f, TFitResultPtr result, ofstream &out ){
	INFO( tag, "(bin=" << bin << ", f=" << f << ", fPtr=" << result << " )" )
	out << "\t\t<TpcEffParams bin=\"" << bin << "\" ";
	out << Common::toXml( f, result );
	out << "/>" << endl;
}