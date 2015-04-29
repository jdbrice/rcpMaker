#ifndef TSF_FIT_RANGE
#define TSF_FIT_RANGE

#include "Utils.h"
using namespace jdb;

namespace TSF{
	class FitRange
	{
	public:

		string dataset;
		double min, max;

		FitRange( string ds, double _min, double _max ){

			dataset = ds;
			min = _min;
			max = _max;
		}
		~FitRange(){}

		bool inRange( double x ){
			if ( x < min || x > max )
				return false;
			return true;
		}

		string toString(){
			return "( " + dts(min) + " < " + dataset + " < " + dts(max) + " )";
		}
		
	};	
}


#endif