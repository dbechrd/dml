#!/bin/bash
rm dml.exe dml.ilk dml.pdb
gcc -std=c99 -g -Wno-deprecated-declarations -I ../../dlb -o dml.exe *.c
