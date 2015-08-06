#ifndef PID_YIELD_PRESENTER_H
#define PID_YIELD_PRESENTER_H

// STL
#include <memory>
#include <string>
using namespace std;

// Roobarb
#include "XmlConfig.h"
#include "HistoBook.h"
#include "LoggerConfig.h"
#include "Reporter.h"
using namespace jdb;

// ROOT
#include "TFile.h"
#include "TH1D.h"

// Local
#include "TSF/FitRunner.h"
#include "Spectra/PidHistoMaker.h"

class PidYieldPresenter
{

protected:

	unique_ptr<Logger> logger;
	XmlConfig * cfg = nullptr;
	string np;

	unique_ptr<Reporter> reporter;
	unique_ptr<HistoBook> book;

	TFile * fPidFit;
	TFile * fPidPS;


	vector<int> 		cenBins;
	vector<string> 		cenLabels;
	vector<int> 		charges;
	vector<int> 		colors;
	vector<double> 		nPart;
	vector<double> 		nColl;

	string plc = "E";

	// Binning in Config
	unique_ptr<HistoBins> binsPt;


public:
	PidYieldPresenter( XmlConfig * _cfg, string _nodePath );
	~PidYieldPresenter();


	/**
	 * Outputs into book /yield/yieldName( plc, charge, iCen )
	 */
	void normalizeYield();
	void normalizeYield( string plc, int charge, int cBin );

	void rcp( int iPer = 6 );
	void rcp( string plc, int charge, int iCen = 0, int iPer = 6 );

	void rcpPanel( int iCen = 0, int iPer = 6);

	void rcpVsNPart( string plc, int charge, int ptBin, int iPer = 6 );
	void rcpVsNPart( int ptBin = 15, int iPer = 6 );
	void rcpVsNPartCompare( int ptBin, int iPer = 6 );

	void compareYields();
	void compareYields( string plc, int charge );

	void chargeRatio();
	void chargeRatio( string plc, int iCen );
	void chargeRatioCompare( string plc );
	void chargeRatioCompare( );


	string rcpName( string plc, int charge, int iCen, int iPer ){
		return "rcp_" + plc + "_" + Common::chargeString( charge ) + "_" + ts(iCen) + "_Over_" + ts(iPer);
	}
	string nPartName( string plc, int charge, int ptBin ){
		return "nPart_" + plc + "_" + Common::chargeString( charge ) +"_" + ts( ptBin );
	}
	string chargeRatioName( string plc, int iCen ){
		return "chargeRatio_" + plc + "_" + ts( iCen );
	}

	string chargeString( int charge ){
		if ( charge > 0 )
			return "+";
		else if ( charge < 0 )
			return "-";

		return "err";
	}

	string plcName( string plc, int charge ){

		if ( "K" == plc ){
			return ( plc + "^{" + chargeString(charge)  + "}" );
		} else if ( "P" == plc ){
			if ( 1 == charge )
				return "P";
			else 
				return "Pbar";
		} else if ( "Pi" == plc ){
			return ( "#pi^{" + chargeString( charge )  + "}" );
		}
		return "new plc";
	}

	
};


#endif