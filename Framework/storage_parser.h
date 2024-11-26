#ifndef STORAGE_PARSER_H
#define STORAGE_PARSER_H
#include <stdio.h>

extern void set_yyin(FILE *fd);

#define MAX_STR_LEN 256

typedef struct {
  int version;
  int result_count;
  int columnCount;
  char test_name[MAX_STR_LEN];
} CsvHeader;

typedef struct {
  CsvHeader header;
  char *column_labels[100];
  int label_count;
  float records[100][100];
  int record_count;
  int field_count[100];
} CsvData;

extern CsvData csv_data;
extern int yyparse(void);
#endif