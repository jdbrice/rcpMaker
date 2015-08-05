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

TH1* evanRcp( string en, int c, int p ){


	//TCanvas * cc = new TCanvas( "c1", "c2", 250, 250 );

	gStyle->SetOptStat( 0 );
	
	vector<double> c6 = readSpectra( c, p, 0, 1, en );
	vector<double> c6e = readSpectraE1( c, p, 0, 1, en );

	TH1D * hc6 = makeHisto( (en+"_"+plcName(p)+"_"+hCharge(c)+"_"+"cen6").c_str(), c6, c6e );

	vector<double> c0 = readSpectra( c, p, 8, 8, en );
	vector<double> c0e = readSpectraE1( c, p, 8, 8, en );
	TH1D * hc0 = makeHisto( (en+"_"+plcName(p)+"_"+hCharge(c)+"_"+"cen0").c_str(), c0, c0e );

	float scaleC = 19.0;
	float scaleP = 784.45;

	if ( "7.7" == en )
		scaleP = 773.7;
	else if ( "19.6" == en ){
		scaleC = 18.91901;
		scaleP = 799.84113;
	}else if ( "27.0" == en ){
		scaleC = 19.97044;
		scaleP = 841.47851;
	} else if ( "39.0" == en ){
		scaleC = 20.0;
		scaleP = 853.0;
	} else if ( "62.4" == en ){
		scaleC = 20.0;
		scaleP = 904.0;
	}

	hc0->Scale( scaleC );
	hc6->Scale( scaleP );

	hc0->Divide( hc6 );
	hc0->SetTitle( (plcName( p ) + " : " + hCharge( c )).c_str()  );

	return hc0;

}
