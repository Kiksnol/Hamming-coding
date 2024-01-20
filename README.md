# Hamming encoding/decoding

### Также доступно на русском: [Русский](README_ru.md)

## Theory
> ***[Arcticle on habr.com](https://habr.com/ru/articles/140611/)***  
> ***[Wikipedia article](https://en.wikipedia.org/wiki/Hamming_code)***
>
> I used Hamming coding "12 8": for every 8 bits 4 parity bits are created: 12 total

## Code implementation
Language - C++.  
Defects in implementation:
1. 12 encoded bits are writing in file as 2 bytes, not as 12 bits consistently, => it is possible to make encoded bits written down successively, but decoding will change and last byte may have only 4 valued bits
2. The data from the file is initially stored in a string, and then the string is encoded. This was done because the initial encoding was done and tested in another project using strings, and for convenience, I decided not to change it.
   If you change the encoding algorithm and remove the intermediary string between the encoded and decoded files, the algorithm will work faster.

## File Breakdown
- main.cpp - contains the command line argument parser and the main program function
- utilities.cpp - includes error handling functions, encoding/decoding functions
- utilities.hpp - contains function prototypes from the corresponding .cpp file + enum class for error types + necessary #include directives
- archive_mod.cpp - includes archive modification functions: creation, adding/removing files, decoding file/archive, merging two archives
- archive_mod.hpp - contains function prototypes from the corresponding .cpp file + enum class for the type of action currently being performed + necessary #include directives
