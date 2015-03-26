#ifndef XML_ROO_REAL_VAR_H
#define XML_ROO_REAL_VAR_H

#include <string>
using namespace std;
#include "XmlConfig.h"
using namespace jdb;

#include "RooRealVar.h"
using namespace RooFit;


class XmlRooRealVar
{
public:

	XmlRooRealVar( XmlConfig * _cfg, string _np ){

		cfg = _cfg;
		np = _np;

		set();

	}
	~XmlRooRealVar(){

	}

	void set( XmlConfig * _cfg = nullptr, string _np = "" ){


		if ( nullptr == _cfg )
			_cfg = cfg;
		if ( "" == _np )
			_np = np;

		string tCN = _cfg->cn( _np );

		// valid config
		if ( nullptr == cfg || "" == np ){
			Logger::log.error(__FUNCTION__) << "Invalid config or path" << endl;
			return;
		}
		// node has valid data
		if ( !cfg->exists( ":name" ) ){
			Logger::log.error(__FUNCTION__) << "No name given" << endl;
			return;
		}
		if ( 	!cfg->exists( ":value" ) && 
				!cfg->exists( ":min" ) && !cfg->exists( ":max" ) ){
			Logger::log.error(__FUNCTION__) << "Must provide rnage or value or both" << endl;
			return;
		}

		name = cfg->getString( ":name", "" );
		min = cfg->getDouble( ":min", 0 );
		max = cfg->getDouble( ":max", 0 );
		initialValue = cfg->getDouble( ":value", 0 );

		if ( nullptr != rrv )
			delete rrv;

		if ( 	cfg->exists( ":value" ) && 
					!cfg->exists( ":min" ) && !cfg->exists( ":max" ))
			rrv = new RooRealVar( name.c_str(), name.c_str(), initialValue );
		else if ( 	!cfg->exists( ":value" ) && 
					cfg->exists( ":min" ) && cfg->exists( ":max" ))
			rrv = new RooRealVar( name.c_str(), name.c_str(), min, max );
		else if ( 	cfg->exists( ":value" ) && 
					cfg->exists( ":min" ) && cfg->exists( ":max" ))
			rrv = new RooRealVar( name.c_str(), name.c_str(), initialValue, min, max );

		// change the current node back so we aren't destructive
		cfg->cn( tCN );

	}

	double getMin() const { return min; }
	double getMax() const { return max; }
	string getName() const { return name; }
	double getValue() const {
		if ( nullptr != rrv ){
			return rrv->getVal();
		}
		return 0;
	}

	
	RooRealVar * rrv = nullptr;
	
protected:

	XmlConfig * cfg = nullptr;
	string np = "";
	

	string name;
	double min, max, initialValue;



};




#endif