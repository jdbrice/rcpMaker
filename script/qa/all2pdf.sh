#!/bin/bash

for x in imageEvent/*.eps; do
	echo $x;
	pstopdf $x ;
done

for x in imageTrack/*.eps; do
	echo $x;
	pstopdf $x ;
done