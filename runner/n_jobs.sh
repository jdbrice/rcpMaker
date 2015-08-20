#!/bin/bash
condor_q $1 | grep " R \| I " | wc -l