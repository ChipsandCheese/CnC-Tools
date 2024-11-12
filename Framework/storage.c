// Bring in the platformCode, just in case we're using GCC and resultingly don't have access to strcat_s :)
#include <platformCode.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <storage.h>

/*
 * Program Name: CnC Common Headers
 * File Name: storage.c
 * Date Created: November 11, 2024
 * Date Updated: November 11, 2024
 * Version: 0.1
 * Purpose: Provides functions for storage aspects of the framework.
 */

const uint8_t VERSIONCODE = 1;

CnCData read_CNC(char fileName[])
{
    //Copied for parity with write_CNC
    //Append .cnc to the testName input.  File type is ALWAYS .cnc
    char AppendedName[255];
    strcpy(AppendedName, fileName);
    strcat_s(AppendedName, 255, ".cnc");

    CnCData data = { .isMalformed = 1 };
    char buffer[BUFFERLIMIT];
    char *bufferSave;

    FILE *file;
    if((file = fopen(fileName, "r")) == NULL) return data;

    if (fgets(buffer, BUFFERLIMIT, file) == NULL)
        return data;

    //Parse first line for metadata
    if(VERSIONCODE != atoi(strtok_r(buffer, ",", &bufferSave))) return data;
    data.resultCount = (uint32_t) atoi(strtok_r(bufferSave, ",", &bufferSave));
    data.columnCount = (uint32_t) atoi(strtok_r(bufferSave, ",", &bufferSave));
    if((data.resultCount == 0) || (data.columnCount == 0)) return data;
    char *name = strtok_r(bufferSave, "\n", &bufferSave);
    memcpy(&data.testName[0], name, strlen(name));

    data.columnNames = malloc(data.columnCount * sizeof(char[256]));
    fgets(buffer, BUFFERLIMIT, file);
    bufferSave = buffer;

    // Grab each column name
    for (int i = 0; i < data.columnCount; i++)
    {
        char *token = strtok_r(bufferSave, ",", &bufferSave);
        int len = strlen(token);

        // Prevent oversized names, and remove newline
        if (len > 256)
            len = 256;
        else if (token[len - 1] == '\n')
            len -= 1;
        memcpy(data.columnNames[i], token, len);

        // Ensure it's null terminated
        data.columnNames[i][len] = 0;
    }

    data.resultList = (double *) malloc(data.resultCount * sizeof(double));
    fgets(buffer, BUFFERLIMIT, file);
    bufferSave = buffer;

    //Grab one line at a time and parse into values.  Currently limited to FP64 types
    for (int i = 0; i < data.resultCount; i++)
    {
        data.resultList[i] = atof(strtok_r(bufferSave, ",", &bufferSave));
    }

    fclose(file);
    data.isMalformed = 0; //Sets the malform check to false as the operation is complete.

    return data;
}

int write_CNC(char testName[], double resultList[], uint32_t resultCount, uint32_t columnCount, char (*columnNames)[256])
{
    FILE *file;

    //Enforce maximum file name limit of 255 or lower in accordance with most restrictive OS limits
    if(strlen(testName) > 250)
        return -2;

    //Append .cnc to the testName input.  File type is ALWAYS .cnc
    char AppendedName[255];
    strcpy(AppendedName, testName);
    strcat_s(AppendedName, 255, ".cnc");

    if((file = fopen(AppendedName, "w")) == NULL)
        return -1;

    //Writes the file metadata to the first row of the csv
    fprintf(file, "%u,", VERSIONCODE);
    fprintf(file, "%u,", resultCount);
    fprintf(file, "%u,", columnCount);
    fprintf(file, "%s\n", testName);

    //Writes the column names to the second row of the csv
    for(int i = 0; i < columnCount; i++)
    {
        fprintf(file, "%s", columnNames[i]);
        if (i + 1 != columnCount)
            fprintf(file, ",");
    }
    fwrite("\n", sizeof(char), 1, file);

    //Writes the resultList in csv format with columnCount enforcing the frequency of line breaks
    uint16_t columnCursor = 0;
    for(int i = 0; i < resultCount; i++)
    {
        fprintf(file, "%lf", resultList[i]);
        if(columnCursor < columnCount) {
            columnCursor++;
            if (columnCursor == columnCount) {
                fprintf(file, "\n");
                columnCursor = 0;
            }
            else
                fprintf(file, ",");
        }
    }

    fclose(file);
    return 0;
}