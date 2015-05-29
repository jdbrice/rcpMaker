#ifndef TSF_FIT_VAR_H
#define TSF_FIT_VAR_H 

//Roobarb
#include "XmlConfig.h"
#include "Util.h"
using namespace jdb;

#include <string>
using namespace std;

namespace TSF{
	class FitVar{

	public:
		string name;
		double val, min, max, error;
		bool fixed;
		bool exclude;

		/**
		 * <FitVar name="zb_mu_Pi" value="-0.5"  min="-0.6" max="-0.4" />
		 */
		FitVar( XmlConfig * cfg, string np ){

			if ( cfg ){

				name = cfg->getString( np + ":name" );

				set( 	cfg->getDouble( np+":value", 0 ),
						cfg->getDouble( np+":min", -1000 ),
						cfg->getDouble( np+":max", 10000 ), 
						cfg->getDouble( np+":error", .0001 )
					);

			} else {
				Logger::log.error(__FUNCTION__) << "Invalid Config" << endl;
			}
			fixed = false;
			exclude = false;

		}
		FitVar( string _name, double _val, double _min, double _max, double _error ){
			name = _name;
			set( _val, _min, _max, _error );
			fixed = false;
			exclude = false;
		}
		~FitVar(){}

		void set( double _val, double _min, double _max, double _error ){
			val = _val;
			min = _min;
			max = _max;
			error = _error;

			if ( min != 0 && min != max ){
					if ( val < min )
					val = min;
				if ( val > max )
					val = max;		
			}
		
		}

		void set( double _val, double _min, double _max ){
			val = _val;
			min = _min;
			max = _max;

			if ( min != 0 && min != max ){
					if ( val < min )
					val = min;
				if ( val > max )
					val = max;		
			}
		
		}

		string toString() {
			return name + " = " + dts(val) + "+/- " + dts(error) + " ( " + dts(min) + ", " + dts(max) + " )"; 
		}

	};
}




#endif