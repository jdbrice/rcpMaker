#ifndef EVALUATE_CONFIG_H
#define EVALUATE_CONFIG_H

#include "TaskRunner.h"


class EvaluateConfig : public TaskRunner
{
public:
	virtual const char* classname() const { return "EvaluateConfig"; } 
	EvaluateConfig() {}
	~EvaluateConfig() {}

	virtual void make(){

		string out = config.getXString( nodePath + ".output.XmlConfig:url", "evaluated_config.xml" );

		INFOC( "Evaluating config and saving to " << out );

		config.toXmlFile( out );
	}
	
};


#endif