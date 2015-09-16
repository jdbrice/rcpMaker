
#include "draw_single_spectra.C"

void draw_spectra_vs_energy( string plc="Pi", string charge="p", string cen="0" ){


	RooPlotLib rpl;

	TH1 * master = new TH1F( "frame", "frame", 1000, 0, 10 );
	master->Draw();


	int iColor = 0;
	TLegend * leg = new TLegend( 0.15, 0.15, 0.35, 0.35 );

	for ( string en : energies ){

		TH1 * h = draw_single_spectra( en, plc, charge, cen, colors[ iColor ], "same" );
		leg->AddEntry( h, en.c_str() );

		iColor ++;
	}


	gPad->SetLogy( 1 );
	master->GetYaxis()->SetRangeUser( 1.5e-13, 5e2 );
	rpl.style( master )
		.set( "title", plc_label( plc, charge ) + " ; p_{T} [GeV/c]   ; dN^{2} / ( N_{evt} 2 #pi pT dpT dy )    " )
		.set( "xr", 0.2, 3.8 )
		.set( "yto", 1.35 )
		.set( "yts", 0.065 )
		.set( "xto", 0.9 )
		.set( "xts", 0.08 )
		.set( "markerstyle", 8 );

	leg->Draw("same");

}