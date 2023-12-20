#include <iostream>

#include "utilities.hpp"

void Error(const ErrorType& Err) {
    switch (Err) {
    case ErrorType::kInputFileNotOpened:
        std::cerr << "Error: cannot open input file." << std::endl;
        return;
    case ErrorType::kOutputFileNotOpened:
        std::cerr << "Error: cannot open output file." << std::endl;
        return;
    case ErrorType::kNoArguments:
        std::cerr << "Error: no arguments." << std::endl;
        return;
    case ErrorType::kNotEnoughArguments:
        std::cerr << "Error: not enough arguments." << std::endl;
        return;
    case ErrorType::kBadArguments:
        std::cerr << "Error: bad arguments." << std::endl;
        return;
    case ErrorType::kUnknownCommand:
        std::cerr << "Error: unknown command." << std::endl;
        return;
    case ErrorType::kNoArchiveName:
        std::cerr << "Error: no archive name" << std::endl;
        return;
    case ErrorType::kNotFoundFileToExtract:
        std::cerr << "Error: not found file in archive" << std::endl;
        return;
    case ErrorType::kNoFilesToExtract:
        std::cerr << "Error: no files to extract" << std::endl;
        return;
    case ErrorType::kFileToDeleteIsNotInArchive:
        std::cerr << "Error: could not find file to delete from archive" << std::endl;
        return;
    default:
        std::cerr << "Error: unknown error." << std::endl;
        return;
    }
}

std::string encodeString(const std::string& inputData) {
    std::string encodedString;

    for (char ch : inputData) {
        std::bitset<8> data_bits(ch);

        int parity_bit_count = 4;

        std::bitset<12> encoded_data;

        encoded_data[2] = data_bits[0];
        encoded_data[4] = data_bits[1];
        encoded_data[5] = data_bits[2];
        encoded_data[6] = data_bits[3];
        encoded_data[8] = data_bits[4];
        encoded_data[9] = data_bits[5];
        encoded_data[10] = data_bits[6];
        encoded_data[11] = data_bits[7];

        encoded_data[0] = encoded_data[2] ^ encoded_data[4] ^ encoded_data[6] ^ encoded_data[8] ^ encoded_data[10];
        encoded_data[1] = encoded_data[2] ^ encoded_data[5] ^ encoded_data[6] ^ encoded_data[9] ^ encoded_data[10];
        encoded_data[3] = encoded_data[4] ^ encoded_data[5] ^ encoded_data[6];
        encoded_data[7] = encoded_data[8] ^ encoded_data[9] ^ encoded_data[10];

        std::bitset<8> pt1, pt2;

        for (int i = 0; i < 8; ++i) {
            pt1[i] = encoded_data[i];
        }

        for (int i = 0; i < 4; ++i) {
            pt2[i] = encoded_data[i + 8];
        }

        std::string a1, a2;
        a1.push_back(static_cast<char>(pt1.to_ulong()));
        a2.push_back(static_cast<char>(pt2.to_ulong()));
        encodedString += a1 + a2;
    }

    return encodedString;
}

void encodeFile(const std::string& input_file, const std::string& output_file) {
    std::ifstream input(input_file, std::ios::binary);
    std::ofstream output(output_file, std::ios::binary);

    if (!input.is_open()) {
        Error(ErrorType::kInputFileNotOpened);
        return;
    }

    if (!output.is_open()) {
        Error(ErrorType::kOutputFileNotOpened);
        return;
    }

    long long file_len = 0;
    while (input.get() != EOF) 
        file_len++;

    input.clear();
    input.seekg(0, std::ios::beg);

    std::string file_data;

    int fnl = static_cast<int>(input_file.size());
    output.write((char *)&fnl, 4);
    output.write(input_file.c_str(), fnl);

    file_data.resize(file_len);
    input.read(&file_data[0], file_len);

    std::string encodedData = encodeString(file_data);
    file_len = encodedData.size();
    output.write((char *)&file_len, sizeof(long long));
    output.write(encodedData.c_str(), file_len);
    
    input.close();
    output.close();
}


std::string decodeString(const std::string& encodedData) {
    std::string decodedString;

    for (int i = 0; i < encodedData.size(); i++) {
        std::bitset<8> decoded_data;
        char c1, c2;

        c1 = encodedData[i++];
        c2 = encodedData[i];

        std::bitset<8> b1(c1), b2(c2);
        std::bitset<12> enc;

        for (int i = 0; i < 8; i++) {
            enc[i] = b1[i];
        }

        for (int i = 0; i < 4; i++) {
            enc[i + 8] = b2[i];
        }

        int p1 = enc[0] ^ enc[2] ^ enc[4] ^ enc[6] ^ enc[8] ^ enc[10];
        int p2 = enc[1] ^ enc[2] ^ enc[5] ^ enc[6] ^ enc[9] ^ enc[10];
        int p3 = enc[3] ^ enc[4] ^ enc[5] ^ enc[6];
        int p4 = enc[7] ^ enc[8] ^ enc[9] ^ enc[10];

        int errorPosition = p1 + 2 * p2 + 4 * p3 + 8 * p4;

        if (errorPosition != 0) {
            enc.flip(errorPosition - 1);
        }

        decoded_data[0] = enc[2];
        decoded_data[1] = enc[4];
        decoded_data[2] = enc[5];
        decoded_data[3] = enc[6];
        decoded_data[4] = enc[8];
        decoded_data[5] = enc[9];
        decoded_data[6] = enc[10];
        decoded_data[7] = enc[11];

        decodedString += static_cast<char>(decoded_data.to_ulong());
    }

    return decodedString;
}

std::string SplitWithSign(const std::string& s, char sign) {
    std::string res;
    bool right = false;

    for (int i = 0; i < s.size(); i++) {
        if (!right) {
            if (s[i] == sign) {
                right = true;
            }
        }
        else {
            res += s[i];
        }
    }

    return res;
}

std::string ulongToString(const unsigned long val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}