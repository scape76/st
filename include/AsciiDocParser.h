#ifndef ASCIIDOC_PARSER_H
#define ASCIIDOC_PARSER_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

enum class AsciiDocTokenType {
  DOC_TITLE_MARKER,  // "= "
  SECTION_L1_MARKER, // "== "
  SECTION_L2_MARKER, // "=== "
  BOLD_MARKER,       // "*"
  ITALIC_MARKER,     // "_"
  TEXT,
  NEWLINE,
  END_OF_FILE,
  UNKNOWN
};

struct AsciiDocToken {
  AsciiDocTokenType type;
  std::string value;
};

struct AsciiDocHtmlNode {
  enum class Type {
    DOCUMENT_ROOT,
    DOC_TITLE,
    SECTION_H2,
    SECTION_H3,
    PARAGRAPH,
    BOLD,
    ITALIC,
    PLAIN_TEXT
  };
  Type type;
  std::string content;
  std::vector<AsciiDocHtmlNode> children;

  AsciiDocHtmlNode(Type t = Type::PARAGRAPH, std::string c = "",
                   std::vector<AsciiDocHtmlNode> ch = {})
      : type(t), content(std::move(c)), children(std::move(ch)) {}

  std::string toHtml(int indent_level = 0) const {
    std::string html_string;
    std::string indent(indent_level * 2, ' ');

    std::string child_html;
    for (const auto &child : children) {
      child_html +=
          child.toHtml(type == Type::DOCUMENT_ROOT ? 0 : indent_level + 1);
    }

    switch (type) {
    case Type::DOCUMENT_ROOT:
      return child_html;
    case Type::DOC_TITLE:
      return indent + "<h1>" +
             (content.empty() ? child_html : content + child_html) + "</h1>\n";
    case Type::SECTION_H2:
      return indent + "<h2>" +
             (content.empty() ? child_html : content + child_html) + "</h2>\n";
    case Type::SECTION_H3:
      return indent + "<h3>" +
             (content.empty() ? child_html : content + child_html) + "</h3>\n";
    case Type::PARAGRAPH:
      if (content.empty() && child_html.empty())
        return "";
      return indent + "<p>" +
             (content.empty() ? child_html : content + child_html) + "</p>\n";
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

class AsciiDocTokenizer {
private:
  const std::string &source;
  size_t currentIndex;

public:
  AsciiDocTokenizer(const std::string &src);
  std::vector<AsciiDocToken> tokenize();
};

class AsciiDocParser {
private:
  const std::vector<AsciiDocToken> &tokens;
  size_t current_token_index;

  bool in_bold_context;
  bool in_italic_context;

  void
  parseLineContent(AsciiDocHtmlNode &parentNode,
                   AsciiDocTokenType terminator = AsciiDocTokenType::NEWLINE);
  AsciiDocToken peek() const;
  AsciiDocToken consume();

public:
  AsciiDocParser(const std::vector<AsciiDocToken> &toks);
  AsciiDocHtmlNode parse();
};

#endif // ASCIIDOC_PARSER_H
