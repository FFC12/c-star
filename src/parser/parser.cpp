#include <memory>
#include <optional>
#include <parser/parser.hpp>
#include <tuple>

static inline bool IsOperator(TokenInfo& token) {
  if(token == Tok::PLUS
    || token == Tok::MINUS
    || token == Tok::STAR
    || token == Tok::DIV) {
    return true;
  } else {
    return false;
  }
}


Parser::Parser(std::shared_ptr<std::vector<TokenInfo>>& tokens)
  : m_TokenList(tokens)
{
  if(!tokens->empty()) {
    m_CurrTok = (*tokens)[0];
  }

  // Hashing tokens as binding powers
  // for each operator
  // -- INFIX operators
  m_InfixMap[Tok::MINUS] = {1,2};
  m_InfixMap[Tok::PLUS] = {1,2};
  m_InfixMap[Tok::STAR] = {3,4};
  m_InfixMap[Tok::DIV] = {3,4};

  // -- POSTFIX operators
  // -- PREFIX operators
  m_PrefixMap[Tok::MINUS] = {0,5};
}

std::optional<TokenInfo> Parser::next() {
  if(m_TokenList->size() <= m_TokenPtr) {
    return std::nullopt;
  } else {
    m_TokenPtr = m_TokenPtr + ((m_TokenList->size() > m_TokenPtr) ? 1 : 0);
    m_CurrTok = (*m_TokenList)[m_TokenPtr];

    return { m_CurrTok.value() };
  }
}

std::optional<TokenInfo> Parser::peek() {
  if(m_TokenList->size() <= m_TokenPtr) {
    return std::nullopt;
  } else {
    if(m_TokenList->size() > m_TokenPtr + 1) {
      return (*m_TokenList)[m_TokenPtr + 1];
    } else {
      return std::nullopt;
    }
  }
}

// 2 + 4
// -5 + 5
void Parser::parseExpression(int minPrec) {
  if(m_CurrTok.has_value()) {
    auto tokenInfo = m_CurrTok.value();

    if(IsOperator(tokenInfo)) {
      auto pbp = m_PrefixMap[tokenInfo.token];
      auto rightPbp = std::get<1>(pbp);
      m_CurrTok = next();
      parseExpression(rightPbp);
    }

    while(true) {
      auto nextTok = peek();

      if(nextTok.has_value()) {
        if(!IsOperator(nextTok.value())) {
          break;
        } else {
          m_CurrTok = next();
        }
      }

      auto ibp = m_InfixMap[nextTok.value().token];
      auto leftIbp = std::get<0>(ibp);
      auto rightIbp = std::get<1>(ibp);

      if(leftIbp < minPrec) {
        break;
      }

      m_CurrTok = next();
      parseExpression(rightIbp);
    }
  }
}

void Parser::run() {
  parseExpression(0);
}
