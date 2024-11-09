/*
 * Program Name: CnC Framework Unit Tests
 * File Name: unitTests.c
 * Date Created: October 19, 2024
 * Date Updated: November 9, 2024
 * Version: 0.3
 * Purpose: Unit Tests for the Framework
 */

#include <stdio.h>
#include <storage.h>

#define TESTNAME "UnitTest.cnc"

CnCData data = {}; //Test struct needs to be populated

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

int testAffinity()
{
    return 0;
}

int testTiming()
{
    return 0;
}

int main(int argc, char *argv[])
{
    printf("CnC Framework Unit Tests.  Return code 0 for success, 1 for verification failure, and 2 for IO error");

    int storageResult = testStorage();
    printf("Storage Test exited with return code %i\n", storageResult);

    int affinityResult = testAffinity();
    printf("Thread Affinity Test exited with return code %i\n", affinityResult);

    int timingResult = testTiming();
    printf("Timing Test exited with return code %i\n", timingResult);

    //Cleans up UnitTest.cnc by default.
    if(!remove(TESTNAME))
    {
        printf("%s deleted successfully", TESTNAME);
    }
    else
    {
        printf("%s could not be deleted", TESTNAME);
    }
    return 0;
}
