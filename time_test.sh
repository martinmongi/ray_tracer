#!/bin/bash
	rm times.out
	for t in `seq 25`;
	do	
        ./ray_tracer input/sphere.in 0 > /dev/null 2>> times.out
    done
    cat times.out | python3 average.py
    for t in `seq 25`;
	do	
        ./ray_tracer input/sphere.in 1 > /dev/null 2>> times.out
    done
    cat times.out | python3 average.py