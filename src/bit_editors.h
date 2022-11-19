//
// Created by Тимофей Жамойдин on 23.10.22.
//
#pragma once

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "my_exceptions.h"
#include "constants.h"

class Reader {
public:
    void operator=(const Reader& other) = delete;
    Reader(const Reader& other) = delete;
    explicit Reader(std::istream& stream, std::string_view filename);
    std::string_view GetFilename();
    bool IsEof();
    bool GetBit();
    std::vector<bool> GetBits(int size);
    u_char GetByte();
    void Reset();

private:
    char buffer_[BUFFER_SIZE];
    size_t current_index_ = 0;
    size_t current_buffer_end_ = 0;
    size_t bites_ = 0;
    std::istream& stream_;
    std::string_view filename_;

    void UpdateBuffer();
};

class FileReader : public Reader {
public:
    explicit FileReader(std::string_view filename);
    ~FileReader();

private:
    std::ifstream file_;
};

class Writer {
public:
    void operator=(const Writer& other) = delete;
    Writer(const Writer& other) = delete;
    explicit Writer(std::ostream& stream);

    void WriteChar(char c);

    void WriteBits(const std::vector<bool>& bites);
    void WriteBit(bool b);

    void Flush();

private:
    char buffer_[BUFFER_SIZE];
    size_t current_index_ = 0;
    size_t bites_ = 0;
    std::ostream& stream_;
};

class FileWriter : public Writer {
public:
    explicit FileWriter(std::string_view filename);
    ~FileWriter();

private:
    std::ofstream file_;
};