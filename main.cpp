#include <iostream>

#include "utilities.hpp"
#include "archive_mod.hpp"

#pragma warning(disable: 4244 4018)

const int data_bits_amount = 8;
const int total_bits_amount = 12;

int ParseCommand(int argc, char* argv[], Action* act) {
    std::string command = argv[1];

    if (command == "-c" || command == "--create") {
        if (argc < 3) {
            Error(ErrorType::kNotEnoughArguments);
            return EXIT_FAILURE;
        }

        *act = Action::create_archive;
    }
    else if (command == "-l" || command == "--list") {
        if (argc < 3) {
            Error(ErrorType::kNotEnoughArguments);
            return EXIT_FAILURE;
        }

        *act = Action::list_files;
    }
    else if (command == "-x" || command == "--extract") {
        if (argc < 3) {
            Error(ErrorType::kNotEnoughArguments);
            return EXIT_FAILURE;
        }

        *act = Action::extract_files;
    }
    else if (command == "-A" || command == "--concatenate") {
        if (argc < 4) {
            Error(ErrorType::kNotEnoughArguments);
            return EXIT_FAILURE;
        }

        *act = Action::merge_two_archives;
    } else if (command == "-d" || command == "--delete") {
        if (argc < 4) {
            Error(ErrorType::kNotEnoughArguments);
            return EXIT_FAILURE;
        }

        *act = Action::delete_file_from_archive;
    } else if (command == "-a" || command == "--append") {
        if (argc < 4) {
            Error(ErrorType::kNotEnoughArguments);
            return EXIT_FAILURE;
        }

        *act = Action::append_file_in_archive;
    } else {
        Error(ErrorType::kUnknownCommand);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void appendFilesToArchive(std::ofstream& archive, const std::string& file_to_append) {
    std::ifstream input(file_to_append, std::ios::binary);

    if (!input.is_open()) {
        Error(ErrorType::kInputFileNotOpened);
        return;
    }

    long long file_len = 0;
    while (input.get() != EOF)
        file_len++;

    input.clear();
    input.seekg(0, std::ios::beg);

    archive.seekp(0, std::ios_base::end);

    int fnl = static_cast<int>(file_to_append.size());
    
    archive.write((char*)&fnl, 4);
    archive.write(file_to_append.c_str(), fnl);
    
    std::string file_data;

    file_data.resize(file_len);
    input.read(&file_data[0], file_len);

    std::string encodedData = encodeString(file_data);
    file_len = encodedData.size();
    archive.write((char*)&file_len, sizeof(long long));
    archive.write(encodedData.c_str(), file_len);

    input.close();

    std::cout << "File " << file_to_append << " appended to archive successfully." << std::endl;
}

struct file_info {
    std::string filename;
    std::string data;
};

void readFilesInfo(const std::string& archive_name, const std::string& file_to_delete, std::vector<file_info> &files) {
    std::ifstream archive(archive_name, std::ios::binary);

    if (!archive.is_open()) {
        Error(ErrorType::kInputFileNotOpened);
        return;
    }
    
    long long file_size = 0;

    while (archive.get() != EOF) {
        file_size++;
    }

    archive.clear();
    archive.seekg(0, std::ios_base::beg);

    while (archive.tellg() < file_size) {
        int fn_size = 0;
        long long file_size = 0;
        std::string filename, file_data;

        archive.read((char*)&fn_size, 4);

        filename.resize(fn_size);

        archive.read((char*)&filename[0], fn_size);


        if (filename != file_to_delete) {
            archive.read((char*)&file_size, sizeof(long long));
            file_data.resize(file_size);
            archive.read((char*)&file_data[0], file_size);

            file_info fi;

            fi.data = std::move(file_data);
            fi.filename = std::move(filename);

            files.push_back(fi);
        } else {
            archive.read((char*)&file_size, sizeof(long long));
            file_data.resize(file_size);
            archive.read((char*)&file_data[0], file_size);
        }
    }

    archive.close();
}

void deleteFileFromArchive(const std::string& archive_name, const std::string& file_to_delete) {
    std::vector<file_info> files;

    readFilesInfo(archive_name, file_to_delete, files);

    std::ofstream archive(archive_name, std::ios::binary);

    if (!archive.is_open()) {
        Error(ErrorType::kInputFileNotOpened);
        return;
    }

    for (int i = 0; i < files.size(); ++i) {
        int fnl = static_cast<int>(files[i].filename.size());
        archive.write((char *)&fnl, 4);
        archive.write(files[i].filename.c_str(), fnl);

        long long file_len = files[i].data.size();

        archive.write((char *)&file_len, sizeof(long long));
        archive.write(files[i].data.c_str(), file_len);
    }

    archive.close();

    std::cout << "File " << file_to_delete << " deleted successfully.";
}

void mergeTwoArchives(const std::string& arch_name1, const std::string& arch_name2, const std::string& archive_name) {
    std::ifstream arch1(arch_name1, std::ios::binary);

    if (!arch1.is_open()) {
        Error(ErrorType::kInputFileNotOpened);
        return;
    }

    std::ifstream arch2(arch_name2, std::ios::binary);

    if (!arch2.is_open()) {
        Error(ErrorType::kInputFileNotOpened);
        return;
    }

    std::ofstream archive(archive_name, std::ios::binary);

    if (!archive.is_open()) {
        Error(ErrorType::kOutputFileNotOpened);
        return;
    }

    char buf;
    while(arch1.get(buf)) {
        archive.put(buf);
    }

    buf = EOF;
    while(arch2.get(buf)) {
        archive.put(buf);
    }

    std::cout << "Merged successfully" << std::endl;
    
    arch1.close();
    arch2.close();
    archive.close();
}

int main( int argc, char* argv[]) {
    std::string archive_name;
    Action act;
    std::vector<std::string> files;

    if (argc < 2) {
        Error(ErrorType::kNoArguments);
        return EXIT_FAILURE;
    }

    if (ParseCommand(argc, argv, &act)) {
        return EXIT_FAILURE;
    }

    switch(act) {
        case Action::create_archive: {
            std::string file_arg;

            file_arg = argv[2];
            int i = 3;

            if (file_arg == "-f") {
                archive_name = argv[3];
                i = 4;
            } else if (file_arg[0] == '-' && file_arg[1] == '-' && file_arg[2] == 'f') {
                archive_name = SplitWithSign(file_arg, '=');
            } else {
                Error(ErrorType::kNoArchiveName);
                return EXIT_FAILURE;
            }

            archive_name += ".haf";

            files.resize(argc - i);
            int c = i;
            for (; i < argc; ++i) {
                files[i - c] = argv[i];
            }

            std::ofstream archive(archive_name, std::ios::binary);

            if (!archive.is_open()) {
                Error(ErrorType::kOutputFileNotOpened);
                return EXIT_FAILURE;
            }

            std::cout << "Files to open total: " << files.size() << std::endl;
            for (int j = 0; j < files.size(); ++j) {
                std::cout << "File to archive now: " << files[j] << std::endl;
                encodeFile(files[j], archive_name);
            }

            archive.close();

            std::cout << "\nArchive created successfully.\n";

            break;
        }
        case Action::list_files: {
            std::string file_arg;

            file_arg = argv[2];

            if (file_arg == "-f") {
                archive_name = argv[3];
            } else if (file_arg[0] == '-' && file_arg[1] == '-' && file_arg[2] == 'f') {
                archive_name = SplitWithSign(file_arg, '=');
            } else {
                Error(ErrorType::kNoArchiveName);
                return EXIT_FAILURE;
            }

            archive_name += ".haf";

            files = listFilesInArchive(archive_name);
            
            if (files.size() > 0) {
                std::cout << "Files in archive " << archive_name << " :\n";

                for (int i = 0; i < files.size(); ++i) {
                    std::cout << i + 1 << ". " << files[i] << std::endl;
                }
            } else {
                std::cout << "No files in achive " << archive_name << std::endl;    
            }

            break;
        }
        case Action::extract_files: {
            std::string file_arg;

            file_arg = argv[2];

            int i = 3;

            if (file_arg == "-f") {
                archive_name = argv[3];
                i = 4;
            } else if (file_arg[0] == '-' && file_arg[1] == '-' && file_arg[2] == 'f') {
                archive_name = SplitWithSign(file_arg, '=');
            } else {
                Error(ErrorType::kNoArchiveName);
                return EXIT_FAILURE;
            }

            archive_name += ".haf";

            files.resize(argc - i);
            int c = i;
            for (; i < argc; ++i) {
                files[i - c] = argv[i];
            }

            std::ifstream archive(archive_name, std::ios::binary);

            if (!archive.is_open()) {
                Error(ErrorType::kInputFileNotOpened);
                return EXIT_FAILURE;
            }

            extractFilesFromArchive(archive_name, files);

            break;
        }
        case Action::append_file_in_archive: {
            std::string file_arg;

            file_arg = argv[2];

            int i = 3;

            if (file_arg == "-f") {
                archive_name = argv[3];
                i = 4;
            } else if (file_arg[0] == '-' && file_arg[1] == '-' && file_arg[2] == 'f') {
                archive_name = SplitWithSign(file_arg, '=');
            } else {
                Error(ErrorType::kNoArchiveName);
                return EXIT_FAILURE;
            }

            archive_name += ".haf";

            std::ofstream archive(archive_name, std::ios::binary | std::ios::app);

            if (!archive.is_open()) {
                Error(ErrorType::kOutputFileNotOpened);
                return EXIT_FAILURE;
            }

            if (i >= argc) {
                Error(ErrorType::kNotEnoughArguments);
                return EXIT_FAILURE;
            }

            appendFilesToArchive(archive, argv[i]);

            break;
        }
        case Action::delete_file_from_archive: {
            std::string file_arg;

            file_arg = argv[2];
            int i = 3;

            if (file_arg == "-f") {
                archive_name = argv[3];
                i = 4;
            } else if (file_arg[0] == '-' && file_arg[1] == '-' && file_arg[2] == 'f') {
                archive_name = SplitWithSign(file_arg, '=');
            } else {
                Error(ErrorType::kNoArchiveName);
                return EXIT_FAILURE;
            }

            archive_name += ".haf";

            deleteFileFromArchive(archive_name, argv[i]);

            break;
        }
        case Action::merge_two_archives: {
            std::string arch_name1 = argv[2], arch_name2 = argv[3];
            std::string file_arg = argv[4];

            arch_name1 += ".haf";
            arch_name2 += ".haf";

            if (file_arg == "-f") {
                archive_name = argv[3];
            } else if (file_arg[0] == '-' && file_arg[1] == '-' && file_arg[2] == 'f') {
                archive_name = SplitWithSign(file_arg, '=');
            } else {
                Error(ErrorType::kNoArchiveName);
                return EXIT_FAILURE;
            }

            archive_name += ".haf";
            
            mergeTwoArchives(arch_name1, arch_name2, archive_name);

            break;
        }
    }


    return EXIT_SUCCESS;
}