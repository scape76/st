#ifndef MARKDOWN_PARSER_H
#define MARKDOWN_PARSER_H

#include <iostream>
#include <string>
#include <vector>

enum class TokenType {
  HEADER1,
  HEADER2,
  BOLD_STAR,
  ITALIC_STAR,
  TEXT,
  NEWLINE,
  END_OF_FILE
};

struct Token {
  TokenType type;
  std::string value;
};

struct HtmlNode {
  enum class Type { PARAGRAPH, H1, H2, BOLD, ITALIC, PLAIN_TEXT };
  Type type;
  std::string content;
  std::vector<HtmlNode> children;

  HtmlNode(Type t = Type::PARAGRAPH, std::string c = "",
           std::vector<HtmlNode> ch = {})
      : type(t), content(std::move(c)), children(std::move(ch)) {}

  std::string toHtml() const {
    std::string child_html;
    for (const auto &child : children) {
      child_html += child.toHtml();
    }

    switch (type) {
    case Type::PARAGRAPH:
      return "<p>" + (content.empty() ? child_html : content + child_html) +
             "</p>\n";
    case Type::H1:
      return "<h1>" + (content.empty() ? child_html : content + child_html) +
             "</h1>\n";
    case Type::H2:
      return "<h2>" + (content.empty() ? child_html : content + child_html) +
             "</h2>\n";
    case Type::BOLD:
      return "<strong>" +
             (content.empty() ? child_html : content + child_html) +
             "</strong>";
    case Type::ITALIC:
      return "<em>" + (content.empty() ? child_html : content + child_html) +
             "</em>";
    case Type::PLAIN_TEXT:
      return content;
    default:
      return "";
    }
  }
};

class MarkdownTokenizer {
private:
  const std::string &source;
  size_t currentIndex;

public:
  MarkdownTokenizer(const std::string &src);
  std::vector<Token> tokenize();
};

class MarkdownParser {
private:
  const std::vector<Token> &tokens;
  size_t current_token_index;
  bool in_bold;
  bool in_italic;

public:
  MarkdownParser(const std::vector<Token> &toks);
  std::string parseAndConvert();
};

#endif // MARKDOWN_PARSER_H
