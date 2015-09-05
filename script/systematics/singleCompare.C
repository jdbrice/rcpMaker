

void singleCompare( string fn1, string fn2, string plc = "Pi" ){
	TFile * f1 = new TFile( fn1.c_str(), "READ" );
	TFile * f2 = new TFile( fn2.c_str(), "READ" );

	string name = plc + "_yield/yield_" + plc + "_0_p";
	TH1D * h1 = (TH1D*)f1->Get( name.c_str() );
	TH1D * h2 = (TH1D*)f2->Get( name.c_str() );
	h2->SetLineColor( kRed );

	h1->Draw();
	h2->Draw("same");

	TH1D * dif = (TH1D*) h1->Clone( "dif" );

	dif->Add( h2, -1 );

	dif->Draw();

	
}