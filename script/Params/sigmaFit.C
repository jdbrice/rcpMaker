

double zbSigmaFunction( double *x, double *p ){

	using namespace TMath;

	double uT = p[ 0 ]; 	// timing offsets
	double uP = p[ 1 ]; 	// momentum
	double ueP = p[ 2 ]; 	// momentum
	double m = p[ 3 ]; 		// mass of species
	double mr = 0.493667; 	// mass of centering species

	double mmtm = x[ 0 ];
	// characteristic determined by B field and Tof radius
	double x0 = 0.1755 / mmtm;

	double a = (1.0 - x0 / ( ASin( x0 ) * Sqrt( 1 - x0*x0 ) ) ) * uP;
	double b = m*m / ( mmtm * Sqrt( mmtm * mmtm + m * m ) ) * ueP;
	double c = mr*mr / ( mmtm * Sqrt( mmtm * mmtm + mr * mr ) ) * ueP;

	return Sqrt( uT*uT + a*a + b*b + c*c ); 

}

vector<double> sigmaFit( string plc = "K" , int iCen = 0, string inFile = "../../products/param/15GeV/fit.root" ){
	
	TCanvas * can = new TCanvas( ("zb_sigma_" + plc + "_" + ts(iCen) ).c_str(), "sigma" );

	gStyle->SetOptStat( 0 );

	double piMass = 0.1395702;
	double kaonMass = 0.493667;
	double protonMass = 0.9382721;

	TFile * f = new TFile( inFile.c_str(), "READ" );

	if ( !f ){
		cout << "File DNE" << endl;
		return;
	}


	string hName = plc + "_zbSigma/sigma_" + plc + "_" + ts( iCen )+  "_p_0";

	TH1D * h1 = (TH1D*)f->Get( hName.c_str() );
	if ( !h1 ){
		cout << "Histo DNE" << endl;
		return;
	}

	TH1D * h = h1->Clone( "tmp" );

	h->Reset();

	double binMaxPt = 1.5;

	if ( "P" == plc )
		binMaxPt = 2.0;

	for ( int i = 1; i <= h1->GetNbinsX(); i ++ ){

		if ( h1->GetBinCenter( i ) < binMaxPt ){

			h->SetBinContent( i, h1->GetBinContent( i ) );
			h->SetBinError( i, h1->GetBinError( i ) );	
		}
		
	}

	
	

	double min = 0.3;
	double max = 1.2;

	if ( 6 == iCen  ){
		max = 1.0;
	}
	
	//<sigma t="0.0111527" p="0.102938" ep="0.00755773"/>
	
	double mass = kaonMass;
	if ( "Pi" == plc )
		mass = piMass;
	if ( "P" == plc  ){
		min = 0.5;
		max = 1.8;
		mass = protonMass;
	}

	TF1 * mfn = new TF1( ("zb_sfn_"+plc).c_str(), zbSigmaFunction, min, max, 4 );
	mfn->SetParameters( 0.0111527, 0.1, 0.005, mass);
	mfn->FixParameter( 3, mass );
	

	h->SetTitle( (plc + " : #sigma z_{#beta}").c_str() );
	h->SetMarkerStyle( 20  );
	h->Draw( "pe" );
	h->Fit( mfn, "R" );

	mfn->SetRange( 0.2, 4.0 );
	mfn->Draw( "same" );

	double uT = mfn->GetParameter( 0 );
	double uP = mfn->GetParameter( 1 );
	double uEP = mfn->GetParameter( 2 );

	vector<double> ps;
	ps.push_back( uT );
	ps.push_back( uP );
	ps.push_back( uEP );

	can->Print( ("zb_sigma_"+plc+"_"+ts(iCen)+".pdf").c_str() );

	return ps;



}