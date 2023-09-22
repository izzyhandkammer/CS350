/*******************************************************************************
* Time Functions Library (implementation)
*
* Description:
*     A library to handle various time-related functions and operations.
*
* Author:
*     Renato Mancuso <rmancuso@bu.edu>
*
* Affiliation:
*     Boston University
*
* Creation Date:
*     September 10, 2023
*
* Notes:
*     Ensure to link against the necessary dependencies when compiling and
*     using this library. Modifications or improvements are welcome. Please
*     refer to the accompanying documentation for detailed usage instructions.
*
*******************************************************************************/

#include "timelib.h"

/* Return the number of clock cycles elapsed when waiting for
 * wait_time seconds using sleeping functions */

uint64_t get_elapsed_sleep(long sec, long nsec)
{
	//printf("this works");
	//fflush(stdout);
	uint64_t start, end;
	get_clocks(start);
	struct timespec time;
	time.tv_sec = sec;
	time.tv_nsec = nsec;
	nanosleep(&time, NULL);
	get_clocks(end);
	return end - start;

	//get_clocks(start);
	//time.tv_sec = sec;
	//time.tv_nsec = nsec;
	//while (nanosleep(&time, &remaining_time) == -1) {
        // Sleep was interrupted, re-calculate remaining time
	//	time = remaining_time;
	//}
	//end = get_clocks();
	//get_clocks(&end);
	//return end - start; 
	/* uint64_t bef, aft;
	get_clocks(bef);
	struct timespec tim, tim2;
	tim.tv_sec = sec;
	tim.tv_nsec = nsec;
	nanosleep(&tim,&tim2);
	get_clocks(aft);
	return aft-bef; */
}


/* Return the number of clock cycles elapsed when waiting for
 * wait_time seconds using busy-waiting functions */

uint64_t get_elapsed_busywait(long sec, long nsec)
{
	/*printf("this works");
	fflush(stdout);
	uint64_t before, after;
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC,&start);
	before = get_clocks();
	end.tv_sec = sec;
	end.tv_nsec = nsec;
	timespec_add(&end, &start);
	while (timespec_cmp(&end, &start)>0) {
		clock_gettime(CLOCK_MONOTONIC, &start);
	}
	after = get_clocks();
	return after-before; */
	struct timespec begin, curr, target;
	uint64_t start, end;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	target = begin;
	struct timespec delay = {.tv_sec = sec, .tv_nsec = nsec};
	timespec_add (&target, &delay);
	get_clocks (start);
	do
	{
		clock_gettime (CLOCK_MONOTONIC, &curr);
	}while (timespec_cmp(&curr, &target) < 0) ;
	get_clocks(end) ;
	return end - start;
	// uint64_t before, after;
	// get_clocks(before);
	// return before;
}

/* Utility function to add two timespec structures together. The input
 * parameter a is updated with the result of the sum. */
void timespec_add (struct timespec * a, struct timespec * b)
{
	/* Try to add up the nsec and see if we spill over into the
	 * seconds */
	time_t addl_seconds = b->tv_sec;
	a->tv_nsec += b->tv_nsec;
	if (a->tv_nsec > NANO_IN_SEC) {
		addl_seconds += a->tv_nsec / NANO_IN_SEC;
		a->tv_nsec = a->tv_nsec % NANO_IN_SEC;
	}
	a->tv_sec += addl_seconds;
}

/* Utility function to compare two timespec structures. It returns 1
 * if a is in the future compared to b; -1 if b is in the future
 * compared to a; 0 if they are identical. */
int timespec_cmp(struct timespec *a, struct timespec *b)
{
	if(a->tv_sec == b->tv_sec && a->tv_nsec == b->tv_nsec) {
		return 0;
	} else if((a->tv_sec > b->tv_sec) ||
		  (a->tv_sec == b->tv_sec && a->tv_nsec > b->tv_nsec)) {
		return 1;
	} else {
		return -1;
	}
}

/* Busywait for the amount of time described via the delay
 * parameter */
// uint64_t busywait_timespec(struct timespec delay)
// {
// 	/* IMPLEMENT ME! (Optional but useful) */
// }
//given in class

