#include <iostream>
#include "parser.h"
#include "encoder.h"
#include "decoder.h"
#include "constants.h"

int main(int argc, char** argv) {
    Instruction<Help> help_instruction(FlagType({"-h"}), [](Help data) -> void {
        std::cout << HELP;
    });
    Instruction<Encoded> encoding_instruction(FlagType({"-c"}), [](Encoded data) -> void {
        Encode(data.archive_name, data.files);
    });
    Instruction<Decoded> decoding_instruction(FlagType({"-d"}), [](Decoded data) -> void {
        Decode(data.archive_name);
    });
    Parser parser(std::tie(encoding_instruction, decoding_instruction, help_instruction));
    parser.Execute(argc, argv);
    return 0;
}
