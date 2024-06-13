#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <Converters.h>
#include <StringProcessor.h>

void ConvertFromCSVToXML(FILE* stream) {
    char line[1024];
    TradeRecords Records[1024];
    int lineCount = 0;
    int objectCount = 0;
    int LotSize = 10;

    while (fgets(line, sizeof(line), stream)) {
        char* fields[3];
        int fieldCount = 0;
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

        strncpy(Records[objectCount].SourceCurrency, fields[0], 3);
        strncpy(Records[objectCount].DestinationCurrency, fields[0] + 3, 3);
        Records[objectCount].Lots = (int)(trade_amount / LotSize);
        Records[objectCount].Price = trade_price;
        objectCount++;
        lineCount++;
    }

    FILE* outFile = fopen("output.xml", "w");
    fprintf(outFile, "<TradeRecords>\n");
    for (int object_index = 0; object_index < objectCount; object_index++) {
        fprintf(outFile, "\t<TradeRecord>\n");
        fprintf(outFile, "\t\t<SourceCurrency>%s</SourceCurrency>\n", Records[object_index].SourceCurrency);
        fprintf(outFile, "\t\t<DestinationCurrency>%s</DestinationCurrency>\n", Records[object_index].DestinationCurrency);
        fprintf(outFile, "\t\t<Lots>%d</Lots>\n", Records[object_index].Lots);
        fprintf(outFile, "\t\t<Price>%f</Price>\n", Records[object_index].Price);
        fprintf(outFile, "\t</TradeRecord>\n");
    }
    fprintf(outFile, "</TradeRecords>");
    fclose(outFile);
    printf("INFO: %d trades processed\n", objectCount);
}
