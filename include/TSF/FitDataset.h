#ifndef TSF_FIT_DATA_SET_H
#define TSF_FIT_DATA_SET_H 

//TSF
#include "TSF/FitDataPoint.h"


// STL
#include <vector>
#include <utility>
#include <string>

namespace TSF{

	class FitDataset
	{
	protected:

		vector<FitDataPoint> points;
		string name;

	public:
		FitDataset( ){}
		~FitDataset(){}

		vector<FitDataPoint>::iterator begin(){
			return points.begin();
		}
		vector<FitDataPoint>::iterator end(){
			return points.end();
		}

		void setName( string _name ){
			name = _name;
		}
		void clear(){
			points.clear();
		}
		void push_back( FitDataPoint fdp ){
			points.push_back( fdp );
		}

		FitDataPoint operator[]( int index ) { 
			if ( index >= 0 && index < points.size() )
				return points[ index ];
			FitDataPoint fdp;
			return fdp;
		}
		pair<double, double> rangeX( ){
			double min = 0, max = 0;

			for ( FitDataPoint fdp : points ){
				if ( fdp.x < min  ) min = fdp.x;
				if ( fdp.x > max  ) max = fdp.x;
			}
			return make_pair( min, max );
		}
		pair<double, double> rangeY( ){
			double min = 0, max = 0;

			for ( FitDataPoint fdp : points ){
				if ( fdp.y < min  ) min = fdp.y;
				if ( fdp.y > max  ) max = fdp.y;
			}
			return make_pair( min, max );
		}
		double yield( ){
			double n = 0;
			for ( FitDataPoint fdp : points ){
				n += fdp.y;
			}
			return n;
		}

		FitDataPoint pointNear( double x ){

			double mindis = 10000;
			FitDataPoint p;
			for ( FitDataPoint fdp : points ){
				double dis = TMath::Abs( x - fdp.x );

				if ( dis < mindis ){
					mindis = dis;
					p = fdp;
				}
			}
			return p;
		}
		
	};	
}


#endif