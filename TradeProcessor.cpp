#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    char SourceCurrency[256]; 
    char DestibationCurrency[256];
    float Lots;
    double Price;
} TradeRecords;


char** SplitString(const char* input_string, char delimiter) {
    int count = 0;
    const char* ptr = input_string;
    while (*ptr != '\0') {
        if (*ptr++ == delimiter) {
            count++;
        }
    }

    char** tokens = (char**)malloc(sizeof(char*) * (count + 2));
    int i = 0;
    ptr = input_string;
    char* token = (char*)malloc(strlen(input_string) + 1);
    int j = 0;
    while (*ptr != '\0') {
        if (*ptr == delimiter) {
            token[j] = '\0';
            tokens[i++] = strdup(token);
            j = 0;
        } else {
            token[j++] = *ptr;
        }
        ptr++;
    }
    token[j] = '\0';
    tokens[i++] = strdup(token);
    tokens[i] = NULL;
    free(token);
    return tokens;
}


int TryConverttoInt(const char* input_string, int* value) {
    char* endptr;
    *value = strtol(input_string, &endptr, 10);
    if (endptr == input_string) {
        return 0;
    }
    return 1;
}

int TryConverttoDouble(const char* input_string, double* value) {
    char* endptr;
    *value = strtod(input_string, &endptr);
    if (endptr == input_string) {
        return 0;
    }
    return 1;
}

void ConvertFromCSVToXML(FILE* stream) {
    char line[1024];
    TradeRecords Records[1024];
    int lineCount = 0;
    int objectCount = 0;

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
        }

        double trade_price;
        if (!TryConverttoDouble(fields[2], &trade_price)) {
            fprintf(stderr, "WARN: Trade price on line %d not a valid decimal: '%s'\n", lineCount + 1, fields[2]);
        }

        strncpy(Records[objectCount].SourceCurrency, fields[0], 3);
        strncpy(Records[objectCount].DestibationCurrency, fields[0] + 3, 3);
        Records[objectCount].Lots = trade_amount / LotSize;
        Records[objectCount].Price = trade_price;
        objectCount++;
        lineCount++;
    }

    FILE* outFile = fopen("output.xml", "w");
    fprintf(outFile, "<TradeRecords>\n");
    for (int i = 0; i < objectCount; i++) {
        fprintf(outFile, "\t<TradeRecord>\n");
        fprintf(outFile, "\t\t<SourceCurrency>%s</SourceCurrency>\n", Records[i].SourceCurrency);
        fprintf(outFile, "\t\t<DestinationCurrency>%s</DestinationCurrency>\n", Records[i].DestibationCurrency);
        fprintf(outFile, "\t\t<Lots>%d</Lots>\n", Records[i].Lots);
        fprintf(outFile, "\t\t<Price>%f</Price>\n", Records[i].Price);
        fprintf(outFile, "\t</TradeRecord>\n");
    }
    fprintf(outFile, "</TradeRecords>");
    fclose(outFile);
    printf("INFO: %d trades processed\n", objectCount);
}
