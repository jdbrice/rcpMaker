#ifndef TSF_GAUSS_MODEL_H
#define TSF_GAUSS_MODEL_H

// RooBarb
#include "XmlConfig.h"
#include "Logger.h"
#include "Util.h"
using namespace jdb;

// Local
#include "TSF/FitVar.h"

// STL
#include <vector>
#include <map>
#include <memory>
#include <utility>

// ROOT
#include "TMath.h"


namespace TSF{
	
	class GaussModel
	{
	protected:

		

	public:

		string dataset, name, yield, mu, sigma;
		double y, m, s;

		double e;
		string eff;

		GaussModel( XmlConfig * cfg, string np, string _dataset = "" ){


			name 	= cfg->getString( np + ":name" );
			yield 	= cfg->getString( np + ":yield" );
			mu 		= cfg->getString( np + ":mu" );
			sigma 	= cfg->getString( np + ":sigma" );
			eff 	= cfg->getString( np + ":eff", "na" );

			dataset = cfg->getString( np + ":dataset", _dataset );

		}
		GaussModel( string _name, string _yield, string _mu, string _sigma, string _ds, string _eff = "na" ){

			name = _name;
			yield = _yield;
			mu = _mu;
			sigma = _sigma;
			eff = _eff;
			dataset = _ds;

		}

		GaussModel( const GaussModel &other ){
			this->dataset = other.dataset;
			this->name = other.name;
			this->yield = other.yield;
			this->mu = other.mu;
			this->eff = other.eff;
			this->y = other.y;
			this->m = other.m;
			this->s = other.s;
			this->s = other.s;
			this->sigma = other.sigma;
		}

		~GaussModel(){}

		void setVars( map< string, shared_ptr<FitVar> > &vars ){
			m = vars[ mu ]->val;
			y = vars[ yield ]->val;
			s = vars[ sigma ]->val;

			if ( vars.count( eff ) )
				e = vars[ eff ]->val; 
			else 
				e = 1.0;
		}

		double eval( double x, double bw = 1.0 ){

			// e is an efficiency param
			double a = e * (y * bw) / ( s * TMath::Sqrt( 2 * TMath::Pi() ) );
			double b = TMath::Exp(  -( x - m ) * ( x - m ) / ( 2 * s * s ) );

			return a * b;
		}
		double integral( double x1 = 0, double x2 = 0){

			return 0.0;
		}

		string toString() { 
			if ( "na" == eff )
				return "Gauss( " + name + ": " + dataset + ", mu=" + dts(m) + ", sig=" +dts(s) +", y=" +dts(y);
			else 
				return "Gauss( " + name + ": " + dataset + ", mu=" + dts(m) + ", sig=" +dts(s) +", y=" +dts(y) + ", e=" + dts(e);
		}


		
	};

}


#endif