#ifndef DRAW_N_COLL_UNC
#define DRAW_N_COLL_UNC



#include "draw_single_spectra.C"

int n_ncoll_unc_bins = 16;
double ncoll_unc_bins[] = { 
0.0,
0.1,
0.2, 
0.3,
0.4,
0.5,
0.6,
0.7,
0.8,
0.9,
5.1, 
5.2, 
5.3,
5.4,
5.5,
5.6,
5.7
 };

void draw_ncoll_uncertainty( int iEn, double cen, double u_cen, double per, double u_per, bool front = false, int color = -1 ){
	
	string name = "u_ncoll_" + dts(u_cen) + "_" + dts( u_per );
	TH1 * h = new TH1D( name.c_str(), "", n_ncoll_unc_bins, ncoll_unc_bins );


	int iB = n_ncoll_unc_bins - iEn;
	
	if ( front )
		iB = iEn + 1;

	h->SetBinContent( iB, 1.0 );
	
	// add the error in
	double sigma = u_cen / cen + u_per / per;
	h->SetBinError( iB, sigma  );

	if ( color > 0 )
		h->SetFillColorAlpha( color, 0.75 );
	else	
		h->SetFillColorAlpha( colors[ iEn ], 0.75 );
	h->Draw( "same e2" );

}

#endif