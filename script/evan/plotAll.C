
#include "evanRcp.C"


string human( string plc, int charge ){

	return "";
}

void plotAll(  ){

	//TCanvas * cc = new TCanvas( "c1", "c2", 500, 200 );
	gStyle->SetOptStat( 0 );
	
	const int nE = 3;
	string ens[] = { "7.7", "11.5", "19.6", "27.0", "39.0", "62.4" };
	int colors[] = { kRed-3, kBlue-3, kGreen-3, kMagenta-3, kOrange-3, kYellow-3 };
	int markers[] = { kFullCircle, kFullSquare, kFullTriangleUp, kFullTriangleDown, kFullStar, kFullDiamond};
	

	
	TH1* hRcp[ nE ][ 3 ][ 2 ];
	
	for ( int en = 0; en < nE; en++ ){
		for ( int c = 0; c < 2; c++ ){
			for ( int p = 0; p < 3; p ++ ){
				string name = ens[en] + "_" + plcName( p ) + "_" + hCharge( c );
				cout << "Making : " << name << endl;
				TH1 * h = evanRcp( ens[en], c, p );
				
				hRcp[ en ][ p ][ c ] = h;
				cout << "h : " << hRcp[ en ][ p ][ c ] << endl;

			}
		}
	}

	Reporter rp( "everything.pdf", 800,575 );
	rp.newPage( 3, 2, 0, 0 );
	bool drawn = false;
	for ( int p = 0; p < 3; p ++ ){
		for ( int c = 0; c < 2; c++ ){
			drawn = false;
			rp.cd( p+1, c+1 );

			for ( int en = 0; en < nE; en++ ){

				hRcp[ en ][ p ][ c ]->SetLineColor( kBlack );
				hRcp[ en ][ p ][ c ]->SetMarkerColor( colors[ en ] );
				hRcp[ en ][ p ][ c ]->SetMarkerStyle( markers[ en ] );
				if ( false == drawn ){
					hRcp[ en ][ p ][ c ]->Draw( "pe1x0" );
					drawn = true;
					hRcp[ en ][ p ][ c ]->GetYaxis()->SetRangeUser( .1, 10 );
					gPad->SetLogy( 1 );
					gPad->SetGrid(1, 1);
				}
				else 
					hRcp[ en ][ p ][ c ]->Draw( "same e1x0" );

				
			}


			string cpf = "rcp.root";
			string name = "rcp_" + plcName( p ) + "_" + charge( c ) + "_0_Over_5";
			TFile * f = new TFile( cpf.c_str(), "READ" );

			TH1D * crcp = (TH1D*)f->Get( ("rcp/"+name).c_str() );

			crcp->SetMarkerStyle( 22 );
			crcp->SetMarkerColor( kRed );

			for ( int i = 1; i < 2; i++ ){
				crcp->SetBinContent( i, 0 ); 
			}
			crcp->SetBinContent( 28, 0 );
			
			if ( nE > 0 ){
				crcp->Draw( "same" );
			} else {
				crcp->Draw();
				crcp->GetYaxis()->SetRangeUser( 0.11, 10);
				//crcp->SetTitle(  )
				gPad->SetLogy(1);
				gPad->SetGrid( 1, 1 );
			}

			

			/*{
			string cpf = "/Users/danielbrandenburg/bnl/local/work/rcpMaker/products/11GeV/presentation/data.root";
			string name = "rcp_" + plcName( p ) + "_" + charge( c ) + "_0_Over_6";
			TFile * f = new TFile( cpf.c_str(), "READ" );

			TH1D * crcp = (TH1D*)f->Get( ("rcp/"+name).c_str() );
			crcp->SetMarkerStyle( 22 );
			crcp->SetMarkerColor( kRed );
			//crcp->Scale( 1.0 );
			crcp->Draw( "same" );
			}*/

		}

		rp.savePage();

	}

}