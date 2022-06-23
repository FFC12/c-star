#include <parser/tokenizer.hpp>
#include <fstream>

//TODO: m_TokenInfos needs to instance again.
Tokenizer::Tokenizer(const std::string &path) {
    if (m_TokenInfos == nullptr) {
        m_TokenInfos = std::make_unique<std::vector<TokenInfo>>();
    }

    auto skipBOM = [&](auto &fs) {
        const unsigned char boms[]{0xEF, 0xBB, 0xBF};
        bool has_bom = true;
        for (const auto &c: boms) {
            if ((unsigned char) fs.get() != c) has_bom = false;
        }
        if (!has_bom) fs.seekg(0);
        return has_bom;
    };

    if (path.empty()) {
        cstar_assert("Path cannot be null or empty!");
    }

    std::ifstream file(path, std::ios::binary | std::ios::in);

    if (skipBOM(file)) {
        // skipping BOM (actually already skipped at this point)
    }

    std::string line;

    while (std::getline(file, line)) {
#if _WIN32
        auto c = line[line.size() - 1];
        if(c == '\r') line.erase(line.size() - 1);
#endif
        if (file.eof()) {
            line = line;
        } else {
            line += "\n";
        }

        this->m_Lines.push_back(line);
        this->m_Buffer.append(line);
    }

    hashingReservedKeywords();
}

Tok Tokenizer::tokenize(std::optional<char> c) {
    switch (c.value()) {
        // Integer or Float Constants
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return advanceConstant();

            // Identifier
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case '_':
            return advanceIdentifier();

        case '{':
            return LBRACK;
        case '}':
            return RBRACK;
        case '(':
            return LPAREN;
        case ')':
            return RPAREN;
        case '[':
            return LSQPAR;
        case ']':
            return RSQPAR;
        case '?':
            return QMARK;
        case '"':
            return advanceLiteral();
        case '\'':
            return advanceSingleChar();
        case '!':
            if (lookAhead('=')) return NOTEQUAL;
            return NOT;
        case '#':
            return HASH;
        case '.':
            if (lookAhead('.')) {
                if (lookAhead('.')) {
                    return TRIPLET;
                }
                return RANGE;
            } else if (lookAhead('=')) {
                return MOVEQ;
            }
            return DOT;
        case ',':
            return COMMA;
        case '|':
            if (lookAhead('|')) {
                return LOR;
            } else if (lookAhead('=')) {
                return OREQ;
            } else {
                return OR;
            }
        case '&':
            if (lookAhead('&')) {
                return LAND;
            } else if (lookAhead('=')) {
                return ANDEQ;
            } else {
                return AND;
            }
        case '*': {
            if (lookAhead('=')) return STAREQ;
            return STAR;
        }
        case '+':
            if (lookAhead('+')) {
                return PLUSPLUS;
            } else if (lookAhead('=')) {
                return PLUSEQ;
            } else {
                return PLUS;
            }
        case '-':
            if (lookAhead('-')) {
                return MINUSMINUS;
            } else if (lookAhead('=')) {
                return MINUSEQ;
            } else if (lookAhead('>')) {
                return ARROW;
            } else {
                return MINUS;
            }
        case '%':
            if (lookAhead('=')) {
                return MODEQ;
            }
            return MOD;
        case '=':
            if (lookAhead('=')) {
                return EQUALEQUAL;
            } else if (lookAhead('>')) {
                return DARROW;
            } else {
                return EQUAL;
            }
        case '~':
            if (lookAhead('=')) {
                return TILDEEQ;
            }
            return TILDE;
        case '/':
            // If it's comment then skip..
            if (lookAhead('/')) {
                return advanceComments(false);
            } else if (lookAhead('*')) {
                return advanceComments(true);
            } else {
                if (lookAhead('=')) {
                    return DIVEQ;
                }
                return DIV;
            }
        case '<':
            if (lookAhead('<')) {
                if (lookAhead('=')) {
                    return LSHIFTEQ;
                } else {
                    return LSHIFT;
                }
            } else if (lookAhead('=')) {
                return LTEQ;
            } else {
                return LT;
            }
        case '>':
            if (lookAhead('>')) {
                if (lookAhead('=')) {
                    return RSHIFTEQ;
                } else {
                    return RSHIFT;
                }
            } else if (lookAhead('=')) {
                return GTEQ;
            } else {
                return GT;
            }
        case '^':
            if (lookAhead('=')) {
                return XOREQ;
            }
            return XOR;
        case ';':
            return SEMICOLON;
        case ':':
            if (lookAhead('=')) {
                return TYPEINF;
            } else if (lookAhead(':')) {
                return COLONCOLON;
            }
            return COLON;
        case '$':
            return DOLLAR;
        case '@':
        case '`': {
            return APOSTROPHE;
        }
        case '\x20': {
            // pass
            return WS;
        }
        case '\n': {
            movePtrToNextLine();
            return NL;
        }
        case '\t': {
            return HT;
        }
        default: {
            return UNHANDLED;
        }
    }
}

void Tokenizer::run() {
    while (auto c = peekChar()) {
        beginToken();
        auto token = tokenize(c);
        endToken(token);
    }
}


Tok Tokenizer::advanceConstant() {
    std::string constant;
    bool isFloat = false;

    std::optional<char> c = m_CurrChar;
    if (c.has_value()) {
        constant += c.value();

        bool pointFlag = false;
        while ((c = peekChar())) {
            if (std::isdigit(c.value()) || c.value() == '.') {
                if (c == '.' && !pointFlag) {
                    pointFlag = true;
                    isFloat = true;
                } else {
                    if (pointFlag && c == '.') {
                        cstar_error("Invalid floating point");
                    }
                }

                constant += c.value();
            } else {
                break;
            }
        }

        restoreChar();
    }

    m_CurrToken.value = constant;
    if (isFloat) {
        return SCALARD;
    } else {
        return SCALARI;
    }
}

Tok Tokenizer::advanceIdentifier() {
    std::string identifier;

    std::optional<char> c = m_CurrChar;
    if (c.has_value()) {
        identifier += c.value();

        while ((c = peekChar())) {
            if (std::isalnum(c.value()) || c.value() == '_') {
                identifier += c.value();
            } else {
                break;
            }
        }

        restoreChar();

        if (m_ReservedKeywords.count(identifier) > 0) {
            return m_ReservedKeywords[identifier];
        } else {
            m_CurrToken.value = identifier;
            return IDENT;
        }
    }
}

Tok Tokenizer::advanceLiteral() {
    std::string literal;

    // advance the '"'
    peekChar();

    std::optional<char> c = m_CurrChar;
    if (c.has_value()) {
        literal += c.value();

        while (c != '"') {
            if (std::isprint(c.value())) {
                literal += c.value();
            } else {
                break;
            }
            c = peekChar();
            if (!c.has_value()) {
                cstar_error("String literal out of range!");
            }
        }
    }

    m_CurrToken.value = literal;
    return LITERAL;
}

Tok Tokenizer::advanceSingleChar() {
    std::string singleCharLiteral;

    // advance the "'"
    peekChar();

    std::optional<char> c = m_CurrChar;
    if (c.has_value()) {
        if (c != '\'') {
            singleCharLiteral += c.value();

            // peek char
            c = peekChar();

            if (c.has_value()) {
                if (c != '\'') {
                    cstar_error("Invalid single character literal");
                }
            } else {
                cstar_error("Invalid single character literal");
            }
        }
    }

    m_CurrToken.value = singleCharLiteral;
    return LETTER;
}

Tok Tokenizer::advanceComments(bool multiline) {
    std::string comment;

    std::optional<char> c = peekChar();
    if (c.has_value()) {
        if (multiline) {
            while (c.has_value()) {
                if (c.value() == '*') {
                    if (lookAhead('/')) {
                        break;
                    }
                } else if (c.value() == '\n') {
                    movePtrToNextLine();
                }
                comment += c.value();
                c = peekChar();
                if (!c.has_value()) {
                    cstar_error("Missing '*/' of multiline comment");
                }
            }
        } else {
            while (c.value() != '\n' && c.value() != '\0') {
                comment += c.value();
                c = peekChar();
                if (!c.has_value()) {
                    break;
                }
            }

            restoreChar();
        }
    }

    m_CurrToken.value = comment;
    return COMMENT;
}

std::shared_ptr<std::vector<TokenInfo>> Tokenizer::getTokenInfoList() {
    return this->m_TokenInfos;
}



