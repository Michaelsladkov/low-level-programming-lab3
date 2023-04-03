#pragma once

#include "common_types.h"
#include "logical_structures.h"

void deleteString(const struct StorageController *const Controller, struct MyString String);
struct MyString createString(struct StorageController *const Controller,
                             const char *const String);