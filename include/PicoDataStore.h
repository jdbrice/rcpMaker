#ifndef PICO_DATA_STORE_H
#define PICO_DATA_STORE_H

class PicoDataStore
{
public:
	
	/**
	 * Kinematics
	 * @param  iHit The hit index in the current Event
	 * @return      the kinematic var for the current event and given hit
	 */
	virtual double trackPt( int iHit ) = 0;
	virtual double trackP( int iHit ) = 0;
	virtual double trackEta( int iHit ) = 0;

	/**
	 * Pid 
	 * @param  iHit The hit index in the current Event
	 * @return      Returns the given var
	 */
	virtual double trackDedx( int iHit ) = 0;
	virtual double trackPathLength( int iHit ) = 0;
	virtual double trackTof( int iHit ) = 0;
	virtual double trackBeta( int iHit ) = 0;

	/**
	 * Analysis Cuts
	 */

};


#endif