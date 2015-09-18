

void set_plot_style()
{
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

}


void vs_dedx( string fn = "/Users/danielbrandenburg/bnl/local/work/rcpMaker/products/nominal/TofEff_P.root" ){

	set_plot_style();

	RooPlotLib rpl;

	TFile * f = new TFile( fn.c_str(), "READ" );

	Reporter rp( "rp.pdf", 800, 800 );

	vector<string> charge={"p", "n"};
	vector<string> cen={ "0", "1", "2", "3", "4", "5", "6" };

	for ( string c : charge ){
		for ( string ic : cen ){
			TH2D *hTotal = (TH2D*)f->Get( ("inclusive_vs_dedx/pt_" + ic + "_" + c).c_str() );
			TH2D *hPass = (TH2D*)f->Get( ("inclusiveTof_vs_dedx/pt_" + ic + "_" + c).c_str() );

			hPass->Divide(hTotal);

			rpl.style( hPass ).set( "xr", 0, 2 ).set( "optstat", 0 );


			rp.newPage();
				hPass->Draw( "colz" );
			rp.savePage();
		}
	}

}