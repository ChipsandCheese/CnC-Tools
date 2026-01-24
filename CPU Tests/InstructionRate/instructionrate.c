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
int threads = getThreadCount();

int main (int argc, char *argv[])
{
    uint64_t iterations;

    //Get clock speed first
    timeExecution((clockTest), NULL, iterations);
}