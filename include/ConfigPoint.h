#ifndef CONFIG_POINT_H
#define CONFIG_POINT_H

#include "XmlConfig.h"

namespace jdb{

	class ConfigPoint
	{
	public:

		double x;
		double y;

		ConfigPoint( XmlConfig * cfg, string np, double dX = 0, double dY = 0, string attrX = ":x", string attrY = ":y"){
			if ( cfg ){
				x = cfg->getDouble( np + attrX, dX );
				y = cfg->getDouble( np + attrY, dY );
			} else {
				x = dX;
				y = dY;
			}
		}
		~ConfigPoint(){}
		
	};

}


#endif