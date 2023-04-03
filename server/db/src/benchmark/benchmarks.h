#pragma once

#include <stdio.h>

void benchmarkNodeInsert(FILE *OutFile);
void benchmarkSelectByAttributes(FILE *OutFile);
void benchmarkDeleteElements(FILE *OutFile);
void benchmarkUpdateProgressingElements(FILE *OutFile);
void benchmarkUpdateConstantElements(FILE *OutFile);
void benchmarkFileSize(FILE *OutFile);
