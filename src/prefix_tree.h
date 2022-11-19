//
// Created by Тимофей Жамойдин on 23.10.22.
//

#pragma once

#include <array>
#include <concepts>
#include <cstdlib>
#include <memory>
#include <string>
#include <queue>
#include <optional>

class PTNode {
public:
    uint16_t character;
    PTNode() = default;
    PTNode(std::initializer_list<std::pair<size_t, std::shared_ptr<PTNode>>> descendants, uint16_t character = 0,
           bool griper = false) {
        this->character = character;
        griper_ = griper;
        for (const auto &[current_character, node] : descendants) {
            descendants_[current_character] = node;
        }
    }
    bool IsGripped() const {
        return griper_;
    }
    void SetGripper() {
        griper_ = true;
    }
    std::optional<PTNode> GetDescend(size_t ind) const {
        if (descendants_[ind] != nullptr) {
            return std::make_optional(*descendants_[ind]);
        }
        return std::nullopt;
    }
    void SetDescend(size_t ind, std::shared_ptr<PTNode> new_node) {
        descendants_[ind] = new_node;
    }
    bool HasDescend(size_t ind) const {
        return descendants_[ind] != nullptr;
    }

private:
    std::array<std::shared_ptr<PTNode>, 2> descendants_;
    bool griper_ = true;
};

struct PTElement {
    PTNode node;
    std::vector<bool> branch;
    PTElement(const PTNode &node, const std::vector<bool> &branch) : node(node), branch(branch) {
    }
};

std::vector<PTElement> Iterate(const PTNode &t) {
    std::queue<PTElement> q;
    std::vector<PTElement> result;
    q.push(PTElement(t, {}));
    while (!q.empty()) {
        auto front_element = q.front();
        q.pop();
        if (front_element.node.IsGripped()) {
            result.push_back(front_element);
        }
        for (size_t a = 0; a < 2; ++a) {
            if (front_element.node.HasDescend(a)) {
                auto child_branch = front_element.branch;
                child_branch.push_back(static_cast<bool>(a));
                q.push(PTElement(front_element.node.GetDescend(a).value(), child_branch));
            }
        }
    }
    return result;
}

void Add(PTNode &t, const std::vector<bool> &a) {
    auto &current_node = t;
    for (const auto &c : a) {
        if (current_node.GetDescend(c) == std::nullopt) {
            current_node.SetDescend(c, std::make_unique<PTNode>());
        }
        current_node = current_node.GetDescend(c).value();
    }
    current_node.SetGripper();
}
