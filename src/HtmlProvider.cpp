#include "../include/HtmlProvider.h"
#include "../include/AsciiDocParser.h"
#include "../include/MarkdownParser.h"

std::string AsciiDocAdapter::getHtml(const std::string &input) {
  AsciiDocTokenizer tokenizer(input);
  std::vector<AsciiDocToken> tokens = tokenizer.tokenize();

  AsciiDocParser parser(tokens);
  AsciiDocHtmlNode html_tree_root = parser.parse();

  return html_tree_root.toHtml();
}

std::string MarkdownAdapter::getHtml(const std::string &input) {
  MarkdownTokenizer tokenizer(input);
  std::vector<Token> tokens = tokenizer.tokenize();

  MarkdownParser parser(tokens);

  return parser.parseAndConvert();
}
