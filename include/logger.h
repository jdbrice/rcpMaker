#ifndef LOGGER_H
#define LOGGER_H 

#include <iostream>
#include <string>
using namespace std;

class Logger{


protected:
	/**
	 * For removing output seamlessly
	 * Not the most efficient but I like the way it allows the system to behave
	 */
	class NullBuffer : public std::streambuf
	{
	public:
	  int overflow(int c) { return c; }
	};
	class NullStream : public std::ostream {
	public:
		NullStream() : std::ostream(&sb) {}
		private: 
			NullBuffer sb;
	};

public:
	/**
	 * Logger - constructor
	 * @param	ll - log level
	 * @param	cs - class / namespace for formatting prefix
	 * @param 	*os - pointer to an output stream. Can be stdout, stderr or a file etc.
	 */
	Logger( int ll, string cs, ostream* os ){
		logLevel = ll;
		cSpace = cs;
		this->os = os;
	} 
	/**
	 * Logger - constructor
	 * @param	ll - log level
	 * @param	cs - class / namespace for formatting prefix
	 * Description          
	 * sets the output stream to stdout
	 */
	Logger( int ll = llWarn, string cs = "" ){
		logLevel = ll;
		cSpace = cs;
		os = &cout;
	}

	int getLogLevel() { return logLevel; }
	void setLogLevel( int ll ) { logLevel = ll; }

	string getClassSpace() { return cSpace; }
	void setClassSpace(string cs ) { cSpace = cs; }

	ostream & warn( string functionName = "" ){
		if ( logLevel < llWarn )
			return ns;

		preMessage( "Warning:", functionName );
		
		return (*os);
	}

	ostream & error( string functionName = "" ){
		if ( logLevel < llError )
			return ns;

		preMessage( "ERROR", functionName );
		
		return (*os);
	}

	ostream & info( string functionName = "" ){
		if ( logLevel < llInfo )
			return ns;

		preMessage( "info", functionName );
		
		return (*os);
	}

	static const int llAll 		= 40;
	static const int llInfo		= 30;
	static const int llWarn 	= 20;
	static const int llError 	= 10;
	static const int llNone 	= 0;


protected:
	
	int logLevel;

	void preMessage( string level = "", string functionName = "" ){

		if ( cSpace.length() >= 2 && functionName.length() >= 2 )
			(*os) << level << " : " << "[" << cSpace << "." << functionName << "] ";
		else if (cSpace.length() < 2 && functionName.length() >= 2)
			(*os) << level << " : " << "[" << functionName << "] ";
		else if ( level.length() >= 1 )
			(*os) << "" << level << " : ";
	}

	std::ostream* os;
	NullStream ns;

	string cSpace;


};



#endif