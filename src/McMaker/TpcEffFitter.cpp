#include "McMaker/TpcEffFitter.h"
#include "Common.h"

// ROOT
#include "TGraphAsymmErrors.h"
#include "TFile.h"
#include "TLatex.h"

#include "Logger.h"
#include "Reporter.h"
#include "RooPlotLib.h"
using namespace jdb;


void TpcEffFitter::initialize(  ){

	DEBUG( classname(), "( " << config.getFilename() << ", " << nodePath << " )" )
	outputPath = config.getString( nodePath + ".output:path", "" );
	
	book = unique_ptr<HistoBook>( new HistoBook( outputPath + config.getString( nodePath +  ".output.data", "TpcEff.root" ), config, "", "" ) );	
}



void TpcEffFitter::make(){
	DEBUG(classname(), "")

	RooPlotLib rpl;

	gStyle->SetOptFit( 111 );
	string params_file =  config.getString( nodePath + ".output.params" );
	if ( "" == params_file ){
		ERROR( classname(), "Specifiy an output params file for the parameters" )
		return;
	}

	ofstream out( params_file.c_str() );

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	out << "<config>" << endl;


	vector<string> labels = config.getStringVector( nodePath + ".CentralityLabels" );
	vector< int> cbins = config.getIntVector( nodePath + ".CentralityBins" );
	Reporter rp( config, nodePath + ".Reporter." );

	double minP0Error = config.getDouble( nodePath + ".Systematics:minP0Error" );
	TLatex Tl;  
	Tl.SetTextSize(40);

	DEBUG( "Starting plc loop" )
	for ( string plc : Common::species ){
		if ( "E" == plc || "D" == plc )	// skip Electrons and Deuterons if I've got those in the list
			continue;

		for ( string c : Common::sCharges ){

			out << "\t<" << plc << "_" << c << ">" << endl;

			string fnMc = config.getString( nodePath + ".input:url" ) + "TpcEff_" + plc + "_" + c + "_mc" + ".root";
			TFile * fmc = new TFile( fnMc.c_str(), "READ" );
			string fnRc = config.getString( nodePath + ".input:url" ) + "TpcEff_" + plc + "_" + c + "_rc" + ".root";
			TFile * frc = new TFile( fnRc.c_str(), "READ" );

			DEBUG( classname(), "Mc File = " << fmc );
			DEBUG( classname(), "Rc File = " << frc );


			if ( !fmc->IsOpen() || !frc->IsOpen() )
				continue;
			

			// build an efficiency for each centrality
			for ( int b : cbins ){

				TH1 * hMc = (TH1*)fmc->Get( ("inclusive/pt_" + ts( b ) + "_" + c ).c_str() );
				TH1 * hRc = (TH1*)frc->Get( ("inclusive/pt_" + ts( b ) + "_" + c ).c_str() );

				INFO( classname(), "N bins MC = " << hMc->GetNbinsX() );
				INFO( classname(), "N bins RC = " << hRc->GetNbinsX() );

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

				INFO( classname(), "Adding TGraphAsymmErrors to book" );
				book->add( plc + "_" + c + "_" + ts(b),  &g );

				// do the fit
				TF1 * fitFunc = new TF1( "effFitFunc", "[0] * exp( - pow( [1] / x, [2] ) )", 0.0, 5.0 );
				fitFunc->SetParameters( .85, 0.05, 5.0, -0.05 );
				fitFunc->SetParLimits( 0, 0.5, 1.0 );
				fitFunc->SetParLimits( 1, 0.0, 0.5 );
				fitFunc->SetParLimits( 2, 0.0, 100000 );

				// fist fit shape
				TFitResultPtr fitPointer = g.Fit( fitFunc, "RSWW" );
				fitPointer = g.Fit( fitFunc, "RS" );

				// ensure that uncertainty on efficiency is at least X%
				if ( fitFunc->GetParError( 0 ) < minP0Error ) {
					INFO( classname(), "P0 uncertainty below threshold (" << (minP0Error * 100) << "%" );
					INFO( classname(), "Setting P0 error to " << (minP0Error * 100) << "%" );
					fitFunc->SetParError( 0, minP0Error );
				}

				INFO( classname(), "FitPointer = " << fitPointer );
				TGraphErrors * band = Common::choleskyBands( fitPointer, fitFunc, 5000, 200, &rp );
				

				rp.newPage();
				rpl.style( &g ).set( "title", Common::plc_label( plc, c ) + " : " + labels[ b ] + ", Fit 68%CL(Red Band)" )
					.set( "yr", 0, 1.1 ).set( "optfit", 111 )
					.set( "xr", 0, 4.5 )
					.set("y", "Efficiency x Acceptance")
					.set( "x", "p_{T}^{MC} [GeV/c]" )
					.draw();


				INFO( classname(), "Stat Box" );
				gStyle->SetStatY( 0.5 );
				gStyle->SetStatX( 0.85 );
				
				fitFunc->SetLineColor( kRed );
				fitFunc->Draw("same");	

				INFO( classname(), "Drawing CL band" );
				
				band->SetFillColorAlpha( kRed, 0.7 );
				band->Draw( "same e3" );

				Tl.DrawLatex( 2, 0.6, "f(pT) = [p0]e^{ (#frac{-[p1]}{pT})^{[p2]} }" );


				rp.savePage();
				string imgName = outputPath + "/img/TpcEff_" + plc + "_" + c + "_" + ts(b) + ".png";
				INFO( classname(), "Exporting image to : " << imgName );
				rp.saveImage( imgName );

				INFO( classname(), "Exporting Params" );
				exportParams( b, fitFunc, fitPointer, out );

			} // loop centrality bins

			out << "\t</" << plc << "_" << c << ">" << endl;

		} // loop on charge
	} // loop on plc

	out << "</config>" << endl;
	out.close();


}

void TpcEffFitter::exportParams( int bin, TF1 * f, TFitResultPtr result, ofstream &out ){
	INFO( classname(), "(bin=" << bin << ", f=" << f << ", fPtr=" << result << " )" )
	out << "\t\t<TpcEffParams bin=\"" << bin << "\" ";
	out << Common::toXml( f, result );
	out << "/>" << endl;
}