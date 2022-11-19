//
// Created by Тимофей Жамойдин on 23.10.22.
//
#include "bit_editors.h"
#include <iostream>
#include "constants.h"

bool Reader::GetBit() {
    if (current_index_ >= current_buffer_end_) {
        UpdateBuffer();
    }
    bool result = (buffer_[current_index_] >> (BITS_IN_BYTE - 1 - bites_++)) & 1;
    if (bites_ >= BITS_IN_BYTE) {
        ++current_index_;
        bites_ = 0;
    }
    return result;
}
u_char Reader::GetByte() {
    u_char byte = 0;
    for (size_t i = 0; i < BITS_IN_BYTE; ++i) {
        byte <<= 1;
        bool b = GetBit();
        if (b) {
            byte |= 1;
        }
    }
    return byte;
}
bool Reader::IsEof() {
    if (current_index_ < current_buffer_end_) {
        return false;
    }
    UpdateBuffer();
    return current_buffer_end_ == 0 && stream_.eof();
}
std::string_view Reader::GetFilename() {
    auto slash = filename_.find('/');
    if (slash == filename_.npos) {
        slash = 0;
    } else {
        ++slash;
    }
    return filename_.substr(slash);
}
std::vector<bool> Reader::GetBits(int size) {
    std::vector<bool> result(size);
    for (size_t i = 0; i < size; ++i) {
        result[i] = GetBit();
    }
    return result;
}
void Reader::UpdateBuffer() {
    stream_.read(buffer_, BUFFER_SIZE);
    current_buffer_end_ = stream_.gcount();
    current_index_ = 0;
}
void Reader::Reset() {
    stream_.clear();
    stream_.seekg(0);
    current_buffer_end_ = 0;
    current_index_ = 0;
}
Reader::Reader(std::istream &stream, std::string_view filename) : stream_(stream), filename_(filename) {
    std::fill(buffer_, buffer_ + BUFFER_SIZE, 0);
}
void Writer::WriteBit(bool b) {
    buffer_[current_index_] <<= 1;
    if (b) {
        buffer_[current_index_] |= 1;
    }
    bites_++;
    if (bites_ >= BITS_IN_BYTE) {
        ++current_index_;
        bites_ = 0;
    }
    if (current_index_ >= BUFFER_SIZE) {
        stream_.write(buffer_, BUFFER_SIZE);
        current_index_ = 0;
    }
}
void Writer::WriteChar(char c) {
    for (size_t i = 0; i < BITS_IN_BYTE; ++i) {
        WriteBit((c >> (BITS_IN_BYTE - i - 1)) & 1);
    }
}
void Writer::WriteBits(const std::vector<bool> &bites) {
    for (const auto &bite : bites) {
        WriteBit(bite);
    }
}
void Writer::Flush() {
    if (current_index_ != 0 || bites_ != 0) {
        if (bites_ != 0) {
            buffer_[current_index_] = static_cast<char>(buffer_[current_index_] << (BITS_IN_BYTE - bites_));
            ++current_index_;
        }
        stream_.write(buffer_, static_cast<std::streamsize>(current_index_));
        current_index_ = 0;
        bites_ = 0;
    }
}
Writer::Writer(std::ostream &stream) : stream_(stream) {
    std::fill(buffer_, buffer_ + BUFFER_SIZE, 0);
}

FileReader::~FileReader() {
    file_.close();
}
FileReader::FileReader(std::string_view filename)
    : Reader(file_, filename), file_(std::string(filename), std::ios::binary) {
    if (!file_.is_open()) {
        std::string wrong_file = std::string(filename);
        throw FilePathException(wrong_file);
    }
}
FileWriter::~FileWriter() {
    file_.close();
}
FileWriter::FileWriter(std::string_view filename)
    : Writer(file_), file_(std::string(filename), std::ios::binary) {
    if (!file_.is_open()) {
        std::string wrong_file = std::string(filename);
        throw FilePathException(wrong_file);
    }
}
