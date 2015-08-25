#ifndef TSF_FIT_DATA_POINT_H
#define TSF_FIT_DATA_POINT_H

//ROOBARB
#include "Utils.h"
using namespace jdb;

#include <string>
using namespace std;

namespace TSF{
	class FitDataPoint
	{
	public:
		double x, y, ey, bw;

		FitDataPoint(){
			x = 0;
			y = 0;
			ey = 0;
			bw = 0;
		}
		FitDataPoint( double _x, double _y, double _ey, double _bw = 1.0 ){
			x = _x;
			y = _y;
			ey = _ey;
			bw = _bw;
		}

		FitDataPoint( const FitDataPoint &other ){
			this->x = other.x;
			this->y = other.y;
			this->ey = other.ey;
			this->bw = other.bw;
		}
		~FitDataPoint(){}

		string toString(){
			return "( " + dts(x) + ", " + dts(y) + " +/- " + dts(ey) + " ) / " + dts(bw);
		}
		
	};
}



#endif