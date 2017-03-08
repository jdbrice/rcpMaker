#ifndef STATUS_H
#define STATUS_H

#include "HistoAnalyzer.h"


class Status : public HistoAnalyzer
{
public:
	virtual const char* classname() const { return "Status"; }
	Status() {}
	~Status() {}
	virtual void initialize() {
		HistoAnalyzer::initialize();



	}

	virtual void make(){

		// Check TFiles
		vector<string> names = filenames();
		for ( string fn : names ){
			if ( fileExists( fn ) ){
				INFOC( "Checking for " << fn << " : " << ANSIColors::color( "EXISTS", "green" ) );
			} else {
				INFOC( "Checking for " << fn << " : " << ANSIColors::color( "DOES NOT EXIST", "red" ) );
			}
		}
	}
protected:

};


#endif