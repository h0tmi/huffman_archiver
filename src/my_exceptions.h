//
// Created by Тимофей Жамойдин on 23.10.22.
//
#pragma once

#include "exception"
#include "string"
#include "sstream"

class FilePathException : public std::exception {
private:
    std::string error_message_;

public:
    explicit FilePathException(std::string& filename) {
        error_message_ = "File " + filename + " not found";
    }
    const char* what() const throw() override {
        return error_message_.c_str();
    }
};

class BadArgumentException : public std::exception {
private:
    std::string error_message_;

public:
    explicit BadArgumentException(const std::string& message) {
        error_message_ = message;
    }
    const char* what() const throw() override {
        return error_message_.c_str();
    }
};

class DamagedDataException : public std::exception {
private:
    std::string error_message_;

public:
    explicit DamagedDataException(std::string& archive_name) {
        error_message_ = "Data in " + archive_name + "was damaged";
    }
    const char* what() const throw() override {
        return error_message_.c_str();
    }
};
