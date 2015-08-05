#include "readSpectra.C"

double ptBins[] = {  0.0, 0.5, 0.6,  0.7,  0.8,  0.9,  1.0,  1.1,  1.2,  1.3,  1.4,  1.5,  1.6,  1.7,  1.8,  1.9,  2.0	,  2.2	,  2.4	,  2.6	,  2.8	, 3.0 ,  3.5,  4.5,  5.0,  6.0,  6.8 };

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

TH1D *  makeHisto( string name, vector<double> &vals, vector<double> &errors ){

	TH1D * h = new TH1D( name.c_str(), name.c_str(), 26, ptBins );

	for ( int i = 0; i < vals.size(); i++ ){

		int bin = i+2; // skip first bin
		float val = vals[ vals.size() - (i + 1) ];
		float error = errors[ errors.size() - (i + 1) ];

		h->SetBinContent( bin, val );
		h->SetBinError( bin, error );
	}
	return h;

}



void singleSpectra( int c, int p, int cl, int ch, string en ) {


	vector<double> vals = readSpectra( c, p, cl, ch, en );
	vector<double> errors = readSpectraE1( c, p, cl, ch, en );

	TH1D * h = makeHisto( plcName( p ) + "_" + charge(c) + "_" + ts(cl) + "_" + ts(ch), vals, errors );

}

void evanSpectra( string en){

	TFile * fout = new TFile( ("spectra_" + en + ".root").c_str(), "RECREATE" );

	vector<string> ens = { "7.7", "11.5", "19.6", "27.0", "39.0", "62.4" };
	if ( find( ens.begin(), ens.end(), en ) == ens.end() ){
		cout << "Invalid energy" << endl;
		return;
	}


	for ( int c = 0; c < 2; c++  ){
		for ( int p = 0; p < 3; p++ ){
			singleSpectra( c, p, 0, 1, en ); // 60-80%
			singleSpectra( c, p, 2, 3, en ); // 40-60%
			singleSpectra( c, p, 4, 5, en ); // 20-40%
			singleSpectra( c, p, 6, 6, en ); // 10-20%
			singleSpectra( c, p, 7, 7, en ); // 5-10%
			singleSpectra( c, p, 8, 8, en ); // 0-5%
		}
	}
	


	fout->Write();
	fout->Close();

}
