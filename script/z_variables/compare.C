

void compare( string fn1, string fn2, string name ){
	

	TFile *f1 = new TFile( fn1.c_str(), "READ" );
	TFile *f2 = new TFile( fn2.c_str(), "READ" );


	TH1 * h1 = (TH1*)f1->Get( name.c_str() );
	TH1 * h2 = (TH1*)f2->Get( name.c_str() );

	h1->Draw();
	h1->SetLineColor( kRed );
	h1->SetMarkerColor( kRed );

	h2->Draw("same");

	gPad->SetLogy(1);

}