# Кодирование Хэмминга без сжатия

## Теория
> ***[Статья на habr.com](https://habr.com/ru/articles/140611/)***
>
> Я использовал кодирование Хэмминга 12 8: на каждые 8 бит приходится по 4 контрольных бита, итого 12

## Реализация
Код реализован на языке C++.
Недочеты в реализации: 
1. Закодированные 12 бит заносятся в архив как два байта, а не последовательно, => можно сделать, чтобы все биты заносились подряд, но тогда изменится декодирование + возможно последний байт будет иметь только 4 значащих бита
2. Данные из файла изначально заносятся в строку, а дальше кодируется строка, так было сделано, поскольку изначальное кодирование я делал и проверял в другом проекте на строках, для удобства решил не менять.
   Если изменить алгоритм кодирования и убрать посредника-строку между кодируемым-декодируемым файлами, то алгоритм будет работать быстрее

## Разбиение на файлы
- main.cpp - содержит парсер строки аргументов и главную функцию программы
- utilities.cpp - содержит функции вывода ошибок, кодирования/декодирования
- utilities.hpp - содержит прототипы функций из одноименного .cpp файла + enum class типов ошибок + нужные #include
- archive_mod.cpp - содержит функции модификаций архива: создания, добавления/удаления файла, декодирования файла/архива, соединения двух архивов
- archive_mod.hpp - содержит прототипы функций из одноименного .cpp файла + enum class типа действий, которое совершается в данный момент + нужные #include

# English README

## Theory
> ***[Arcticle on habr.com](https://habr.com/ru/articles/140611/)***
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
