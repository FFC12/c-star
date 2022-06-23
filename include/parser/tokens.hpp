#ifndef CSTAR_TOKENS_HPP
#define CSTAR_TOKENS_HPP

#include <base.hpp>
#include <type_traits>
#include <variant>

enum class ReservedKeywords {
    I8 = 0,
    I16,
    I32,
    I64,
    //  I128,
    U8,
    U16,
    U32,
    U64,
    U128,
    SIZE,
    F32,
    F64,
    UCHAR,
    CHAR,
    BOOL,
    VEC2,
    VEC3,
    VEC4,
    VEC,
    VOID,
    ANY,
    ATTRIB,
    STRUCT,
    ENUM,
    BREAK,
    CONTINUE,
    CONSTPTR,
    CONSTREF,
    READONLY,
    RET,
    IN,
    AS,
    IF,
    ELIF,
    ELSE,
    REF,
    DEREF,
    INCLUDE,
    INVOLVED,
    OPTION,
    SWITCH,
    LOOP,
    DEFAULT,
    EXTERN,
    FROM,
    IMPORT,
    NATIVE,
    EXPORT,
    STATIC,
    CAST,
    UNSAFE_CAST,
    SIZEOF,
    TYPEOF,
    MOVE,
    CONST,
    NIL,
    TRUE,
    FALSE,
    AWAIT,
    END_OF_ENUM
};

enum Tok {
    I8,
    I16,
    I32,
    I64,
    //  I128,
    U8,
    U16,
    U32,
    U64,
    U128,
    SIZE,
    F32,
    F64,
    UCHAR,
    CHAR,
    BOOL,
    VEC2,
    VEC3,
    VEC4,
    VEC,
    VOID,
    ANY,
    ATTRIB,
    STRUCT,
    ENUM,
    BREAK,
    CONTINUE,
    CONSTPTR,
    CONSTREF,
    READONLY,
    RET,
    IN,
    AS,
    IF,
    ELIF,
    ELSE,
    REF,
    DEREF,
    INCLUDE,
    INVOLVED,
    OPTION,
    SWITCH,
    LOOP,
    DEFAULT,
    EXTERN,
    FROM,
    IMPORT,
    NATIVE,
    EXPORT,
    STATIC,
    CAST,
    UNSAFE_CAST,
    SIZEOF,
    TYPEOF,
    MOVE,
    CONST,
    NIL,
    TRUE,
    FALSE,
    AWAIT,
    //-----------
    SCALARI,  // integer scalar like 10, 11
    SCALARD,  // double scalar like 10.03f, 20.0243f
    MATRIX,
    VECTOR,
    IDENT,
    LITERAL,
    LETTER,
    DOT,
    DOLLAR,  // $
    RANGE,
    TRIPLET,
    SEMICOLON,
    COMMA,
    UNDERSCORE,  // _
    TYPEINF,
    PLUS,
    PLUSEQ,
    PLUSPLUS,
    MINUS,
    MINUSMINUS,
    MINUSEQ,
    QMARK,   //?
    ARROW,   //->
    DARROW,  // =>
    DIV,
    DIVEQ,
    STAR,    // *
    STAREQ,  // *=
    MOD,
    MODEQ,
    LSHIFT,
    LSHIFTEQ,
    RSHIFT,
    RSHIFTEQ,
    EQUAL,
    EQUALEQUAL,
    NOT,
    NOTEQUAL,
    TILDE,
    TILDEEQ,
    LAND,
    AND,
    ANDEQ,
    LOR,
    OR,
    OREQ,
    XOR,
    XOREQ,
    LTEQ,
    LT,
    GTEQ,
    GT,
    COMMENT,
    LBRACK,
    RBRACK,
    LPAREN,
    RPAREN,
    LSQPAR,
    RSQPAR,
    HASH,
    COLONCOLON,
    COLON,
    PAREQ,  // partial eq ===
    MOVEQ,  // .=
    APOSTROPHE,
    SQUOTE,
    DQUOTE,
    UNHANDLED,
    NL,  // \n
    HT,  // \t
    WS,  // 0x20
};

struct TokenInfo {
    Tok token;
    std::string value{};
    std::string span{};
    size_t begin = 1;
    size_t end = 1;
    size_t line = 1;

    TokenInfo() = default;
    TokenInfo(TokenInfo& other) {
        token = other.token;
        this->value = other.value;
        this->span = other.span;
        this->begin = other.begin;
        this->end = other.end;
        this->line = other.line;
    }

    TokenInfo(TokenInfo&& other) noexcept {
        this->token = other.token;
        this->value = std::move(other.value);
        this->span = std::move(other.span);
        this->begin = other.begin;
        this->end = other.end;
        this->line = other.line;
    }

    TokenInfo& operator=(TokenInfo&& other)  noexcept {
        this->token = other.token;
        this->value = std::move(other.value);
        this->span = std::move(other.span);
        this->begin = other.begin;
        this->end = other.end;
        this->line = other.line;
        return *this;
    }

    bool operator==(const TokenInfo& rhs) const { return (this->token) == rhs.token; }
    bool operator==(Tok rhs) const { return (this->token) == rhs; }
};

#endif