#ifndef CONFIG_RANGE_H
#define CONFIG_RANGE_H

#include "XmlConfig.h"

namespace jdb{

	class ConfigRange
	{
	public:

		double min;
		double max;

		ConfigRange( XmlConfig * cfg, string np, double dMin = 0, double dMax = 0, string attrMin = ":min", string attrMax = ":max"){
			if ( cfg ){
				min = cfg->getDouble( np + attrMin, dMin );
				max = cfg->getDouble( np + attrMax, dMax );
			} else {
				min = dMin;
				max = dMax;
			}
		}
		~ConfigRange();
		
	};

}


#endif