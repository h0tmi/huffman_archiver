//
// Created by Тимофей Жамойдин on 23.10.22.
//

#include "bit_view.h"

BitView::BitView(const size_t size) : data_(size) {
}
BitView::BitView() : data_(0) {
}
void BitView::ToSize(size_t new_size) {
    if (data_.size() < new_size) {
        data_.resize(new_size, false);
    }
}
size_t BitView::Size() {
    return data_.size();
}
void BitView::operator++() {
    bool add = true;
    for (size_t k = data_.size() - 1; add; --k) {
        if (!data_[k]) {
            data_[k] = true;
            add = false;
        } else {
            data_[k] = false;
        }
    }
}
std::vector<bool>& BitView::Data() {
    return data_;
}
