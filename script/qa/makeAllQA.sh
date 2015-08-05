#!/bin/bash


root -l -b -q event.C
root -l -b -q track.C
./all2pdf.sh
./publish.sh