#ifndef SPACTRA_HISTO_FILE_MAKER_H
#define SPACTRA_HISTO_FILE_MAKER_H

#include "HistoAnalyzer.h"
#include "Common.h"

#include "ANSIColors.h"

class SpectraHistoFileMaker : public HistoAnalyzer
{

protected:

	string plc, energy;
public:
	virtual const char* classname() const { return "SpectraHistoFileMaker"; }
	SpectraHistoFileMaker() {}
	~SpectraHistoFileMaker() {}


	virtual void overrideConfig(){
		DEBUG( classname(), "" );
	}

	virtual void initialize() {
		INFO( classname(), "" );
		
	}



	virtual void make(){
		
	}// make


	


protected:
	

};


#endif