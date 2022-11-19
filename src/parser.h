//
// Created by Тимофей Жамойдин on 23.10.22.
//

#pragma once

#include <functional>
#include <vector>
#include <string>
#include <tuple>
#include "my_exceptions.h"
#include "constants.h"

class Encoded {
public:
    Encoded() = default;
    explicit Encoded(const std::vector<std::string_view>& s);

public:
    std::string archive_name;
    std::vector<std::string_view> files;
};
Encoded::Encoded(const std::vector<std::string_view>& s) {
    if (s.size() < 2) {
        throw BadArgumentException(LOW_ARGS);
    }
    archive_name = s[0];
    files.insert(files.end(), s.begin() + 1, s.end());
}
class Decoded {
public:
    Decoded() = default;
    explicit Decoded(const std::vector<std::string_view>& s);

public:
    std::string archive_name;
};
Decoded::Decoded(const std::vector<std::string_view>& s) {
    if (s.size() != 1) {
        throw BadArgumentException(ARGS_ERROR);
    }
    archive_name = s[0];
}
class Help {
public:
    Help() = default;
    explicit Help(const std::vector<std::string_view>& s);
};
Help::Help(const std::vector<std::string_view>& s) {
}

template <typename ARGTYPE>
class Instruction {
public:
    Instruction(std::function<bool(std::string)> checker, std::function<void(ARGTYPE)> instruction);
    bool Check(int argc, char** argv);
    void Execute();

private:
    std::function<bool(std::string)> checker_;
    ARGTYPE argument_;
    std::function<void(ARGTYPE)> instruction_;
};
template <typename ARGTYPE>
bool Instruction<ARGTYPE>::Check(int argc, char** argv) {
    if (argc <= 1) {
        return false;
    }
    auto first_parameter = std::string(argv[1]);
    if (checker_(first_parameter)) {
        std::vector<std::string_view> other;
        for (size_t i = 2; i < argc; ++i) {
            other.push_back(std::string_view(argv[i]));
        }
        argument_ = ARGTYPE(other);
        return true;
    }
    return false;
}
template <typename ARGTYPE>
void Instruction<ARGTYPE>::Execute() {
    instruction_(argument_);
}
template <typename ARGTYPE>
Instruction<ARGTYPE>::Instruction(std::function<bool(std::string)> checker, std::function<void(ARGTYPE)> instruction)
    : checker_(checker), instruction_(instruction) {
}

template <class... ARGS>
class Parser {
public:
    explicit Parser(const std::tuple<ARGS...>& args);

    template <size_t CNT = 0, typename... T>
    typename std::enable_if<CNT == sizeof...(T) - 1, void>::type ExecuteTuple(std::tuple<T...> tup, int argc,
                                                                              char** argv) {
        std::get<CNT>(tup).Check(argc, argv);
        std::get<CNT>(tup).Execute();
    }

    template <size_t CNT = 0, typename... T>
    typename std::enable_if<(CNT < sizeof...(T) - 1), void>::type ExecuteTuple(std::tuple<T...> tup, int argc,
                                                                               char** argv) {
        auto handler = std::get<CNT>(tup);
        if (handler.Check(argc, argv)) {
            handler.Execute();
            return;
        }
        ExecuteTuple<CNT + 1>(tup, argc, argv);
    }

    void Execute(int argc, char** argv);

private:
    std::tuple<ARGS...> args_;
};
template <class... ARGS>
Parser<ARGS...>::Parser(const std::tuple<ARGS...>& args) : args_(args) {
}
template <class... ARGS>
void Parser<ARGS...>::Execute(int argc, char** argv) {
    ExecuteTuple(args_, argc, argv);
}

class FlagType {
public:
    explicit FlagType(const std::vector<std::string>& flags) : flags_(flags) {
    }
    bool operator()(std::string pos) {
        return std::find(flags_.begin(), flags_.end(), pos) != flags_.end();
    }

private:
    std::vector<std::string> flags_;
};
