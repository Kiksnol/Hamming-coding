#include <iostream>
#include <bitset>
#include <cstdlib>

#include "utilities.hpp"
#include "archive_mod.hpp"

std::vector<std::string> listFilesInArchive(const std::string &archive_name) {
    std::ifstream archive(archive_name, std::ios::binary);

    std::vector<std::string> fileList;

    long long len = 0;

    while (archive.get() != EOF)
        len++;

    archive.clear();
    archive.seekg(0, std::ios::beg);

    while (archive.tellg() < len) {
        int fn_size = 0;
        long long file_size = 0;
        std::string filename, file_data;

        archive.read((char*)&fn_size, 4);

        filename.resize(fn_size);

        archive.read((char*)&filename[0], fn_size);
        archive.read((char*)&file_size, sizeof(long long));
        file_data.resize(file_size);
        archive.read((char*)&file_data[0], file_size);
        fileList.push_back(filename);
    }

    return fileList;
}

int getAmountOfFiles(const std::string& archive_name) {
    std::vector<std::string> files = listFilesInArchive(archive_name);

    return static_cast<int>(files.size());
}

void extractArchive(const std::string& archive_name) {
    std::ifstream archive(archive_name, std::ios::binary);
    int amount_of_files = getAmountOfFiles(archive_name);

    for (int i = 0; i < amount_of_files; ++i) {
        int fn_size = 0;
        long long file_size = 0;
        std::string filename, file_data;

        archive.read((char*)&fn_size, 4);

        filename.resize(fn_size);

        archive.read((char*)&filename[0], fn_size);
        archive.read((char*)&file_size, sizeof(long long));
        file_data.resize(file_size);
        archive.read((char*)&file_data[0], file_size);
        std::string decoded_file_data = decodeString(file_data);

        std::ofstream output(filename, std::ios::binary);

        if (!output.is_open()) {
            Error(ErrorType::kOutputFileNotOpened);
            continue;
        }

        file_size = decoded_file_data.size();

        output.write(decoded_file_data.c_str(), file_size);
        output.close();

        std::cout << "File extracted: " << filename << std::endl;
    }
}

void extractFileFromArchive(const std::string& archive_name, const std::string& out_filename) {
    std::ifstream archive(archive_name, std::ios::binary);
    int amount_of_files = getAmountOfFiles(archive_name);

    for (int i = 0; i < amount_of_files; ++i) {
        int file_size = 0, fn_size = 0;
        std::string filename, file_data;

        archive.read((char*)&fn_size, 4);

        filename.resize(fn_size);

        archive.read((char*)&filename[0], fn_size);
        archive.read((char*)&file_size, sizeof(long long));
        file_data.resize(file_size);
        archive.read((char*)&file_data[0], file_size);
        if (filename == out_filename) {
            std::string decoded_file_data = decodeString(file_data);

            std::ofstream output(filename, std::ios::binary);

            if (!output.is_open()) {
                Error(ErrorType::kOutputFileNotOpened);
                continue;
            }

            output.write(decoded_file_data.c_str(), file_size);
            output.close();

            std::cout << "File extracted: " << filename << std::endl;
        }
    }
}

void extractFilesFromArchive(const std::string& archive_name, const std::vector<std::string>& files_to_extract) {
    std::ifstream archive(archive_name, std::ios::binary);
    std::vector<std::string> file_list = listFilesInArchive(archive_name);
    
    if (files_to_extract.empty()) {
        extractArchive(archive_name);
    }
    else {
        for (const std::string& file_to_extract : files_to_extract) {
            std::cout << "Extracting file: " << file_to_extract << std::endl;

            if (std::find(file_list.begin(), file_list.end(), file_to_extract) != file_list.end()) {
                extractFileFromArchive(archive_name, file_to_extract);
                std::cout << "File extracted: " << file_to_extract << std::endl;
            }
            else {
                Error(ErrorType::kNotFoundFileToExtract);
            }
        }
    }
}