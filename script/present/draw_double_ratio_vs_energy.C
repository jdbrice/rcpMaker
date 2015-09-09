#include "draw_double_ratio.C"
#include "draw_single_spectra.C"
#include "common.C"

void draw_double_ratio_vs_energy( 	string charge = "p",
									string plc1 = "P", string plc2 = "Pi",  
									string iCen1 = "0", string iCen2 = "6" ){

	gStyle->SetOptStat( 0 );
	RooPlotLib rpl;
	TH1 * master;

	Reporter rp( "img/rp_double_ratio.pdf", 400, 600 );

	gPad->SetTopMargin( 0.01);
	TLegend * leg = new TLegend( 0.6, 0.1, 0.9, 0.5 );
	leg-> SetNColumns(1);
	leg->SetTextFont( 52 );
	leg->SetTextSize( 0.05 );

	gPad->SetLeftMargin( 0.15 );

	int iColor = 0;
	float scaler = 1.0;
	for ( string en : energies ){

		TH1 * h;
		if ( 0 == iColor ){
			master = draw_double_ratio( en, plc1, plc2, charge, iCen1, iCen2, colors[ iColor ], "", scaler, &rp );
			h = master;
		}
		else
			h = draw_double_ratio( en, plc1, plc2, charge, iCen1, iCen2, colors[ iColor ], "same", scaler, &rp );

		leg->AddEntry( h, (en + " x " + ts( (int)scaler )).c_str() );


		scaler *= 2;
		iColor++;
	}

	string yt = centrality_labels[ stoi( iCen1 ) ] + " central / " + centrality_labels[ stoi( iCen2 ) ] + " central " + plc_label( plc1, charge ) + "/" + plc_label( plc2, charge ) + " Ratio";

	rpl.style( master ).set( "yr", 0.3, 140 )
	.set( "xr", 0, 5 )
	.set( "title", " ; pT [GeV/c]; " + yt )
	.set( "yto", 1.5 );

	leg->Draw();
	gPad->SetGrid(1, 0);


	rp.saveImage( "img/double_ratio_" + iCen1+"_over_" + iCen2 +"_"+ plc1+"_over_"+plc2+"_"+charge+".pdf" );


}