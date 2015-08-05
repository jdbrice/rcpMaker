

void makeLegend(){

	Reporter rp( "legend.pdf", 200, 200 );

	int nE = 6;
	string ens[] = { "7.7", "11.5", "19.6", "27.0", "39.0", "62.4" };
	int colors[] = { kRed-3, kBlue-3, kGreen-3, kMagenta-3, kOrange-3, kYellow-3 };
	int markers[] = { kFullCircle, kFullSquare, kFullTriangleUp, kFullTriangleDown, kFullStar, kFullDiamond};

	TH1D * h2[ 10 ];

	TLegend * leg = new TLegend( 0.1, 0.1, 0.9, 0.9 );
	for ( int i = 0; i < nE; i ++ ){
		string name = "bla_"+ts(i);
		h2[ i ] = new TH1D( name.c_str(), "bla", 10, 0, 10 );
		h2[ i ]->SetMarkerColor( colors[ i ] );
		h2[ i ]->SetMarkerStyle( markers[ i ] );

		leg->AddEntry( h2[ i ], ens[ i ].c_str() );

	}

	TH1D * h = new TH1D( "blah", "blah", 10, 0, 10 );
	h->SetMarkerColor( kRed );
	h->SetMarkerStyle( kFullTriangleUp );

	leg->AddEntry( h, "14.5 GeV ( Daniel )" );

	leg->Draw();

	rp.savePage();
}