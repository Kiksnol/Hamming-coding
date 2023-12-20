#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>

enum class ErrorType {
    kInputFileNotOpened,
    kOutputFileNotOpened,
    kNoArguments,
    kNotEnoughArguments,
    kBadArguments,
    kUnknownCommand,
    kNoArchiveName,
    kNotFoundFileToExtract,
    kNoFilesToExtract,
    kFileToDeleteIsNotInArchive,
};

void Error(const ErrorType& Err);
std::string encodeString(const std::string& inputData);
void encodeFile(const std::string& input_file, const std::string& output_file);
std::string decodeString(const std::string& encodedData);
std::string SplitWithSign(const std::string& s, char sign);
std::string ulongToString(const unsigned long val);