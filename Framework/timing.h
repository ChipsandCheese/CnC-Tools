#ifndef TIMING_H
#define TIMING_H
/*
 * Program Name: CnC Common Headers
 * File Name: timing.h
 * Date Created: January 24, 2024
 * Date Updated: October 16, 2024
 * Version: 0.5
 * Purpose: Provides a function to time the execution of the passed in function.
 */
#ifndef timespec
#include <time.h>
#endif
#ifndef uint64_t
#include <stdint.h>
#endif

//DO NOT DECLARE GLOBALS

/* @Param func: The pointer of the function being timed.
 * @Return: Time in nanoseconds (up to nanosecond precision).
 */

uint64_t timeExecution(void (*func)(int i), uint64_t iterations)
{
    //Only execute on positive values for iterations
    if(iterations > 0)
    {
        struct timespec start;
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &start);//Start timing
        func(iterations);
        clock_gettime(CLOCK_MONOTONIC, &end);//End timing
        uint64_t seconds = end.tv_sec - start.tv_sec;//calculate seconds diff
        uint64_t nanoseconds = end.tv_nsec - start.tv_nsec;//calculate nanoseconds diff
        return (seconds * 1e9) + nanoseconds;
    }
    else
    {
        return iterations;
    }
}

#endif // TIMING_H
