//
// Created by Тимофей Жамойдин on 23.10.22.
//

#pragma once

#include "vector"

class BitView {
public:
    explicit BitView(const size_t size);
    explicit BitView();

    void ToSize(size_t new_size);

    size_t Size();

    void operator++();
    std::vector<bool> &Data();

private:
    std::vector<bool> data_;
};