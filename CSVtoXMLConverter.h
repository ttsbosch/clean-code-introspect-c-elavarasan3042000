#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "TradeRecord.h"
#include "StringConverters.h"

void WriteXML(TradeRecords *records, int recordCount);
void ParseCSVLine(char* line, char* fields[], int* fieldCount) {
    *fieldCount = 0;
    char* token = strtok(line, ",");
    while (token != NULL && *fieldCount < 3) {
        fields[*fieldCount] = token;
        (*fieldCount)++;
        token = strtok(NULL, ",");
    }
}
bool ValidateTradeCurrencies(const char* currency, int lineCount) {
    if (strlen(currency) != 6) {
        fprintf(stderr, "WARN: Trade currencies on line %d malformed: '%s'\n", lineCount + 1, currency);
        return false;
    }
    return true;
}

bool ValidateTradeAmount(const char* amountStr, int lineCount, int& amount) {
    if (!TryConverttoInt(amountStr, &amount)) {
        fprintf(stderr, "WARN: Trade amount on line %d not a valid integer: '%s'\n", lineCount + 1, amountStr);
        return false;
    }
    return true;
}

bool ValidateTradePrice(const char* priceStr, int lineCount, double& price) {
    if (!TryConverttoDouble(priceStr, &price)) {
        fprintf(stderr, "WARN: Trade price on line %d not a valid decimal: '%s'\n", lineCount + 1, priceStr);
        return false;
    }
    return true;
}

bool ValidateTradeInfo(char* fields[], int lineCount) {
    // Validate trade currencies
    if (!ValidateTradeCurrencies(fields[0], lineCount)) {
        return false;
    }

    // Validate trade amount
    int trade_amount;
    if (!ValidateTradeAmount(fields[1], lineCount, trade_amount)) {
        return false;
    }

    // Validate trade price
    double trade_price;
    if (!ValidateTradePrice(fields[2], lineCount, trade_price)) {
        return false;
    }

    return true;
}
void ProcessValidRecord(TradeRecords Records[], int* recordCount, char* fields[], int LotSize) {
    strncpy(Records[*recordCount].SourceCurrency, fields[0], 3);
    strncpy(Records[*recordCount].DestinationCurrency, fields[0] + 3, 3);
    int trade_amount;
    TryConverttoInt(fields[1], &trade_amount);  // Assuming TryConverttoInt already validated
    Records[*recordCount].Lots = (int)(trade_amount / LotSize);
    TryConverttoDouble(fields[2], &Records[*recordCount].Price);  // Assuming TryConverttoDouble already validated
    (*recordCount)++;
}


void WriteXML(TradeRecords *records, int recordCount) {
    FILE* outFile = fopen("output.xml", "w");
    fprintf(outFile, "<TradeRecords>\n");
    for (int i = 0; i < recordCount; i++) {
        fprintf(outFile, "\t<TradeRecord>\n");
        fprintf(outFile, "\t\t<SourceCurrency>%s</SourceCurrency>\n", records[i].SourceCurrency);
        fprintf(outFile, "\t\t<DestinationCurrency>%s</DestinationCurrency>\n", records[i].DestinationCurrency);
        fprintf(outFile, "\t\t<Lots>%f</Lots>\n", records[i].Lots);
        fprintf(outFile, "\t\t<Price>%f</Price>\n", records[i].Price);
        fprintf(outFile, "\t</TradeRecord>\n");
    }
    fprintf(outFile, "</TradeRecords>");
    fclose(outFile);
}
int ReadFromCSV(FILE* stream)
{
    char line[1024];
    TradeRecords Records[1024];
    int lineCount = 0;
    int recordCount = 0;
    int LotSize = 100;

    while (fgets(line, sizeof(line), stream)) {
        char* fields[3];
        int fieldCount = 0;

        // Parse CSV line into fields
        ParseCSVLine(line, fields, &fieldCount);

        // Validate trade information
        if (fieldCount != 3 || !ValidateTradeInfo(fields, lineCount)) {
            continue;
        }

        // Process valid record
        ProcessValidRecord(Records, &recordCount, fields, LotSize);

        lineCount++;
    }
    return recordCount
}
void ConvertFromCSVToXML(FILE* stream) {
    int recordCount = 0;
    recordCount = ReadFromCSV(stream)
    // Write records to XML
    WriteXML(Records, recordCount);

    // Print information about processed trades
    printf("INFO: %d trades processed\n", recordCount);
}
