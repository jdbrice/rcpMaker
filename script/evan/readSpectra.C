


string path = "/Users/danielbrandenburg/Downloads/all.spectra/spectracorrected.c";
vector<double> readSpectra( int charge, int plc, int cl, int ch, string en ){
	
	string name = path + ts(charge) + ".p" + ts( plc ) + ".cl" + ts(cl) + ".ch" + ts( ch ) + "." + en + ".txt";

	cout << "Opening : " << name << endl;
	ifstream inf( name.c_str() );

	
	double pt, val, e1, e2;
	char line[500];

	inf.getline( line, 500 );
	
	vector<double> vals;
	bool br = true;
	int i = 0;
	while ( br ){
		inf.getline( line, 500 );
		std::istringstream iss(line);
		

		if ( !(iss >> pt >> val >>e1 >>e2) ) break;
		//cout << "["<<i<<"]" << pt << ", " << val << endl;
		vals.push_back( val );


		if ( inf.eof() )
			br = false;
		//cout << iss << endl;
		i++;
	}


	inf.close();

	return vals;

}

vector<double> readSpectraBins( int charge, int plc, int cl, int ch, string en ){
	
	string name = path + ts(charge) + ".p" + ts( plc ) + ".cl" + ts(cl) + ".ch" + ts( ch ) + "." + en + ".txt";

	cout << "Opening : " << name << endl;
	ifstream inf( name.c_str() );

	
	double pt, val, e1, e2;
	char line[500];

	inf.getline( line, 500 );
	
	vector<double> vals;
	bool br = true;
	while ( br ){
		inf.getline( line, 500 );
		std::istringstream iss(line);
		

		if ( !(iss >> pt >> val >>e1 >>e2) ) break;

		vals.push_back( pt );


		if ( inf.eof() )
			br = false;
		//cout << iss << endl;
	}

	inf.close();

	return vals;

}


vector<double> readSpectraE1( int charge, int plc, int cl, int ch, string en ){
	
	string name = path + ts(charge) + ".p" + ts( plc ) + ".cl" + ts(cl) + ".ch" + ts( ch ) + "." + en + ".txt";

	cout << "Opening : " << name << endl;
	ifstream inf( name.c_str() );

	
	double pt, val, e1, e2;
	char line[500];

	inf.getline( line, 500 );
	
	vector<double> vals;
	bool br = true;
	while ( br ){
		inf.getline( line, 500 );
		std::istringstream iss(line);
		

		if ( !(iss >> pt >> val >>e1 >>e2) ) break;

		vals.push_back( e1 );


		if ( inf.eof() )
			br = false;
		//cout << iss << endl;
	}

	inf.close();

	return vals;

}

vector<double> readSpectraE2( int charge, int plc, int cl, int ch, string en ){
	
	string name = path + ts(charge) + ".p" + ts( plc ) + ".cl" + ts(cl) + ".ch" + ts( ch ) + "." + en + ".txt";

	cout << "Opening : " << name << endl;
	ifstream inf( name.c_str() );

	
	double pt, val, e1, e2;
	char line[500];

	inf.getline( line, 500 );
	
	vector<double> vals;
	bool br = true;
	while ( br ){
		inf.getline( line, 500 );
		std::istringstream iss(line);
		

		if ( !(iss >> pt >> val >>e1 >>e2) ) break;

		vals.push_back( e2 );


		if ( inf.eof() )
			br = false;
		//cout << iss << endl;
	}

	inf.close();

	return vals;

}

