/*
 * Program Name: CnC Framework Unit Tests
 * File Name: unitTests.c
 * Date Created: October 19, 2024
 * Date Updated: November 11, 2024
 * Version: 0.4
 * Purpose: Unit Tests for the Framework
 */

#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include <platformCode.h>
#include <storage.h>
#include <timing.h>

#define TESTNAME "UnitTest"

double DEFAULT_RESULTS[16] = {
    0.0, 1.0, 2.0, 3.0,
    4.0, 5.0, 6.0, 7.0,
    8.0, 9.0, 10.0, 11.0,
    12.0, 13.0, 14.0, 15.0
};

char DEFAULT_NAMES[16][256] = {
    "COLUMN0",
    "COLUMN1",
    "COLUMN2",
    "COLUMN3"
};

CnCData data = {
    .isMalformed = 0,
    .testName = {},
    .resultCount = 16,
    .columnCount = 4,
    .columnNames = &DEFAULT_NAMES[0],
    .resultList = &DEFAULT_RESULTS[0],
}; //Test struct needs to be populated

/*
 * Test the storage API's ability to read and write accurately
 * @Return: 0 if successful, 1 for verification failure, and 2 for IO error.
 */

int testStorage()
{
    if (write_CNC(TESTNAME, data.resultList, data.resultCount, data.columnCount, data.columnNames) != 0)
        return 2;
    CnCData data_copy = read_CNC(TESTNAME);

    // Compare the 2 versions of the data
    if (data.columnCount != data_copy.columnCount)
        return 1;
    for (int i = 0; i < data.columnCount; i++)
        if (strcmp(data.columnNames[i], data_copy.columnNames[i]))
            return 1;
    if (data.resultCount != data_copy.resultCount)
        return 1;
    for (int i = 0; i < data.resultCount; i++)
        if (data.resultList[i] != data_copy.resultList[i])
            return 1;
    if (data.isMalformed != 0 & data_copy.isMalformed != 0)
        return 1;

    return 0;
}

/*
 * Test the affinity getter/setter
 * @Return: 0 if successful, 1 for verification failure
 */

int testAffinity()
{
    pthread_t thread = pthread_self();
    int affinity = 1;

    setAffinity(thread, affinity);
    int new_affinity = getAffinity(thread);
    if(new_affinity == -1 || new_affinity != affinity)
        return 1;
    return 0;
}

/*
 * Just a function to occupy the cpu for a bit
 */
int delay(int iterations)
{
    volatile double a = 34.567876867;
    volatile double b = 24.313214355;
    for(int i; i < iterations; i++)
    {
        a += b;
    }
}

/*
 * Test the timing function
 * @Return: time in ns for execution, or 0 for failure
 */
int testTiming()
{
    return timeExecution((timed_execution_function_t) delay, (void *)1000000000, 1000000000);
}


int main(int argc, char *argv[])
{
    printf("CnC Framework Unit Tests.  Return code 0 for success, 1 for verification failure, and 2 for IO error");

    int storageResult = testStorage();
    printf("Storage Test exited with return code %i\n", storageResult);

    int affinityResult = testAffinity();
    printf("Thread Affinity Test exited with return code %i\n", affinityResult);

    int timingResult = testTiming();
    printf("Timing Test exited with result time of %i nanoseconds\n", timingResult);

    //Append .cnc to the testName input.  File type is ALWAYS .cnc
    char AppendedName[255];
    strcpy(AppendedName, TESTNAME);
    strcat_s(AppendedName, 255, ".cnc");
    //Cleans up UnitTest.cnc by default.
    if(!remove(AppendedName))
    {
        printf("%s deleted successfully\n", AppendedName);
    }
    else
    {
        printf("%s could not be deleted\n", AppendedName);
    }
    return 0;
}
