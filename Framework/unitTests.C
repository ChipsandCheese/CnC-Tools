/*
 * Program Name: CnC Framework Unit Tests
 * File Name: unitTests.c
 * Date Created: October 19, 2024
 * Date Updated: October 27, 2024
 * Version: 0.2.1
 * Purpose: Unit Tests for the Framework
 */

#include <stdio.h>
#include <storage.h>

#define TESTNAME "UnitTest.cnc"

CnCData data = {}; //Test struct needs to be populated

/*
 * Test the storage API's ability to read and write accurately
 * @Return: 0 if successful, -1 for IO error, -2 for verification failure
 */

int testStorage()
{
    if (write_CNC(TESTNAME, data.resultList, data.resultCount, data.columnCount, data.columnNames) != 0)
        return -1;
    CnCData data_copy = read_CNC(TESTNAME);

    // Compare the 2 versions of the data
    if (data.columnCount != data_copy.columnCount)
        return -2;
    for (int i = 0; i < data.columnCount; i++)
        if (strcmp(data.columnNames[i], data_copy.columnNames[i]))
            return -2;
    if (data.resultCount != data_copy.resultCount)
        return -2;
    for (int i = 0; i < data.resultCount; i++)
        if (data.resultList[i] != data_copy.resultList[i])
            return -2;
    if (data.isMalformed != 0 & data_copy.isMalformed != 0)
        return -2;

    return 0;
}

int main(int argc, char *argv[])
{

    int result = 0;

    printf("Storage test exited with return code %i\n", result |= testStorage());
    remove(TESTNAME); //Cleans up Test Log by default.

    return 0;
}
