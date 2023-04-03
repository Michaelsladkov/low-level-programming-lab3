#include <stdio.h>

#include "benchmarks.h"

int main() {
    const char *InsertBenchmarkResultName = "InsertTime.csv";
    const char *SelectBenchmarkResultName = "SelectByAttrsTime.csv";
    const char *DeleteBenchmarkResultName = "DeleteConstantElementsTime.csv";
    const char *UpdateProgressingBenchmarkResultName = "UpdateProgressingElementsTime.csv";
    const char *UpdateConstantBenchmarkResultName = "UpdateConstantElementsTime.csv";
    const char *FileSizeBenchmarkResultName = "FileSizeByNodes.csv";

    FILE *Result;

    Result = fopen(FileSizeBenchmarkResultName, "w");
    benchmarkFileSize(Result);
    fclose(Result);

    Result = fopen(InsertBenchmarkResultName, "w");
    benchmarkNodeInsert(Result);
    fclose(Result);

    Result = fopen(SelectBenchmarkResultName, "w");
    benchmarkSelectByAttributes(Result);
    fclose(Result);

    Result = fopen(DeleteBenchmarkResultName, "w");
    benchmarkDeleteElements(Result);
    fclose(Result);

    Result = fopen(UpdateProgressingBenchmarkResultName, "w");
    benchmarkUpdateProgressingElements(Result);
    fclose(Result);

    Result = fopen(UpdateConstantBenchmarkResultName, "w");
    benchmarkUpdateConstantElements(Result);
    fclose(Result);
}