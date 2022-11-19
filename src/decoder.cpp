//
// Created by Тимофей Жамойдин on 23.10.22.
//

#include "decoder.h"
#include "bit_view.h"

Char9Bit ConvertToChar9Bit(const std::vector<bool> &b) {
    Char9Bit converted = 0;
    for (size_t i = 0; i < b.size(); ++i) {
        converted += (b[i] << (b.size() - 1 - i));
    }
    return converted;
}

std::unordered_map<HuffmanCode, Char9Bit> GetCanonicalCodes(Reader &input_stream) {
    Char9Bit alpha_size = ConvertToChar9Bit(input_stream.GetBits(9));
    std::vector<Char9Bit> alphabet(alpha_size);
    for (size_t i = 0; i < alpha_size; ++i) {
        alphabet[i] = ConvertToChar9Bit(input_stream.GetBits(9));
    }
    std::vector<Char9Bit> length_cnt = {0};
    size_t sum_len = 0;
    for (size_t i = 0; sum_len < alpha_size; ++i) {
        length_cnt.push_back(ConvertToChar9Bit(input_stream.GetBits(9)));
        sum_len += length_cnt[i + 1];
    }
    BitView canonical;
    std::unordered_map<HuffmanCode, Char9Bit> result;
    for (size_t i = 0; i < alphabet.size(); ++i) {
        if (i != 0) {
            ++canonical;
        }
        auto code = alphabet[i];
        size_t new_size = canonical.Size();
        while (length_cnt[new_size] == 0) {
            ++new_size;
        }
        --length_cnt[new_size];
        canonical.ToSize(new_size);
        result[canonical.Data()] = code;
    }
    return result;
}

void Decode(const std::string_view archive_name) {
    FileReader input(archive_name);
    bool archive_end = false;
    while (!archive_end) {
        auto canonical_forms = GetCanonicalCodes(input);
        bool filename_end = false;
        HuffmanCode current_symbol;
        std::string filename;
        while (!filename_end) {
            if (canonical_forms.count(current_symbol)) {
                Char9Bit sym = canonical_forms[current_symbol];
                if (sym == FILENAME_END) {
                    filename_end = true;
                } else {
                    filename.push_back(static_cast<char>(sym));
                }
                current_symbol.clear();
            } else {
                if (input.IsEof()) {
                    std::string damaged_archive = std::string(archive_name);
                    throw DamagedDataException(damaged_archive);
                }
                current_symbol.push_back(input.GetBit());
            }
        }
        FileWriter output(filename);
        bool file_end = false;
        while (!file_end) {
            if (canonical_forms.count(current_symbol)) {
                Char9Bit sym = canonical_forms[current_symbol];
                if (sym == ONE_MORE_FILE) {
                    file_end = true;
                } else if (sym == ARCHIVE_END) {
                    file_end = true;
                    archive_end = true;
                } else {
                    output.WriteChar(static_cast<char>(sym));
                }
                current_symbol.clear();
            } else {
                current_symbol.push_back(input.GetBit());
            }
        }
        output.Flush();
    }
}
