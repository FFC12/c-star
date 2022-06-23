#ifndef CSTAR_TOKENIZER_HPP
#define CSTAR_TOKENIZER_HPP

#include <base.hpp>
#include <cassert>
#include <fstream>
#include <helpers/magic_enum.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <parser/tokens.hpp>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <map>

class Parser;

class Tokenizer {
    friend Parser;
    std::map<std::string, Tok> m_ReservedKeywords;
    std::vector<std::string> m_Lines;
    std::shared_ptr<std::vector<TokenInfo>> m_TokenInfos;
    std::string m_Buffer;
    std::string_view m_CurrLine;
    size_t m_CurrPtr = 0;
    size_t m_LineNo = 0;
    TokenInfo m_CurrToken{};
    std::optional<char> m_CurrChar;

    void hashingReservedKeywords() {
        // check if it's reserved keyword
        for (int i = 0;
             static_cast<ReservedKeywords>(i) < ReservedKeywords::END_OF_ENUM;
             i++) {
            ReservedKeywords keyword = (ReservedKeywords) i;
            auto tokenStr = GetTokenAsStr<ReservedKeywords>(keyword);
            m_ReservedKeywords[tokenStr] = GetStrAsToken(tokenStr);
        }
    }

    void movePtrToNextLine() {
        m_LineNo++;
        m_CurrPtr = 0;
    }

    std::optional<char> peekChar() {
        // We have to check is it done with consuming lines
        // if we reached the EOF out then just let it break
        // the outer loop through nullopt
        if (m_Lines.size() - 1 < m_LineNo) {
            m_CurrChar = std::nullopt;
            return m_CurrChar;
        }

        m_CurrLine = m_Lines[m_LineNo];

        auto lineStrSize = m_CurrLine.empty() ? 0 : m_CurrLine.size();
        if (lineStrSize < m_CurrPtr + 1) {
            m_CurrChar = std::nullopt;
        } else {
            m_CurrChar = m_CurrLine[m_CurrPtr++];

            if (m_CurrChar == '\000') {
                m_CurrChar = std::nullopt;
            }
        }

        return m_CurrChar;
    }

    std::optional<char> restoreChar() {
        m_CurrLine = m_Lines[m_LineNo];

        if (m_CurrPtr == 0) {
            m_LineNo--;
            auto lastLine = m_Lines[m_LineNo];
            m_CurrPtr = lastLine.size() - 1;
            m_CurrLine = m_Lines[m_LineNo];
        }

        m_CurrChar = m_CurrLine[m_CurrPtr--];
        return m_CurrChar;
    }

    bool lookAhead(char c) {
        if (c == peekChar()) {
            return true;
        } else {
            restoreChar();
            return false;
        }
    }

    void beginToken() {
        m_CurrToken.begin = m_CurrPtr;
        m_CurrToken.line = m_LineNo + 1;  // normalized
        m_CurrToken.span = m_CurrLine;
    }

    void endToken(Tok token) {
        if (token == Tok::NL)
            m_CurrToken.end = m_CurrToken.begin;
        else
            m_CurrToken.end = m_CurrPtr;

        m_CurrToken.token = token;

        m_TokenInfos->emplace_back(std::move(m_CurrToken));

        // reset
        m_CurrToken.value = "";
    }

    static inline Tok GetStrAsToken(const std::string &str) {
        std::string tempStr = str;
        for (auto &c: tempStr) {
            c = std::toupper(c);
        }

        auto token = magic_enum::enum_cast<Tok>(tempStr);
        if (token.has_value()) {
            return token.value();
        } else {
            cstar_assert("Not possible in our case!");
        }
    }

    Tok tokenize(std::optional<char> c);

    Tok advanceConstant();

    Tok advanceIdentifier();

    Tok advanceLiteral();

    Tok advanceSingleChar();

    Tok advanceComments(bool multiline);

public:
    Tokenizer() = default;

    explicit Tokenizer(const std::string &path);

    void run();

    std::shared_ptr<std::vector<TokenInfo>> getTokenInfoList();

    template<typename T>
    static inline std::string GetTokenAsStr(T token) {
        static_assert(std::is_enum<T>::value, "The type must be enum");
        auto nameView = magic_enum::enum_name(token);
        std::string name(nameView);
        for (auto &c: name) {
            c = std::tolower(c);
        }
        return name;
    }
};

#endif //!TOKENIZER_HPP
