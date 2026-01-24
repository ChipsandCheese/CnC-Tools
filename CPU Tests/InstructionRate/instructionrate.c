#ifndef FRAMEWORK
#define FRAMEWORK
#include "../../Framework/*"
#endif

/*
 * Program Name: Instruction Rate
 * File Name: instructionrate.c
 * Date Created: January 24, 2026
 * Date Updated: January 24, 2026
 * Version: 0.1
 * Purpose: TBA
 */

#define ADD_CYCLES 1
int threads = getThreadCount();

int main (int argc, char *argv[])
{
    uint64_t iterations;

    //Get clock speed first. Formula is add cycle count divided by latency (execution time in nanoseconds divided by iteration count)
    double cpuFreq = ADD_CYCLES / (double)(timeExecution((clockTest), NULL, iterations) / (double) iterations);
}