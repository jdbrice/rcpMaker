#ifndef REF_MULT_CORRECTION_H
#define REF_MULT_CORRECTION_H


/**
 * JDB
 */
#include "XmlConfig.h"
#include "ConfigRange.h"
#include "Logger.h"
using namespace jdb;
/**
 * ROOT
 */
#include "TMath.h"
#include "TRandom3.h"

class RefMultCorrection
{
protected:

	TRandom3 * rGen;
	Logger * logger;
	XmlConfig * cfg;

	int year;
	double energy;
	ConfigRange * zVertexRange;
	ConfigRange * runRange;
	vector<double> zParameters;



public:

	RefMultCorrection(string paramFile ){

		cout <<"Making logger" << endl;
		logger = new Logger();
		logger->setClassSpace( "RefMultCorrection" );

		logger->info(__FUNCTION__) << "Loading params from " << paramFile << endl;
		cfg = new XmlConfig( paramFile.c_str() );

		year 		= cfg->getInt( "year" );
		energy 		= cfg->getDouble( "energy" );
		zVertexRange= new ConfigRange( cfg, "zVertex" );
		runRange	= new ConfigRange( cfg, "runRange" );
		zParameters = cfg->getDoubleVector( "zParameters" );

		logger->info(__FUNCTION__) << "Year : " << year << endl;
		logger->info(__FUNCTION__) << "Energy : " << energy << endl;
		logger->info(__FUNCTION__) << "Z Vertex : " << zVertexRange->toString() << endl;
		logger->info(__FUNCTION__) << "Run Range : " << runRange->toString() << endl;
		logger->info(__FUNCTION__) << "Z Params : " ;
		for ( int i = 0; i < zParameters.size(); i++ )
			cout << zParameters[ i ] << " ";
		cout << endl;

		rGen = new TRandom3( 0 );

	}
	~RefMultCorrection(){

		delete zVertexRange;
		delete runRange;
		delete cfg;
		delete logger;
	}


	int refMult( int rawRefMult, double z ){

		if ( z < zVertexRange->min || z > zVertexRange->max )
			return rawRefMult;

		double rmZ = zPolEval( z );
		if ( rmZ > 0 && zParameters.size() >= 1 ){

			const double center = zParameters[ 0 ];
			double corRefMult = center / rmZ;

			double refMultRnd = rawRefMult + rGen->Rndm();

			return refMultRnd * corRefMult;
		}

		logger->warn( __FUNCTION__ ) << "Ref Mult not corrected " << endl;
		return rawRefMult;
	}

	double zPolEval( double z ){
		if ( zParameters.size() <= 0 ){
			logger->warn(__FUNCTION__) << "No Z Parameters " << endl;
			return 0;
		}

		double r = 0;
		//r = zParameters[ 0 ] + zParameters[ 1 ] * z;// + zParameters[ 2 ] * z * z + zParameters[ 3 ] * z * z * z + zParameters[ 4 ] * z * z * z * z + zParameters[ 5 ] * z*z*z*z*z
		for ( int i = 0; i < zParameters.size(); i++ ){
			r += zParameters[ i ] * TMath::Power( z, i );
		}
		return r;
	}
	
};


#endif