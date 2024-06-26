#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "TradeRecord.h"
#include "StringConverters.h"

void WriteXML(TradeRecords *records, int recordCount);
void ConvertFromCSVToXML(FILE* stream) {
    char line[1024];
    TradeRecords Records[1024];
    int lineCount = 0;
    int recordCount = 0;
    int LotSize = 100;

    while (fgets(line, sizeof(line), stream)) {
        char* fields[3];
        int fieldCount = 0;
        const char* input_string=line;
        char* token = strtok(line, ",");
        while (token != NULL) {

            fields[fieldCount++] = token;
            token = strtok(NULL, ",");
        }
        if (fieldCount != 3) {
            fprintf(stderr, "WARN: Line %d malformed. Only %d field(s) found.\n", lineCount + 1, fieldCount);
            continue;
        }

        if (strlen(fields[0]) != 6) {
            fprintf(stderr, "WARN: Trade currencies on line %d malformed: '%s'\n", lineCount + 1, fields[0]);
            continue;
        }

        int trade_amount;
        if (!TryConverttoInt(fields[1], &trade_amount)) {
            fprintf(stderr, "WARN: Trade amount on line %d not a valid integer: '%s'\n", lineCount + 1, fields[1]);
            continue;
        }

        double trade_price;
        if (!TryConverttoDouble(fields[2], &trade_price)) {
            fprintf(stderr, "WARN: Trade price on line %d not a valid decimal: '%s'\n", lineCount + 1, fields[2]);
            continue;
        }

        strncpy(Records[recordCount].SourceCurrency, fields[0], 3);
        strncpy(Records[recordCount].DestinationCurrency, fields[0] + 3, 3);
        Records[recordCount].Lots = (int)(trade_amount / LotSize);
        Records[recordCount].Price = trade_price;
        recordCount++;
        lineCount++;
    }
    WriteXML(Records, recordCount);
    printf("INFO: %d trades processed\n", recordCount);
}
void WriteXML(TradeRecords *records, int recordCount) {
    FILE* outFile = fopen("output.xml", "w");
    fprintf(outFile, "<TradeRecords>\n");
    for (int i = 0; i < recordCount; i++) {
        fprintf(outFile, "\t<TradeRecord>\n");
        fprintf(outFile, "\t\t<SourceCurrency>%s</SourceCurrency>\n", records[i].SourceCurrency);
        fprintf(outFile, "\t\t<DestinationCurrency>%s</DestinationCurrency>\n", records[i].DestinationCurrency);
        fprintf(outFile, "\t\t<Lots>%d</Lots>\n", records[i].Lots);
        fprintf(outFile, "\t\t<Price>%f</Price>\n", records[i].Price);
        fprintf(outFile, "\t</TradeRecord>\n");
    }
    fprintf(outFile, "</TradeRecords>");
    fclose(outFile);
}
