

#include "meanFit.C"
#include "sigmaFit.C"
#include "dedxSigmaFit.C"


void report( vector<vector<double> > &data, ofstream &ofs, int nPar = 3 ){
	
	for ( int j = 0; j < nPar; j++ ){
		ofs << "\t\t\t<p" + ts(j ) + "> ";
		for ( int i = 0; i < data.size(); i++ ){

			ofs << data[ i ][ j ];
			if ( i != data.size() - 1  )
				ofs << ", ";
		}
		ofs << "</p" << ts( j ) << ">" << endl;
	}

}

void makeParams( string inFile  = "../../products/param/15GeV/fit.root" ){

	int cBins[] = { 0, 1, 2, 3, 4, 5, 6 };

	vector<vector< double> > sPi;
	vector<vector< double>> sK;
	vector<vector< double>> sP;

	vector<vector< double>> mPi;
	vector<vector< double>> mK;
	vector<vector< double>> mP;

	vector<vector< double>> dsPi;
	vector<vector< double>> dsK;
	vector<vector< double>> dsP;

	for ( int i = 0; i < 7; i++ ){
		int iCen = cBins[ i ];

		sPi.push_back( sigmaFit( "Pi", iCen, inFile ) );
		sK.push_back( sigmaFit( "K", iCen, inFile ) );
		sP.push_back( sigmaFit( "P", iCen, inFile ) );

		mPi.push_back( meanFit( "Pi", iCen, inFile ) );
		mK.push_back( meanFit( "K", iCen, inFile ) );
		mP.push_back( meanFit( "P", iCen, inFile ) );

		dsPi.push_back( dedxSigmaFit( "Pi", iCen, inFile ) );	
		dsK.push_back( dedxSigmaFit( "K", iCen, inFile ) );	
		dsP.push_back( dedxSigmaFit( "P", iCen, inFile ) );
	}

	
	ofstream ofs( "pidParams.xml" );


	// TOF
		// Pi
	ofs << "<TofPidParams>" << endl;
	ofs << "\t<Pi>" << endl;
	ofs << "\t\t<sigma>" << endl;
		report( sPi, ofs );
	ofs << "\t\t</sigma>" << endl;
	ofs << "\t\t<mean>" << endl;
		report( mPi, ofs );
	ofs << "\t\t</mean>" << endl;
	ofs << "\t</Pi>" << endl;


	// K
	ofs << "\t<K>" << endl;
	ofs << "\t\t<sigma>" << endl;
		report( sK, ofs );
	ofs << "\t\t</sigma>" << endl;
	ofs << "\t\t<mean>" << endl;
		report( mK, ofs );
	ofs << "\t\t</mean>" << endl;
	ofs << "\t</K>" << endl;

	// P
	ofs << "\t<P>" << endl;
	ofs << "\t\t<sigma>" << endl;
		report( sP, ofs );
	ofs << "\t\t</sigma>" << endl;
	ofs << "\t\t<mean>" << endl;
		report( mP, ofs );
	ofs << "\t\t</mean>" << endl;
	ofs << "\t</P>" << endl;
	ofs << "</TofPidParams>" << endl;

	// dEdx
		// Pi
	ofs << "<DedxPidParams>" << endl;
	ofs << "\t<Pi>" << endl;
	ofs << "\t\t<sigma>" << endl;
		report( dsPi, ofs, 4 );
	ofs << "\t\t</sigma>" << endl;
	ofs << "\t</Pi>" << endl;
	// K
	ofs << "\t<K>" << endl;
	ofs << "\t\t<sigma>" << endl;
		report( dsK, ofs, 4 );
	ofs << "\t\t</sigma>" << endl;
	ofs << "\t</K>" << endl;
	// P
	ofs << "\t<P>" << endl;
	ofs << "\t\t<sigma>" << endl;
		report( dsPi, ofs, 4 );
	ofs << "\t\t</sigma>" << endl;
	ofs << "\t</P>" << endl;
	ofs << "</DedxPidParams>" << endl;
	

	ofs.close();


}