
#include "readSpectra.C"


double ptBins[] = { 
0.0,
0.5,
0.6, 
0.7, 
0.8, 
0.9, 
1.0, 
1.1, 
1.2, 
1.3, 
1.4, 
1.5, 
1.6, 
1.7, 
1.8, 
1.9, 
2.0	, 
2.2	, 
2.4	, 
2.6	, 
2.8	,
3.0 , 
3.5, 
4.5, 
5.0, 
6.0, 
6.8 };

TH1D *  makeHisto( string name, vector<double> &vals, vector<double> &errors ){

	TH1D * h = new TH1D( name.c_str(), name.c_str(), 26, ptBins );

	for ( int i = 0; i < vals.size(); i++ ){

		int bin = i+2; // skip first bin
		float val = vals[ vals.size() - (i + 1) ];
		float error = errors[ errors.size() - (i + 1) ];

		h->SetBinContent( bin, val );
		h->SetBinError( bin, error );
	}
	//h->Draw();
	//gPad->SetLogy(1);

	return h;

}


string plcName( int plc ){

	if ( 0 == plc )
		return "Pi";
	if ( 1 == plc )
		return "K";
	if ( 2 == plc )
		return "P";
	return "";
}

string charge( int c  ){
	if ( 0 == c  )
		return "n";
	if ( 1 == c )
		return "p";
	return "";
}

string hCharge( int c ) {
	if ( 0 == c  )
		return "minus";
	if ( 1 == c )
		return "plus";
	return "";	
}

TH1* evanYield( string en, int c, int p, int iCen ){

	int cl = 0;
	int ch = 0;

	switch (iCen) {
		case 0 :
			cl = 8;
			ch = 8;
		break;
		case 6 :
			cl = 0;
			ch = 1;
		break;
	}

	gStyle->SetOptStat( 0 );
	
	
	vector<double> c6 = readSpectra( c, p, cl, ch, en );
	vector<double> c6e = readSpectraE1( c, p, cl, ch, en );

	TH1D * hc6 = makeHisto( (en+"_"+plcName(p)+"_"+hCharge(c)+"_"+"cen6").c_str(), c6, c6e );

	return hc6;

}


void spectraCompare( int c, int p ){

	TH1D * evan19 = evanYield( "19.6", c, p, 0 );
	TH1D * evan11 = evanYield( "11.5", c, p, 0 );

	string mc = "n";
	if ( 1 == c )
		mc = "p";

	string plc = "Pi";
	if ( 1 == p )
		plc = "K";
	else if ( 2 == p )
		plc = "P";

	string dan = "../../products/15/spectra/dN_dptdy.root";
	TFile * f = new TFile( dan.c_str(), "READ" );

	TH1D * hDan = (TH1D*)f->Get( (plc + "_" + mc + "_" + ts(0) ).c_str() );


	/*for ( int i = 0; i < hDan->GetNbinsX(); i++ ){
		double bc = hDan->GetBinContent( i );
		hDan->SetBinContent( i, bc * ( 1.0 / (hDan->GetBinCenter( i ) ) *   )  );
	}*/

	hDan->Draw();

	evan19->SetLineColor( kRed );
	evan19->Draw("same");

	evan11->SetLineColor( kGreen );
	evan11->Draw("same");

	gPad->SetLogy(1);

}