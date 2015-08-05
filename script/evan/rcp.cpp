#include "readSpectra.C"


double ptBins[] = { 
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
6.0 };

TH1D *  makeHisto( string name, vector<double> &vals, vector<double> &errors ){

	TH1D * h = new TH1D( name.c_str(), name.c_str(), 24, ptBins );

	for ( int i = 0; i < vals.size(); i++ ){
		h->SetBinContent( vals.size() - i, vals[ i ] );
		h->SetBinError( vals.size() - i, errors[ i ] );
	}
	h->Draw();
	gPad->SetLogy(1);

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

void rcp( string en, int c, int p, string cpf = "/Users/danielbrandenburg/bnl/local/work/rcpMaker/products/presentation/11GeV/data.root" ){


	TCanvas * cc = new TCanvas( "c1", "c2", 250, 250 );

	gStyle->SetOptStat( 0 );
	
	vector<double> c6 = readSpectra( c, p, 0, 1, en );
	vector<double> c6e = readSpectraE1( c, p, 0, 1, en );

	TH1D * hc6 = makeHisto( "cen6", c6, c6e );

	vector<double> c0 = readSpectra( c, p, 8, 8, en );
	vector<double> c0e = readSpectraE1( c, p, 8, 8, en );
	TH1D * hc0 = makeHisto( "cen0", c0, c0e );

	hc0->Scale( 19.0 );
	hc6->Scale( 784.0 );

	hc0->Divide( hc6 );

	hc0->SetTitle( (plcName( p ) + " : " + hCharge( c )).c_str()  );
	hc0->GetYaxis()->SetRangeUser( 0.1, 5 );
	hc0->GetXaxis()->SetRangeUser( 0.0, 4 );
	hc0->SetMarkerStyle( 20 );
	hc0->SetMarkerColor( kBlue );
	hc0->Draw();
	gPad->SetLogy(1);	

	string name = "rcp_" + plcName( p ) + "_" + charge( c ) + "_0_Over_6";
	TFile * f = new TFile( cpf.c_str(), "READ" );

	TH1D * crcp = (TH1D*)f->Get( ("rcp/"+name).c_str() );
	crcp->SetMarkerStyle( 22 );
	crcp->SetMarkerColor( kRed );
	crcp->Scale( 1.08 );
	crcp->Draw( "same" );

	cc->SetGrid(1, 1);

	cc->Print( (name + ".pdf").c_str());



}
