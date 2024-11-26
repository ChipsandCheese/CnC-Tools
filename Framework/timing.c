#include <generic/rte_cycles.h>
#include <rte_cycles.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>

#include "timing.h"

#define NS_PER_SEC 1E9

/*
 * Program Name: CnC Common Headers
 * File Name: timing.c
 * Date Created: November 11, 2024
 * Date Updated: November 11, 2024
 * Version: 0.1
 * Purpose: Provides a function to time the execution of the passed in function.
 */
uint64_t timeExecution(timed_execution_function_t func, void *data, int iterations) {
    if (iterations > 0) {
        uint64_t start = rte_rdtsc_precise();
        func(data);
        uint64_t end = rte_rdtsc_precise();
        double tsc_cycles_per_ns = ((double)rte_get_tsc_hz()) / NS_PER_SEC;
        double duration_ns = ((double)(end - start)) / tsc_cycles_per_ns;
        return duration_ns;
    } else
        return iterations;
}
