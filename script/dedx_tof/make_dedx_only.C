
#include "Reporter.h"

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

void make_dedx_only(){
	TFile * f = new TFile( "histograms.root", "READ" );

	RooPlotLib rpl;

	Reporter rp( "tmp.pdf", 600, 400 );
	set_plot_style();
	gStyle->SetOptStat( 0 );


	TH2 * h = (TH2*)f->Get( "dedxRaw" );

	h->RebinX( 2 );
	h->RebinY( 2 );

	rpl.style( h ).set( "title", " ; p_{T} * charge; ln(dE/dx)" )
		.set( "yr", 0.6, 2.2 )
		.set( "xr", -3, 3 )
		.set( "yts", 0.08 )
		.set( "xts", 0.08 )
		.set( "xto", 0.7 )
		.set( "yto", 0.6 )
		.set( "draw", "col" ).draw();

	gPad->SetBottomMargin( 0.13 );

	gPad->SetLogz(1);
	gPad->SetLeftMargin( 0.13 );
	gPad->SetRightMargin( 0.05 );

	rp.saveImage( "img/dedx_raw.pdf" );
	rp.saveImage( "img/dedx_raw.png" );
}