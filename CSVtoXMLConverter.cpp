#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "CSVtoXMLConverter.h"

int main() {
    FILE* file = fopen("trades.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Could not open file.\n");
        return 1;
    }
    ConvertFromCSVToXML(file);
    fclose(file);
    return 0;
}
