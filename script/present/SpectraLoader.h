#include <fstream>

#include "Logger.h"

#include "TGraphErrors.h"

class SpectraLoader{

public:

	static constexpr auto tag = "SpectraLoader";
	vector<double> pT;
	vector<double> value;
	vector<double> stat;
	vector<double> sys;

	// computed
	vector<double> width;

	SpectraLoader( string fname ){

		ifstream inf( fname.c_str() );

		string tmp;
		double a, b, c, d;

		getline( inf, tmp );

		while( inf >> a >> b >> c >> d ){

			pT.push_back( a );
			value.push_back( b );
			stat.push_back( c );
			sys.push_back( d );

			// INFO( tag,  "[" << a << "] = " << b << " +/- " << c << " +/- " << d );

		}	

		if ( pT[ 0 ] > pT[ pT.size() - 1 ] ){
			reverse( pT.begin(), pT.end() );
			reverse( value.begin(), value.end() );
			reverse( stat.begin(), stat.end() );
			reverse( sys.begin(), sys.end() );
		}


		for ( int i = 0; i < pT.size(); i++ ){

			if ( i < pT.size() - 1  )
				width.push_back( (pT[ i+1 ] - pT[ i ]) / 2.0 ) ;
			else
				width.push_back( (pT[ i ] - pT[ i - 1 ]) / 2.0 );

			// INFO( tag, "width[" << i << "] = " << width[ i ] );
		}


		inf.close();
	}

	void trim( int N = 1 ){

		for ( int i = 0; i < N; i++ ){
			pT.erase( pT.begin() );
			width.erase( width.begin() );
			value.erase( value.begin() );
			stat.erase( stat.begin() );
			sys.erase( sys.begin() );
		}
	}

	void trunc( int N = 1 ){

		for ( int i = 0; i < N; i++ ){
			pT.erase( pT.end() );
			width.erase( width.end() );
			value.erase( value.end() );
			stat.erase( stat.end() );
			sys.erase( sys.end() );
		}
	}

	TGraphErrors * statGraph(){
		TGraphErrors * graph = new TGraphErrors( pT.size(), pT.data(), value.data(), width.data(), stat.data() );
		return graph;
	}

	vector<double> getBins() {
		vector<double> bins;
		// make the bin edges
		for ( int i = 0; i < pT.size(); i++ ){
			if ( 0 == i){
				bins.push_back( pT[ 0 ] - width[ 0 ] );
			}
			bins.push_back( pT[ i ] + width[ i ] );
		}

		for ( int i = 0; i < bins.size(); i++ ){
			//INFO( tag, "bin edge [" << i << "] = " << bins[ i ] );
		}
		return bins;
	}

	TH1D * statHisto( string name ){
		vector<double> bins = getBins();
		TH1D * h = new TH1D( name.c_str(), "", bins.size() - 1, bins.data() );
		for ( int i = 0; i < bins.size() + 1; i++ ){
			h->SetBinContent( i + 1, value[ i ] );
			h->SetBinError( i + 1, stat[ i ] );
		}
		return h;
	}

	TH1D * combinedErrorHisto( string name ){
		vector<double> bins = getBins();
		TH1D * h = new TH1D( name.c_str(), "", bins.size() - 1, bins.data() );
		for ( int i = 0; i < bins.size() + 1; i++ ){
			h->SetBinContent( i + 1, value[ i ] );
			h->SetBinError( i + 1, sqrt( stat[ i ]*stat[ i ] + sys[i]*sys[i] ) );
		}
		return h;
	}

	

	TH1D * sysHisto( string name ){
		vector<double> bins = getBins();
		TH1D * h = new TH1D( name.c_str(), "", bins.size() - 1, bins.data() );
		for ( int i = 0; i < bins.size() + 1; i++ ){
			h->SetBinContent( i + 1, value[ i ] );
			h->SetBinError( i + 1, sys[ i ] );
		}
		return h;
	}

	TH1D * relSysHisto( string name ){
		vector<double> bins = getBins();
		TH1D * h = new TH1D( name.c_str(), "", bins.size() - 1, bins.data() );
		for ( int i = 0; i < bins.size() + 1; i++ ){
			h->SetBinContent( i + 1, sys[i] / value[ i ] );
			h->SetBinError( i + 1, 0 );
		}
		return h;
	}

	TH1D * relStatHisto( string name ){
		vector<double> bins = getBins();
		TH1D * h = new TH1D( name.c_str(), "", bins.size() - 1, bins.data() );
		for ( int i = 0; i < bins.size() + 1; i++ ){
			h->SetBinContent( i + 1, stat[i] / value[ i ] );
			h->SetBinError( i + 1, 0 );
		}
		return h;
	}

};