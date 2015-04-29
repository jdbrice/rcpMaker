#ifndef PID_PROBABILITY_MAPPER_H
#define PID_PROBABILITY_MAPPER_H

// ROOBARB
#include "HistoAnalyzer.h"
#include "HistoBins.h"

// STL
#include <map>
#include <string>
using namespace std;

class PidProbabilityMapper : HistoAnalyzer
{
protected:

	// binning
	unique_ptr<HistoBins> binsPt;
	unique_ptr<HistoBins> binsEta;
	unique_ptr<HistoBins> binsCharge;

	double evaluateGauss( double x, double y, double m, double s );
	

public:
	PidProbabilityMapper( XmlConfig * cfg, string np );
	~PidProbabilityMapper();
	
	map<string, double> pidWeights( int charge, int iCen, double pt, double eta, double zb, double zd );
};


#endif