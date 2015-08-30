#ifndef PID_POINT_H
#define PID_POINT_H 

#include "TNamed.h"

#include <vector>

class PidPoint : public TNamed {

public:

	PidPoint(){
		pT_bin = -1;
	}
	~PidPoint(){}

	int pT_bin;
	std::vector<double> zb;
	std::vector<double> zd;

	ClassDef(PidPoint,2); // PidPoint

};

#endif