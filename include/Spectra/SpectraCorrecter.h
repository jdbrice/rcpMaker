#ifndef SPECTRA_CORRECTER_H
#define SPECTRA_CORRECTER_H

// STL
#include <memory>
#include <map>
using namespace std;

// LOCAL
#include "Params/EffParams.h"
#include "Spectra/PidHistoMaker.h"


class SpectraCorrecter
{
protected:
	string filename;
	shared_ptr<XmlConfig> cfg;
	string nodePath;
	map<string, unique_ptr<EffParams> > params;

public:
	SpectraCorrecter( string _filename ){
		filename = _filename;
		cfg = shared_ptr<XmlConfig>( new XmlConfig( filename.c_str() ));

		string cs[] = { "p", "n"};

		for ( string plc : Common::species ){
			for ( string c : cs ){
				params[  plc + "_" + c  ] = unique_ptr<EffParams>( new EffParams( plc, cfg, plc + "_" + c ) );
			}
		}
	
	}

	double reweight( string plc, int c, int rm, double pt ){
		DEBUG( plc << ", " << c << ", " << rm << ", " << pt )
		string name = plc + "_" + Common::chargeString( c );
	
		if ( params.find( name ) != params.end() )
			return params[ name ]->reweight( pt, rm );

		ERROR( "Invalid" )
		return 0.0;

	}

	~SpectraCorrecter(){

	}
	
};


#endif