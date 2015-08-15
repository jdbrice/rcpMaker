#ifndef TSF_FIT_DATA_SET_H
#define TSF_FIT_DATA_SET_H 

//TSF
#include "TSF/FitDataPoint.h"
#include "TSF/FitRange.h"


// STL
#include <vector>
#include <utility>
#include <string>
#include <cmath>

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
		
		double yield(  ){

			double n = 0;
			for ( FitDataPoint fdp : points ){
				n += fdp.y;
			}
			return n;
		}
		double yield( vector<FitRange> &ranges ){

			if ( 0 == ranges.size()  ){
				double n = 0;
				for ( FitDataPoint fdp : points ){
					n += fdp.y;
				}
				return n;	
			} else {
				double n = 0;
				for ( FitDataPoint fdp : points ){
					if ( inRange( ranges, fdp.x ) )
						n += fdp.y;
				}
				return n;
			}
			return 0;
		}

		bool inRange( vector<FitRange> &ranges, double x ){
			bool foundDS = false;
			for ( FitRange fr : ranges ){

				if ( name == fr.dataset ){
					foundDS = true;
					if ( fr.inRange( x ) )
						return true;
				}
			}
			if ( foundDS )
				return false;
			else
				return true;
		}

		FitDataPoint pointNear( double x ){

			double mindis = 10000;
			FitDataPoint p;
			for ( FitDataPoint fdp : points ){
				double dis = abs( x - fdp.x );

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