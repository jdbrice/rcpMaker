
double zdSigmaFunction( double *x, double *p ){

	using namespace TMath;

	double x0 = x[ 0 ];

	return p[0] - ( p[1] + p[2] * x0 ) * Exp( -p[3] * x0 );

}

vector<double> dedxSigmaFit( string plc = "K" , int iCen = 0, string inFile = "../../products/param/15GeV/fit.root" ){
	
	TCanvas * can = new TCanvas( ("mu_" + plc+ "_" + ts(iCen) ).c_str(), "mu" );

	double piMass = 0.1395702;
	double kaonMass = 0.493667;
	double protonMass = 0.9382721;

	TFile * f = new TFile( inFile.c_str(), "READ" );

	if ( !f ){
		cout << "File DNE" << endl;
		return;
	}


	string hName = plc + "_zdSigma/sigma_" + plc + "_" + ts( iCen )+  "_p_0";

	TH1D * h = (TH1D*)f->Get( hName.c_str() );
	if ( !h ){
		cout << "Histo DNE" << endl;
		return;
	}

	h->Draw( "pe" );

	double min = 0.2;
	double max = 1.5;
	
	
	double mass = kaonMass;
	if ( "Pi" == plc ){
		min = 0.35;
		mass = piMass;
	}
	if ( "P" == plc ){
		min = 0.35;
		max = 1.6;
		mass = protonMass;
	}

	TF1 * mfn = new TF1( "mfn", zdSigmaFunction, min, max, 4 );
	
	mfn->SetParameters( 0.05, 0.005, 0.5, 0.5);
	mfn->SetParLimits( 1, 0, 1 );
	if ( "Pi" != plc )
		mfn->SetParLimits( 2, 0, 10 );
	mfn->SetParLimits( 3, 0, 10000 );
	
	h->SetTitle( (plc + " : #sigma Log_{10}( z_{d} ) ").c_str() );
	h->GetYaxis()->SetRangeUser( 0, 0.06 );
	h->SetMarkerStyle( 20  );
	h->Draw( "pe" );
	h->Fit( mfn, "R" );
	h->Fit( mfn, "R" );
	mfn->SetRange( 0.2, 4.0 );
	mfn->Draw( "same");

	double p0 = mfn->GetParameter( 0 );
	double p1 = mfn->GetParameter( 1 );
	double p2 = mfn->GetParameter( 2 );
	double p3 = mfn->GetParameter( 3 );

	can->Print( ("zd_sigma_"+plc+"_"+ts(iCen)+".pdf").c_str() );

	vector<double> ps;
	ps.push_back( p0 );
	ps.push_back( p1 );
	ps.push_back( p2 );
	ps.push_back( p3 );

	return ps;

}