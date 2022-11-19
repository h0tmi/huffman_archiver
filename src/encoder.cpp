//
// Created by Тимофей Жамойдин on 23.10.22.
//
#include <unordered_map>
#include <map>
#include <iostream>
#include "prefix_tree.h"
#include "bit_editors.h"
#include <queue>
#include "constants.h"
#include "encoder.h"
#include "bit_view.h"

struct BuildUnit {
    std::shared_ptr<PTNode> node;
    size_t weight;

    BuildUnit(const std::shared_ptr<PTNode> &node, size_t weight) : node(node), weight(weight) {
    }
    bool operator<(const BuildUnit &other) const {
        return std::tie(weight, node->character) >= std::tie(other.weight, other.node->character);
    }
    bool operator==(const BuildUnit &rhs) const {
        return node == rhs.node && weight == rhs.weight;
    }
};
std::unordered_map<HuffmanCode, Char9Bit> BuildPrefixTree(const std::unordered_map<Char9Bit, size_t> &frequency_table) {
    std::priority_queue<BuildUnit> q;
    for (const auto &[character, frequency] : frequency_table) {
        auto new_node = new PTNode({}, character, true);
        q.push(BuildUnit(std::shared_ptr<PTNode>(new_node), frequency));
    }
    while (q.size() > 1) {
        auto a = q.top();
        q.pop();
        auto b = q.top();
        q.pop();
        if (a.node->character > b.node->character) {
            std::swap(a, b);
        }
        auto new_node = new PTNode({{0, a.node}, {1, b.node}}, std::min(a.node->character, b.node->character));
        q.push({std::shared_ptr<PTNode>(new_node), a.weight + b.weight});
    }
    std::vector<PTElement> vertexes = Iterate(*q.top().node);
    std::unordered_map<HuffmanCode, Char9Bit> result;
    for (const auto &v : vertexes) {
        result[v.branch] = v.node.character;
    }
    return result;
}
std::vector<HuffmanCode> SortCodes(const std::unordered_map<HuffmanCode, Char9Bit> &pref_tree) {
    std::vector<HuffmanCode> codes;
    codes.reserve(pref_tree.size());
    for (const auto &[mask, _] : pref_tree) {
        codes.push_back(mask);
    }
    auto comp = [&](const auto &lhs, const auto &rhs) -> bool {
        return std::make_tuple(lhs.size(), pref_tree.at(lhs)) < std::make_tuple(rhs.size(), pref_tree.at(rhs));
    };
    std::sort(codes.begin(), codes.end(), comp);
    return codes;
}
std::vector<Char9Bit> SortAlphas(const std::unordered_map<HuffmanCode, Char9Bit> &pref_tree) {
    auto sorted_codes = SortCodes(pref_tree);
    std::vector<Char9Bit> result;
    result.reserve(pref_tree.size());
    for (const auto &code : sorted_codes) {
        result.push_back(pref_tree.at(code));
    }
    return result;
}
std::unordered_map<Char9Bit, HuffmanCode> GetCodes(const std::vector<HuffmanCode> &codes,
                                                   const std::unordered_map<HuffmanCode, Char9Bit> &code_to_char) {
    BitView canon(codes[0].size());
    std::unordered_map<Char9Bit, HuffmanCode> result;
    for (size_t i = 0; i < codes.size(); ++i) {
        auto code = codes[i];
        if (i != 0) {
            ++canon;
        }
        canon.ToSize(code.size());
        result[code_to_char.at(code)] = canon.Data();
    }
    return result;
}
template <std::unsigned_integral T>
HuffmanCode To9Bites(T a) {
    std::vector<bool> write(9);
    for (size_t i = 0; i < 9; ++i) {
        write[i] = (a >> (8 - i)) & 1;
    }
    return write;
}
void WriteFileName(Writer &output_stream, const std::unordered_map<HuffmanCode, Char9Bit> &pref_tree,
                   const std::unordered_map<Char9Bit, HuffmanCode> &canonical_forms) {
    output_stream.WriteBits(To9Bites(canonical_forms.size()));
    std::vector<Char9Bit> sorted_alph = SortAlphas(pref_tree);
    for (const auto &character : sorted_alph) {
        output_stream.WriteBits(To9Bites(character));
    }
    std::array<size_t, 256> lengths;
    lengths.fill(0);
    size_t max_code_size = 0;
    for (const auto &[_, can_code] : canonical_forms) {
        ++lengths[can_code.size()];
        max_code_size = std::max(max_code_size, can_code.size());
    }
    for (size_t i = 1; i <= max_code_size; ++i) {
        output_stream.WriteBits(To9Bites(lengths[i]));
    }
}
void WriteFileData(Writer &output_stream, Reader &input_stream,
                   const std::unordered_map<Char9Bit, HuffmanCode> &canonical_forms) {
    for (const char &character : input_stream.GetFilename()) {
        u_char c = character;
        output_stream.WriteBits(canonical_forms.at(c));
    }
    output_stream.WriteBits(canonical_forms.at(FILENAME_END));
    while (!input_stream.IsEof()) {
        auto c = input_stream.GetByte();
        output_stream.WriteBits(canonical_forms.at(c));
    }
}

void Encode(const std::string_view archive_name, const std::vector<std::string_view> &filenames) {
    std::unordered_map<Char9Bit, HuffmanCode> canonical_codes;
    FileWriter output(archive_name);
    for (size_t i = 0; i < filenames.size(); ++i) {
        auto current_file = filenames[i];
        FileReader input(current_file);
        std::unordered_map<Char9Bit, size_t> frequency_table = {{FILENAME_END, 1}, {ONE_MORE_FILE, 1}, {ARCHIVE_END, 1}};
        for (const auto &character : input.GetFilename()) {
            ++frequency_table[character];
        }
        while (!input.IsEof()) {
            ++frequency_table[input.GetByte()];
        }

        auto prefix_tree = BuildPrefixTree(frequency_table);
        std::priority_queue<BuildUnit> q;
        for (const auto &[character, frequency] : frequency_table) {
            auto new_node = new PTNode({}, character, true);
            q.push(BuildUnit(std::shared_ptr<PTNode>(new_node), frequency));
        }
        while (q.size() > 1) {
            auto a = q.top();
            q.pop();
            auto b = q.top();
            q.pop();
            if (a.node->character > b.node->character) {
                std::swap(a, b);
            }
            auto new_node = new PTNode({{0, a.node}, {1, b.node}}, std::min(a.node->character, b.node->character));
            q.push({std::shared_ptr<PTNode>(new_node), a.weight + b.weight});
        }
        std::vector<PTElement> vertexes = Iterate(*q.top().node);
        std::unordered_map<HuffmanCode, Char9Bit> result;
        for (const auto &v : vertexes) {
            result[v.branch] = v.node.character;
        }

        auto sorted_codes = SortCodes(prefix_tree);
        canonical_codes = GetCodes(sorted_codes, prefix_tree);

        WriteFileName(output, prefix_tree, canonical_codes);

        input.Reset();
        WriteFileData(output, input, canonical_codes);

        if (i + 1 < filenames.size()) {
            output.WriteBits(canonical_codes[ONE_MORE_FILE]);
        }
    }
    output.WriteBits(canonical_codes[ARCHIVE_END]);
    output.Flush();
}
