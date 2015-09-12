#include "Utils.h"
#include "RooPlotLib.h"
#include "Bichsel.h"

double mE = 0.000510998;
double mPi = 0.1395702;
double mK = 0.493667;
double mP = 0.9382721;

void set_plot_style()
{
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
}


double one_beta( double m, double p ){
	return sqrt( 1 + pow( m/p, 2 ) );
}

void setlines( TLine * l, int c, int w  ){
	l->SetLineColor( c );
	l->SetLineWidth( w );
}


double sigTof = 0.012;
double sigTpc = 0.07;
void drawTofLines( double center, int c, double x1, double x2 ){
	float n = 10;
	TLine * l1 = new TLine( x1, center-sigTof*n, x2, center-sigTof*n );
	setlines( l1, c, 2 );
	TLine * l2 = new TLine( x1, center+sigTof*n, x2, center+sigTof*n );
	setlines( l2, c, 2 );

	l1->Draw("same");
	l2->Draw("same");
}

void drawTpcLines( double center, int c, double y1, double y2 ){
	TLine * l1 = new TLine(center-sigTpc*3, y1, center-sigTpc*3, y2 );
	setlines( l1, c, 2 );
	TLine * l2 = new TLine(center+sigTpc*3, y1, center+sigTpc*3, y2 );
	setlines( l2, c, 2 );

	l1->Draw("same");
	l2->Draw("same");
}

void make( string charge = "p", bool lines = false, int iCen = 0, string cs = "Pi", string hFile = "histograms.root" ){

	Bichsel bgen;
	gStyle->SetPadLeftMargin(0.16);
	gStyle->SetPadBottomMargin(0.12);
	gStyle->SetPadRightMargin(0.12);

	set_plot_style();

	RooPlotLib rpl;
	string rpName = "rp_" + charge + "_dedx_beta_lines.pdf";
	if ( false == lines )
		rpName = "rp_" + charge + "_dedx_beta.pdf";

	string chargeName = "Positive Tracks : ";
	if ( "n" == charge )
		chargeName = "Negative Tracks : ";

	Reporter rp( rpName, 1000, 800 );

	TFile * f = new TFile( hFile.c_str(), "READ" );

	TH1D * nlBeta = (TH1D*)f->Get( "nlBeta" );


	for ( int iPt = 0; iPt < 100; iPt ++ ){

		TAxis * x = nlBeta->GetXaxis();
		double lpT = x->GetBinLowEdge( iPt + 1 );
		double hpT = x->GetBinLowEdge( iPt + 1 ) + x->GetBinWidth( iPt + 1 );
		double avgP = (lpT + hpT) / 2.0;

		string name = "dedx_tof/dedx_tof_" + cs + "_" + charge + "_" + ts(iCen) + "_" + ts( iPt);
		TH2D * h2 = (TH2D*)f->Get( name.c_str() );

		if ( !h2 )
			break;
		if ( iPt == 5 ) // what went wrong?
			continue;


		double x1 = h2->GetXaxis()->GetBinLowEdge( 1 );
		double x2 = h2->GetXaxis()->GetBinLowEdge( h2->GetNbinsX() ) + h2->GetXaxis()->GetBinWidth( h2->GetNbinsX() );
		double y1 = h2->GetYaxis()->GetBinLowEdge( 1 );
		double y2 = h2->GetYaxis()->GetBinLowEdge( h2->GetNbinsY() ) + h2->GetYaxis()->GetBinWidth( h2->GetNbinsY() );

		  

		rpl.style( h2 ).set( "draw", "col" )
			.set( "optstat", 0 ).set( "logz", 1 )
			.set( "title", chargeName + dts(lpT) + " < p_{T} [GeV/c] < " + dts(hpT) +"; ln(dE/dx) - ln(dE/dx)_{#pi}; #beta^{-1} - #beta^{-1}_{#pi}" )
			.set( "yoffset", 0.01 ).draw(); 

		if ( lines  ){
			double bKaon = one_beta( mK, avgP ) - one_beta( mPi, avgP );
			double bProton = one_beta( mP, avgP ) - one_beta( mPi, avgP );

			double dKaon = bgen.meanLog( avgP, mK, -1, 1000 ) - bgen.meanLog( avgP, mPi, -1, 1000 );
			double dProton = bgen.meanLog( avgP, mP, -1, 1000 ) - bgen.meanLog( avgP, mPi, -1, 1000 );

			drawTofLines( 0, kRed+1, x1, x2 );		
			drawTpcLines( 0, kRed + 1, y1, y2);
			// around Kaons
			drawTofLines( bKaon, kOrange+1, x1, x2 );	
			drawTpcLines( dKaon, kOrange + 1, y1, y2);
			// around Protons
			drawTofLines( bProton, kBlack, x1, x2 );	
			drawTpcLines( dProton, kBlack, y1, y2);
		}

		// double dElec = bgen.meanLog( avgP, mE, -1, 1000 ) - bgen.meanLog( avgP, mPi, -1, 1000 );
		// double bElec = one_beta( mE, avgP ) - one_beta( mPi, avgP );
		// //drawTpcLines( dMerged, kBlack, y1, y2);
		// TEllipse * ell = new TEllipse( dElec,bElec, sigTpc * 3, sigTof * 3 );
		// ell->SetFillColorAlpha( kBlack, 0 );
		// ell->Draw();

		rp.savePage();
		rp.saveImage( ("img/dedx_tof_" + ts(iPt) + ".pdf").c_str() );
	}

}