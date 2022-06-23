#include <iostream>
#include <parser/tokenizer.hpp>
#include <parser/parser.hpp>

int main(int argc, char **argv) {
    if (argc > 1) {
        auto path = std::string(argv[1]);
        Tokenizer tokenizer(path);
        tokenizer.run();

        auto tokenInfoListPtr = tokenizer.getTokenInfoList();

        Parser parser(tokenInfoListPtr);
        parser.run();
    }

    return 0;
}
