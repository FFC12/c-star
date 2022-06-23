#ifndef CSTAR_PARSER_HPP
#define CSTAR_PARSER_HPP
#include <optional>
#include <tuple>
#include <memory>
#include <map>
#include <parser/tokenizer.hpp>

// Operator precedence
// Pratt Parsing...
class Parser {
  using PrecMap = std::map<Tok, std::tuple<int,int>>;
private:
  std::optional<TokenInfo> next();

  std::optional<TokenInfo> peek();

  std::optional<TokenInfo> m_CurrTok;
  std::shared_ptr<std::vector<TokenInfo>> m_TokenList;

  // Operator precedence hashmaps
  // for infix, prefix and postfix operators
  PrecMap m_InfixMap;
  PrecMap m_PrefixMap;
  PrecMap m_PostfixMap;

  size_t m_TokenPtr{0};
public:
  Parser(std::shared_ptr<std::vector<TokenInfo>> &tokens);

  void run();

  void parseExpression(int minPrec = 0);
};

#endif
