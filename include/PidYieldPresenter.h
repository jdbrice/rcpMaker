#ifndef PID_YIELD_PRESENTER_H
#define PID_YIELD_PRESENTER_H

/**
 * STL
 */
#include <memory>
#include <string>
using namespace std;

/**
 * Roobarb
 */
#include "XmlConfig.h"
#include "HistoBook.h"
#include "LoggerConfig.h"
#include "Reporter.h"
using namespace jdb;

/**
 * ROOT
 */
#include "TFile.h"
#include "TH1D.h"

/**
 * Local
 */
#include "SGFRunner.h"
#include "PidPhaseSpace.h"

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

	

	/**
	 * Binning in Config
	 */
	unique_ptr<HistoBins> binsPt;
	unique_ptr<HistoBins> binsEta;
	unique_ptr<HistoBins> binsCharge;


public:
	PidYieldPresenter( XmlConfig * _cfg, string _nodePath );
	~PidYieldPresenter();


	/**
	 * Takes the fit results and produces a single spectra for each centrality, species, charge
	 * Outputs into the book into directory /raw/yieldName( plc, charge, iCen )
	 */
	void integrateEta();
	void integrateEta( string plc, int charge, int cBin );

	/**
	 * Takes the eta integrated yields and applies normalization to them
	 * outputs into book /yield/yieldName( plc, charge, iCen )
	 */
	void normalizeYield();
	void normalizeYield( string plc, int charge, int cBin );

	void rcp();
	void rcp( string plc, int charge, int iCen = 0, int iPer = 6 );

	void rcpPannel();

	void compareYields( string plc, int charge );

	string rcpName( string plc, int charge ){
		return "rcp_" + plc + "_" + PidPhaseSpace::chargeString( charge );
	}

	string chargeString( int charge ){
		if ( charge > 0 )
			return "+";
		else if ( charge < 0 )
			return "-";

		return "err";
	}

	
};


#endif