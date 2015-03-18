

void rcpFromSpectra( string fname = "../products/inclusive/inclusiveSpectra.root", string bName = "pt_" ){

	TCanvas * c = new TCanvas( "c", "R_{CP}", 800, 600 );
	TFile * f = new TFile( fname.c_str(), "READ" );

	string cName = bName + "1";
	string pName = bName + "0";
	TH1D * central = (TH1D*)f->Get( cName.c_str() );
	TH1D * per = (TH1D*)f->Get( pName.c_str() );

	gStyle->SetOptStat(0);

	c->Divide( 2, 1 );
	c->cd(1 );
	central->Scale( 19 );
	per->Scale( 719 );

	per->SetTitle( "0%-5% (Blue), 60%-80% (Red)" );
	per->Sumw2();
	central->Sumw2();
	per->Draw( "pe" );
	central->Draw( "same pe");
	per->SetLineColor( kRed );
	

	gPad->SetLogy(1);

	c->cd(2 );
	TH1D* rcp = ((TH1D*)central->Clone( "ratio" ))->Rebin(2);
	TH1D* rbPer = per->Rebin( 2 );
	rcp->SetTitle( "0%-5% / 60%-80% " );
	rcp->Divide( per );

	rcp->GetYaxis()->SetRangeUser( 0, 3 );
	rcp->Draw();

	c->Print( (bName+".pdf").c_str() );

}