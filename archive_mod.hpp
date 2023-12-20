#pragma once

enum class Action {
    create_archive,
    list_files,
    extract_files,
    append_file_in_archive,
    delete_file_from_archive,
    merge_two_archives,
};

std::vector<std::string> listFilesInArchive(const std::string &archive_name);
int getAmountOfFiles(const std::string& archive_name);
void extractFileFromArchive(const std::string& archive_name, const std::string& out_filename);
void extractArchive(const std::string& archive_name);
void extractFilesFromArchive(const std::string& archive_name, const std::vector<std::string>& files_to_extract);