#ifndef SGF_SCHEMA_H
#define SGF_SCHEMA_H

/**
 * STL
 */
#include <map>
using namespace std;

/**
 * Roobarb
 */
#include "XmlConfig.h"
#include "Logger.h"
using namespace jdb;

/**
 * Local
 */
#include "XmlRooRealVar.h"

/**
 * RooFit
 */
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooCategory.h"
#include "RooSimultaneous.h"
#include "RooDataHist.h"
#include "RooPlot.h"

/**
 * Root
 */
#include "TH1D.h"


class SGFSchema
{
public:
	SGFSchema( XmlConfig * _cfg, string _np );
	~SGFSchema();

	void updateData( map<string, TH1D * > &dataHist, string var );
	void combineData();

	RooRealVar * var( string vName ){
		if ( nullptr == rrv[ vName ] ){
			l.error(__FUNCTION__) << vName << " DNE" << endl;
			return nullptr;
		}
		if ( nullptr == rrv[ vName ]->rrv ){
			l.error(__FUNCTION__) << " undefined" << endl;
			return nullptr;
		}
		return rrv[ vName ]->rrv;
		
	}

	RooDataHist * data( string name = "") {
		if ( "" == name )
			return allData;
		return rdhSingle[ name ];
	}

	RooSimultaneous * model() { return simModel; }
	RooCategory * category() { return rSample; }

	void resetYield( string plc, double _yield );
	void setInitial( string var, string plc, double _mu, double _sigma, double _dmu = 1.5, double _dsigma = .25 );
	void fixSigma( string var, string plc, double sigma );

	

protected:

	void initialize();
	void makeVariables();

	void makeModels();
	void makeModel( string mnp );
	void makeSimultaneous();

	void forceROI();

	XmlConfig * cfg;
	string np;

	// RooRealVars
	map< string, XmlRooRealVar * > rrv;
	map< string, RooGaussian * > gausses;
	map< string, string > gaussIndVar;
	map< string, RooAddPdf * > models;
	map< string, string > modelSample;
	map< string, RooDataHist * > rdhSingle;
	RooCategory * rSample;
	RooSimultaneous * simModel;
	RooDataHist * allData;
	

	Logger l;
	
};



#endif