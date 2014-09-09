
#ifndef JDB_UTILS_H
#define JDB_UTILS_H 

#include <string>

// for timing functions
#include <time.h>  

// for ttys detection
#include <unistd.h> 

// for round
#include <math.h>

using namespace std;


namespace jdbUtils{

	class taskTimer{
	public:

		clock_t startTime;
		void start( ) { startTime = clock(); }
		double elapsed( ) { return ( (clock() - startTime) / (double)CLOCKS_PER_SEC ); }
		
	};

	/**
	 * String utilities that need >= c++ 2011
	 */
	inline std::string ts( int i, int len = -1){
		if ( len <= -1 )
			return (to_string( (long long int) i));
		else
			return (to_string( (long long int) i)).substr( 0, len );
	}
	inline std::string ts( double d, int len = -1){
		if ( len <= -1 )
			return to_string( (long double) d);
		else 
			return (to_string( (long double) d)).substr( 0, len );
	}
	inline std::string ts( float f, int len  = -1){
		return ts( (double) f, len );
	}
	inline std::string ts( uint u, int len  = -1){
		if ( len <= -1 )
			return to_string( (long long unsigned int) u);
		else 
			return (to_string( (long long unsigned int) u)).substr( 0, len );
	}

	
	/**
	 * Plain text progress bar 
	 * simply call inside loop
	 */
	
	inline void progressBar( int i, int nevents, int textWidth = 60, double elapsedTime = -1 ){
		
		double progress =  ((double)i / (double)nevents);
		if ( i == nevents - 1)
    		progress = 1.001;


    	int res = (int)((double)nevents / (double) textWidth);
    	if ( res == 0 || res == 1)
    		res = i;

    	// decide when to update
    	if ( i == 0 || (i % res ) == 0 || i == nevents - 1  ){
			// skip for non interactive output
			if (!isatty(fileno(stdout)) && progress <= 1 )
				return;

			double per = progress  * 100;
			per = round( per );

			cout << "[";
	    	for ( int ip = 0; ip < textWidth; ip ++ ){
	    		if ( ip < round( (progress * (double)textWidth) ) )
	    			cout << "=";
	    		else 
	    			cout << " ";
	    	}
	    	if (isatty(fileno(stdout)) ){ 
		 	   	cout << "]" << per << "%" ;
				if ( elapsedTime >= 0 ){
					cout << " : " << ts(elapsedTime, 4) << " sec ";
				}
				cout << "\r";
				std::cout.flush();
				if (progress > 1)
					cout << "[" << endl;
			} else {
					cout << "]" << per << "%" << "\n";
			}
		}
	}


	/**
	 * 
	 */
	class taskProgress{
	protected:
		int max = 100;
		int barWidth = 60;
		bool showTitle = true;
		bool showElapsed = true;
		string title = "";

		taskTimer tt;

	public:
		taskProgress(){

		}
		taskProgress( string title, int max = 100, int width = 60, bool sTitle = true, bool sElapse = true ){
			this->title = title;
			this->max = max;
			barWidth = width;
			showTitle = sTitle;
			showElapsed = sElapse;
		}

		void showProgress( int i ){
			if ( 0 == i){
				tt.start();
				if ( showTitle )
					cout << "Running Task : " << title << endl;
			}

			if ( showElapsed )
				progressBar( i, max, barWidth, tt.elapsed() );
			else 
				progressBar( i, max, barWidth );
		}


	};

}



#endif