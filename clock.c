/*******************************************************************************
* CPU Clock Measurement Using RDTSC
*
* Description:
*     This C file provides functions to compute and measure the CPU clock using
*     the `rdtsc` instruction. The `rdtsc` instruction returns the Time Stamp
*     Counter, which can be used to measure CPU clock cycles.
*
* Author:
*     Renato Mancuso
*
* Affiliation:
*     Boston University
*
* Creation Date:
*     September 10, 2023
*
* Notes:
*     Ensure that the platform supports the `rdtsc` instruction before using
*     these functions. Depending on the CPU architecture and power-saving
*     modes, the results might vary. Always refer to the CPU's official
*     documentation for accurate interpretations.
*
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "timelib.h"

int main (int argc, char ** argv)
{
	/* IMPLEMENT ME! */
	long sec = atol(argv[1]);
    long nsec = atol(argv[2]);
    char method = argv[3][0];
    uint64_t elapsed = 0;

	if (method != 's' && method != 'b') {
        fprintf(stderr, "Invalid method. Use 's' for sleep or 'b' for busy-wait.\n");
        return 1;
    }

	if (method == 's'){
        //fprintf(stderr, "this works\n");
        //return 1;
		elapsed = get_elapsed_sleep(sec, nsec);
        printf("WaitMethod: SLEEP\n");
	} else {
        elapsed = get_elapsed_busywait(sec, nsec);
		printf("WaitMethod: BUSYWAIT\n");
	}
    double total_sec = sec + (double)nsec/1e9;
    double clock_speed = (double)elapsed / total_sec / 1e6;

    printf("WaitTime: %ld %ld\n", sec, nsec);
    printf("ClocksElapsed: %llu\n", (unsigned long long)elapsed);
    printf("ClockSpeed: %.2f\n", clock_speed);

    return 0; 
    /*
    long sec=atol(argv[1]);
    long nsec=atol(argv[2]);
    char method= argv[3][0];
    unit64_t elapsed = 0;
    if (method=='s') {
        uint64_t elapsed = get_elapsed_sleep(sec,nsec);
        printf("WaitMethod: %s\n","SLEEP");
        /*uint64_t x= get_elapsed_busywait(sec,nsec);
        printf("WaitMethod: %s\n", "BUSYWAIT");
        printf("WaitTime: %ld %ld \n", sec,nsec);
        printf("ClocksElapsed: %lu\n", x);
        double time=(double)sec + ((double)nsec)/1e9;

        //printf("ClockSpeed: %.2f\n", (x/(double)time)*1e-6);
    } else {
        printf("try again!\n" );
    }
    return 0; */
}

