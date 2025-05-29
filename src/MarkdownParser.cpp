#include "../include/MarkdownParser.h"

MarkdownTokenizer::MarkdownTokenizer(const std::string &src)
    : source(src), currentIndex(0) {}

std::vector<Token> MarkdownTokenizer::tokenize() {
  std::vector<Token> tokens;
  std::string current_text;

  for (size_t i = 0; i < source.length(); ++i) {
    char c = source[i];
    char next_c = (i + 1 < source.length()) ? source[i + 1] : '\0';

    if (c == '#') {
      if (!current_text.empty()) {
        tokens.push_back({TokenType::TEXT, current_text});
        current_text.clear();
      }
      if (next_c == '#') {
        tokens.push_back({TokenType::HEADER2, "##"});
        i++;
      } else {
        tokens.push_back({TokenType::HEADER1, "#"});
      }
    } else if (c == '*') {
      if (!current_text.empty()) {
        tokens.push_back({TokenType::TEXT, current_text});
        current_text.clear();
      }
      if (next_c == '*') {
        tokens.push_back({TokenType::BOLD_STAR, "**"});
        i++;
      } else {
        tokens.push_back({TokenType::ITALIC_STAR, "*"});
      }
    } else if (c == '\n') {
      if (!current_text.empty()) {
        tokens.push_back({TokenType::TEXT, current_text});
        current_text.clear();
      }
      tokens.push_back({TokenType::NEWLINE, "\n"});
    } else {
      current_text += c;
    }
  }

  if (!current_text.empty()) {
    tokens.push_back({TokenType::TEXT, current_text});
  }
  tokens.push_back({TokenType::END_OF_FILE, ""});
  return tokens;
}

MarkdownParser::MarkdownParser(const std::vector<Token> &toks)
    : tokens(toks), current_token_index(0), in_bold(false), in_italic(false) {}

std::string MarkdownParser::parseAndConvert() {
  std::string html_output;

  HtmlNode current_paragraph;

  while (current_token_index < tokens.size() &&
         tokens[current_token_index].type != TokenType::END_OF_FILE) {
    Token current_token = tokens[current_token_index];

    switch (current_token.type) {
    case TokenType::HEADER1:
      if (!current_paragraph.content.empty() ||
          !current_paragraph.children.empty()) {
        html_output += current_paragraph.toHtml();
        current_paragraph = HtmlNode{};
      }
      {
        HtmlNode header_node{HtmlNode::Type::H1, "", {}};
        current_token_index++;

        while (current_token_index < tokens.size() &&
               tokens[current_token_index].type != TokenType::NEWLINE &&
               tokens[current_token_index].type != TokenType::END_OF_FILE) {
          if (tokens[current_token_index].type == TokenType::TEXT) {
            header_node.content += tokens[current_token_index].value;
          }
          current_token_index++;
        }
        html_output += header_node.toHtml();
      }
      break;

    case TokenType::HEADER2:
      if (!current_paragraph.content.empty() ||
          !current_paragraph.children.empty()) {
        html_output += current_paragraph.toHtml();
        current_paragraph = HtmlNode{};
      }
      {
        HtmlNode header_node{HtmlNode::Type::H2, "", {}};
        current_token_index++; // skip '##'

        while (current_token_index < tokens.size() &&
               tokens[current_token_index].type != TokenType::NEWLINE &&
               tokens[current_token_index].type != TokenType::END_OF_FILE) {
          if (tokens[current_token_index].type == TokenType::TEXT) {
            header_node.content += tokens[current_token_index].value;
          }
          current_token_index++;
        }
        html_output += header_node.toHtml();
      }
      break;

    case TokenType::BOLD_STAR:
      if (!in_bold) {
        if (!current_paragraph.content.empty() &&
            current_paragraph.children.empty()) {
          current_paragraph.children.push_back(
              {HtmlNode::Type::PLAIN_TEXT, current_paragraph.content, {}});
          current_paragraph.content.clear();
        }
        current_paragraph.children.push_back({HtmlNode::Type::BOLD, "", {}});
        in_bold = true;
      } else {
        if (!current_paragraph.children.empty() &&
            current_paragraph.children.back().type == HtmlNode::Type::BOLD) {
          in_bold = false;
        } else {
          if (!current_paragraph.children.empty() &&
              (in_italic && current_paragraph.children.back().type ==
                                HtmlNode::Type::ITALIC)) {
            current_paragraph.children.back().content += current_token.value;
          } else {
            current_paragraph.content += current_token.value;
          }
        }
      }
      current_token_index++;
      break;

    case TokenType::ITALIC_STAR:
      if (!in_italic) {
        if (!current_paragraph.content.empty() &&
            current_paragraph.children.empty()) {
          current_paragraph.children.push_back(
              {HtmlNode::Type::PLAIN_TEXT, current_paragraph.content, {}});
          current_paragraph.content.clear();
        }
        current_paragraph.children.push_back({HtmlNode::Type::ITALIC, "", {}});
        in_italic = true;
      } else {
        if (!current_paragraph.children.empty() &&
            current_paragraph.children.back().type == HtmlNode::Type::ITALIC) {
          in_italic = false;
        } else {
          if (!current_paragraph.children.empty() &&
              (in_bold && current_paragraph.children.back().type ==
                              HtmlNode::Type::BOLD)) {
            current_paragraph.children.back().content += current_token.value;
          } else {
            current_paragraph.content += current_token.value;
          }
        }
      }
      current_token_index++;
      break;

    case TokenType::TEXT:
      if (in_bold && !current_paragraph.children.empty() &&
          current_paragraph.children.back().type == HtmlNode::Type::BOLD) {
        current_paragraph.children.back().content += current_token.value;
      } else if (in_italic && !current_paragraph.children.empty() &&
                 current_paragraph.children.back().type ==
                     HtmlNode::Type::ITALIC) {
        current_paragraph.children.back().content += current_token.value;
      } else {
        if (!current_paragraph.children.empty() &&
            (current_paragraph.children.back().type == HtmlNode::Type::BOLD ||
             current_paragraph.children.back().type ==
                 HtmlNode::Type::ITALIC)) {
          current_paragraph.children.push_back(
              {HtmlNode::Type::PLAIN_TEXT, current_token.value, {}});
        } else if (!current_paragraph.children.empty() &&
                   current_paragraph.children.back().type ==
                       HtmlNode::Type::PLAIN_TEXT) {
          current_paragraph.children.back().content += current_token.value;
        } else {
          current_paragraph.content += current_token.value;
        }
      }
      current_token_index++;
      break;

    case TokenType::NEWLINE:
      if (!current_paragraph.content.empty() ||
          !current_paragraph.children.empty()) {
        html_output += current_paragraph.toHtml();
      }
      current_paragraph = HtmlNode{};
      in_bold = false;
      in_italic = false;
      current_token_index++;
      break;

    default:
      current_token_index++;
      break;
    }
  }

  if (!current_paragraph.content.empty() ||
      !current_paragraph.children.empty()) {
    html_output += current_paragraph.toHtml();
  }

  return html_output;
}
