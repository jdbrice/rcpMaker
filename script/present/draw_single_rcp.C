#ifndef DRAW_SINGLE_RCP
#define DRAW_SINGLE_RCP


#include "draw_single_spectra.C"
#include "RooPlotLib.h"

TH1* draw_single_rcp(string energy, string plc, string charge, string iCen = "0", string iPer = "6",
							double cen_nColl = 800.0, double per_nColl = 20.0, 
							int color = kRed, string draw_opt = "", int marker = 8){

	RooPlotLib rpl;

	gStyle->SetOptStat( 0 );

	string cen_fn = file_name( energy, plc, charge, iCen );
	string per_fn = file_name( energy, plc, charge, iPer );

	SpectraLoader cen_sl( cen_fn );
	SpectraLoader per_sl( per_fn );

	TH1* cen_sys = normalize_binning( cen_sl.sysHisto( cen_fn + "_cen_sys" ) );
	TH1* per_sys = normalize_binning( per_sl.sysHisto( per_fn + "_per_sys" ) );
	
	TH1* cen_stat = normalize_binning( cen_sl.statHisto( cen_fn + "_cen_stat" ) );
	TH1* per_stat = normalize_binning( per_sl.statHisto( per_fn + "_per_stat" ) );

	TH1* cen_stat_rcp = (TH1*)cen_stat->Clone( (cen_fn + "_cen_stat_rcp").c_str() );
	TH1* cen_sys_rcp = (TH1*)cen_sys->Clone( (cen_fn + "_cen_sys_rcp").c_str() );

	// do the division
	cen_stat_rcp->Divide( per_stat );
	cen_stat_rcp->Scale( per_nColl / cen_nColl );

	cen_sys_rcp->Divide( per_sys );
	cen_sys_rcp->Scale( per_nColl / cen_nColl );



	rpl.style( cen_sys_rcp )
		.set( "title", " ;p_{T} [GeV/c]; R_{CP} ( " + centrality_labels[ stoi( iCen ) ] + " ) / ( " + centrality_labels[ stoi( iPer ) ] + " )    " )
		.set( "yts", 0.05 )
		.set( "yls", 0.06 )
		.set( "xts", 0.05 )
		.set( "xls", 0.05 )
		.set( "mst", marker )
		.set( "color", color, 0.13 );

	if ( "14.5" == energy )
		rpl.set( "ms", 2 );

	rpl.style( cen_stat_rcp )
		.set( "yts", 0.05 )
		.set( "yls", 0.06 )
		.set( "mst", marker )
		.set( "color", color );

	cen_sys_rcp->Draw( ( draw_opt + " e2").c_str() );
	cen_stat_rcp->Draw( "same e1x0" );

	cen_sys_rcp->GetYaxis()->SetRangeUser( 0.15, 6 );
	gPad->SetLogy(1);
	gPad->SetGrid( 1, 1 );


	return cen_stat_rcp;

}

#endif