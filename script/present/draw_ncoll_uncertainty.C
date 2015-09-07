
#include "draw_single_spectra.C"

int n_ncoll_unc_bins = 9;
double ncoll_unc_bins[] = { 
0.0,
0.1,
0.2, 
0.3,
0.4,
0.5, 
5.2, 
5.4,
5.6,
5.8,
6.0,
6.2,
6.4,
6.6,
6.8 };

void draw_ncoll_uncertainty( int iEn, double cen, double u_cen, double per, double u_per, bool front = false ){
	
	string name = "u_ncoll_" + dts(u_cen) + "_" + dts( u_per );
	TH1 * h = new TH1D( name.c_str(), "", n_ncoll_unc_bins, ncoll_unc_bins );


	int iB = n_ncoll_unc_bins - iEn;
	if ( front )
		iB = iEn + 1;
	h->SetBinContent( iB, 1.0 );
	
	// add the error in
	double sigma = u_cen / cen + u_per / per;
	h->SetBinError( iB, sigma );

	h->SetFillColorAlpha( colors[ iEn ], 0.75 );
	h->Draw( "same e2" );

}