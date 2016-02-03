#ifndef TEST_ANALYZER_H
#define TEST_ANALYZER_H

#include "TreeAnalyzer.h"
#include "Logger.h"
using namespace jdb;


class BaseAnalyzer
{
public:
	virtual const char* classname() const { return "BaseAnalyzer"; }
	BaseAnalyzer() {}
	~BaseAnalyzer() {}

	virtual void init( int test ) { DEBUG( classname(), "test=" << test ); }
	
};

class TestAnalyzer : public BaseAnalyzer {

public:
	TestAnalyzer() {}
	~TestAnalyzer() {}

	virtual void initialize(){
		DEBUG( classname(), "wow");
	}



};



#endif