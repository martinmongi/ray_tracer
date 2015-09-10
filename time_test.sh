#!/bin/bash
	rm times_c.out
	rm times_asm.out
	for t in `seq 25`;
	do	
        ./ray_tracer test.in 0 > /dev/null 2>> times_c.out
    done
    cat times_c.out | python3 average.py
    for t in `seq 25`;
	do	
        ./ray_tracer test.in 1 > /dev/null 2>> times_asm.out
    done
    cat times_asm.out | python3 average.py