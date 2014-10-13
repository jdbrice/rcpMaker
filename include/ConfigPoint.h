#ifndef ConfigPoint
#define ConfigPoint

#include "XmlConfig.h"

namespace jdb{

	class ConfigPoint
	{
	public:

		double x;
		double y;

		ConfigPoint( XmlConfig * cfg, string np, string attrX = ":x", string attrY = ":y"){
			if ( cfg ){
				x = cfg->getDouble( np + attrX, 0 );
				y = cfg->getDouble( np + attrY, 0 );
			} 
		}
		~ConfigPoint();
		
	};

}


#endif