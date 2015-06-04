#ifndef REF_MULT_CORRECTION_H
#define REF_MULT_CORRECTION_H


/**
 * JDB
 */
#include "XmlConfig.h"
#include "ConfigRange.h"
#include "Logger.h"
#include "HistoBins.h"
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
	HistoBins * centralityBins;
	vector<double> zParameters;
	vector<double> wParameters;
	double weightStop;		// maximum M for which the weighing applies
	vector<int> badRuns;



public:

	RefMultCorrection(string paramFile ){

		logger = new Logger();
		logger->setClassSpace( "RefMultCorrection" );

		logger->info(__FUNCTION__) << "Loading params from " << paramFile << endl;
		cfg = new XmlConfig( paramFile.c_str() );

		year 		= cfg->getInt( "year" );
		energy 		= cfg->getDouble( "energy" );
		zVertexRange= new ConfigRange( cfg, "zVertex" );
		runRange	= new ConfigRange( cfg, "runRange" );
		zParameters = cfg->getDoubleVector( "zParameters" );
		wParameters = cfg->getDoubleVector( "wParameters" );
		weightStop 	= cfg->getDouble( "wParameters:stop" );
		badRuns 	= cfg->getIntVector( "badRuns" );

		centralityBins = new HistoBins( cfg, "bins" );

		logger->info(__FUNCTION__) << "Year : " << year << endl;
		logger->info(__FUNCTION__) << "Energy : " << energy << endl;
		logger->info(__FUNCTION__) << "Z Vertex : " << zVertexRange->toString() << endl;
		logger->info(__FUNCTION__) << "Run Range : " << runRange->toString() << endl;
		logger->info(__FUNCTION__) << "Z Params : " ;
		for ( int i = 0; i < zParameters.size(); i++ )
			logger->info("", false) << zParameters[ i ] << " ";
		logger->info("", false) << endl;

		logger->debug( __FUNCTION__ ) << "Centrality Bins (80-75% -> 0-5%) : " << endl;
		logger->debug( __FUNCTION__ ) << centralityBins->toString() << endl;

		logger->debug( __FUNCTION__ ) << endl;
		logger->debug( __FUNCTION__ ) << "Centrality bin16 sanity" <<endl;
		logger->debug( __FUNCTION__ ) << "Centrality Bin for M=3 (-1) = " << bin16( 3 ) << endl;
		logger->debug( __FUNCTION__ ) << "Centrality Bin for M=8 (0)= " << bin16( 8 ) << endl;
		logger->debug( __FUNCTION__ ) << "Centrality Bin for M=240 (15) = " << bin16( 240 ) << endl;
		logger->debug( __FUNCTION__ ) << "Centrality Bin for M=600 (-2)= " << bin16( 600 ) << endl;
		logger->debug( __FUNCTION__ ) << "Centrality bin9 sanity" <<endl;
		logger->debug( __FUNCTION__ ) << "Centrality Bin for M=3 (-1)= " << bin9( 3 ) << endl;
		logger->debug( __FUNCTION__ ) << "Centrality Bin for M=8 (0)= " << bin9( 8 ) << endl;
		logger->debug( __FUNCTION__ ) << "Centrality Bin for M=25 (2)= " << bin9( 25 ) << endl;
		logger->debug( __FUNCTION__ ) << "Centrality Bin for M=240 (8) = " << bin9( 240 ) << endl;
		logger->debug( __FUNCTION__ ) << "Centrality Bin for M=600 (-2)= " << bin9( 600 ) << endl;
		logger->debug( __FUNCTION__ ) << endl;

		logger->debug( __FUNCTION__ ) << "Bad Run Sanity Check" << endl;
		logger->debug( __FUNCTION__ ) << "Run 15047004 is Bad (BAD): " << isBad(15047004) << endl;
		logger->debug( __FUNCTION__ ) << "Run 15057050 is Bad (GOOD): " << isBad(15057050) << endl;
		logger->debug( __FUNCTION__ ) << endl;

		logger->debug( __FUNCTION__ ) << "Bad Run Sanity Check" << endl;
		logger->debug( __FUNCTION__ ) << "M=5 event weight : " << eventWeight( 5, 0 ) << endl;
		logger->debug( __FUNCTION__ ) << "M=10 event weight : " << eventWeight( 10, 0 ) << endl;
		logger->debug( __FUNCTION__ ) << "M=20 event weight : " << eventWeight( 20, 0 ) << endl;
		logger->debug( __FUNCTION__ ) << "M=29 event weight : " << eventWeight( 29, 0 ) << endl;
		logger->debug( __FUNCTION__ ) << endl;


		rGen = new TRandom3( );

	}
	~RefMultCorrection(){

		delete zVertexRange;
		delete runRange;
		delete cfg;
		delete logger;
	}


	bool isBad( int runIndex ){

		vector<int>::iterator it = std::find( badRuns.begin(), badRuns.end(), runIndex );
		if ( it == badRuns.end() )
			return false;
		return true;

	}
	int bin16( float m, double z = -1000 ){

		if ( z < -999 ){ // corrected refMult - just use
			int bin = centralityBins->findBin( m, BinEdge::upper );
			return bin;
		} else if ( z >= zVertexRange->min && z <= zVertexRange->max ){
			int bin = centralityBins->findBin( refMult( m, z ), BinEdge::upper );
			return bin;
		}
		return -1;
	}
	int bin9( float m, double z = -1000 ){
		int preBin = bin16( m, z );
		if ( preBin < 0 )
			return preBin;
		if ( preBin < 14 )
			return (preBin / 2); // integer divide and floor
		else if ( 14 == preBin )
			return 7;
		else if ( 15 == preBin )
			return 8;
		return -1;
	}
	int refMult( int rawRefMult, double z ){

		if ( z < zVertexRange->min || z > zVertexRange->max )
			return rawRefMult;

		double rmZ = zPolEval( z );
		if ( rmZ > 0 && zParameters.size() >= 1 ){

			const double center = zParameters[ 0 ];
			double corRefMult = center / rmZ;

			double refMultRnd = rawRefMult + rGen->Rndm();

			return TMath::Nint( refMultRnd * corRefMult );
		}

		logger->warn( __FUNCTION__ ) << "Ref Mult not corrected : zPol = " << rmZ << endl;
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
	// gets the event weight
	// @corrRefMult 	the corrected ref mult
	// @z 				z vertex
	// @return 			event weight
	double eventWeight( double corrRefMult, double z ){
		
		if ( wParameters.size() < 6 )
			return 1.0;
		if ( corrRefMult >= weightStop )
			return 1.0;
		// reference them individually as a pedigogical and verbose step
		const double p0 = wParameters[ 0 ];
		const double p1 = wParameters[ 1 ];
		const double p2 = wParameters[ 2 ];
		const double p3 = wParameters[ 3 ];
		const double p4 = wParameters[ 4 ];
		const double A 	= wParameters[ 5 ];

		if ( corrRefMult == -(p3/p2) )
			return 1.0;

		double weight = p0 + p1/(p2*corrRefMult + p3) + p4*(p2*corrRefMult + p3);
		weight = weight + ( weight - 1.0 ) * A * z * z;
		return weight;


	}
	
};


#endif