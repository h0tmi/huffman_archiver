//
// Created by Тимофей Жамойдин on 23.10.22.
//

#pragma once

#include "vector"


const size_t BUFFER_SIZE = 1024;
const char BITS_IN_BYTE = 8;

using HuffmanCode = std::vector<bool>;
using Char9Bit = uint16_t;

const Char9Bit FILENAME_END = 256;
const Char9Bit ONE_MORE_FILE = 257;
const Char9Bit ARCHIVE_END = 258;

const std::string HELP =
    "archiver -c archive_name file1 [file2 ...] - заархивировать файлы file1, file2, ... и сохранить результат в файл "
    "archive_name.\n"
    "\n"
    "archiver -d archive_name - разархивировать файлы из архива archive_name и положить в текущую директорию.\n"
    "\n"
    "archiver -h - вывести справку по использованию программы.\n";
const std::string LOW_ARGS = "Please enter at least 2 arguments";
const std::string ARGS_ERROR = "Please enter 1 argument";