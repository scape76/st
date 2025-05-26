#include "../include/AsciiDocParser.h"

AsciiDocTokenizer::AsciiDocTokenizer(const std::string &src)
    : source(src), currentIndex(0) {}

std::vector<AsciiDocToken> AsciiDocTokenizer::tokenize() {
  std::vector<AsciiDocToken> tokens;
  std::string current_text_segment;
  bool at_start_of_line = true;

  while (currentIndex < source.length()) {
    char c = source[currentIndex];
    char next_c =
        (currentIndex + 1 < source.length()) ? source[currentIndex + 1] : '\0';
    char next_next_c =
        (currentIndex + 2 < source.length()) ? source[currentIndex + 2] : '\0';

    if (at_start_of_line) {
      if (c == '=' && next_c == ' ') { // Document Title
        if (!current_text_segment.empty()) {
          tokens.push_back({AsciiDocTokenType::TEXT, current_text_segment});
          current_text_segment.clear();
        }
        tokens.push_back({AsciiDocTokenType::DOC_TITLE_MARKER, "= "});
        currentIndex += 2; // Consume "= "
        at_start_of_line = false;
        continue;
      } else if (c == '=' && next_c == '=' &&
                 next_next_c == ' ') { // Section L1
        if (!current_text_segment.empty()) {
          tokens.push_back({AsciiDocTokenType::TEXT, current_text_segment});
          current_text_segment.clear();
        }
        tokens.push_back({AsciiDocTokenType::SECTION_L1_MARKER, "== "});
        currentIndex += 3; // Consume "== "
        at_start_of_line = false;
        continue;
      } else if (c == '=' && next_c == '=' && next_next_c == '=' &&
                 (currentIndex + 3 < source.length() &&
                  source[currentIndex + 3] == ' ')) { // Section L2
        if (!current_text_segment.empty()) {
          tokens.push_back({AsciiDocTokenType::TEXT, current_text_segment});
          current_text_segment.clear();
        }
        tokens.push_back({AsciiDocTokenType::SECTION_L2_MARKER, "=== "});
        currentIndex += 4; // Consume "=== "
        at_start_of_line = false;
        continue;
      }
    }

    // Inline or regular text
    if (c == '*') {
      if (!current_text_segment.empty()) {
        tokens.push_back({AsciiDocTokenType::TEXT, current_text_segment});
        current_text_segment.clear();
      }
      tokens.push_back({AsciiDocTokenType::BOLD_MARKER, "*"});
      currentIndex++;
      at_start_of_line = false;
    } else if (c == '_') {
      if (!current_text_segment.empty()) {
        tokens.push_back({AsciiDocTokenType::TEXT, current_text_segment});
        current_text_segment.clear();
      }
      tokens.push_back({AsciiDocTokenType::ITALIC_MARKER, "_"});
      currentIndex++;
      at_start_of_line = false;
    } else if (c == '\n') {
      if (!current_text_segment.empty()) {
        tokens.push_back({AsciiDocTokenType::TEXT, current_text_segment});
        current_text_segment.clear();
      }
      tokens.push_back({AsciiDocTokenType::NEWLINE, "\n"});
      currentIndex++;
      at_start_of_line = true;
    } else {
      current_text_segment += c;
      currentIndex++;
      at_start_of_line = false;
    }
  }

  if (!current_text_segment.empty()) {
    tokens.push_back({AsciiDocTokenType::TEXT, current_text_segment});
  }
  tokens.push_back({AsciiDocTokenType::END_OF_FILE, ""});
  return tokens;
}

// --- AsciiDocParser Implementation ---
AsciiDocParser::AsciiDocParser(const std::vector<AsciiDocToken> &toks)
    : tokens(toks), current_token_index(0), in_bold_context(false),
      in_italic_context(false) {}

AsciiDocToken AsciiDocParser::peek() const {
  if (current_token_index < tokens.size()) {
    return tokens[current_token_index];
  }
  return {AsciiDocTokenType::END_OF_FILE, ""};
}

AsciiDocToken AsciiDocParser::consume() {
  if (current_token_index < tokens.size()) {
    return tokens[current_token_index++];
  }
  return {AsciiDocTokenType::END_OF_FILE, ""};
}

// Helper to parse content of a line (text and inline elements) until a NEWLINE
// or EOF
void AsciiDocParser::parseLineContent(AsciiDocHtmlNode &parentNode,
                                      AsciiDocTokenType terminator) {
  while (peek().type != terminator &&
         peek().type != AsciiDocTokenType::END_OF_FILE) {
    AsciiDocToken current_token = peek();
    if (current_token.type == AsciiDocTokenType::TEXT) {
      consume(); // consume TEXT token
      if (in_bold_context && !parentNode.children.empty() &&
          parentNode.children.back().type == AsciiDocHtmlNode::Type::BOLD) {
        parentNode.children.back().content += current_token.value;
      } else if (in_italic_context && !parentNode.children.empty() &&
                 parentNode.children.back().type ==
                     AsciiDocHtmlNode::Type::ITALIC) {
        parentNode.children.back().content += current_token.value;
      } else {
        // If last child is PLAIN_TEXT, append. Otherwise, create new.
        if (!parentNode.children.empty() &&
            parentNode.children.back().type ==
                AsciiDocHtmlNode::Type::PLAIN_TEXT) {
          parentNode.children.back().content += current_token.value;
        } else {
          parentNode.children.push_back(
              {AsciiDocHtmlNode::Type::PLAIN_TEXT, current_token.value});
        }
      }
    } else if (current_token.type == AsciiDocTokenType::BOLD_MARKER) {
      consume(); // consume BOLD_MARKER
      if (!in_bold_context) {
        parentNode.children.push_back({AsciiDocHtmlNode::Type::BOLD});
        in_bold_context = true;
      } else {
        // This implies closing the bold. Check if the last child is indeed
        // BOLD.
        if (!parentNode.children.empty() &&
            parentNode.children.back().type == AsciiDocHtmlNode::Type::BOLD) {
          in_bold_context = false;
        } else { // Mismatched marker, treat as plain text
          parentNode.children.push_back(
              {AsciiDocHtmlNode::Type::PLAIN_TEXT, current_token.value});
        }
      }
    } else if (current_token.type == AsciiDocTokenType::ITALIC_MARKER) {
      consume(); // consume ITALIC_MARKER
      if (!in_italic_context) {
        parentNode.children.push_back({AsciiDocHtmlNode::Type::ITALIC});
        in_italic_context = true;
      } else {
        if (!parentNode.children.empty() &&
            parentNode.children.back().type == AsciiDocHtmlNode::Type::ITALIC) {
          in_italic_context = false;
        } else { // Mismatched marker
          parentNode.children.push_back(
              {AsciiDocHtmlNode::Type::PLAIN_TEXT, current_token.value});
        }
      }
    } else {
      // Should not happen if line content is well-defined, or it's an unhandled
      // token type
      consume(); // Consume to avoid infinite loop
    }
  }
  // Reset inline contexts at the end of line processing (or specific
  // terminator)
  in_bold_context = false;
  in_italic_context = false;
}

AsciiDocHtmlNode AsciiDocParser::parse() {
  AsciiDocHtmlNode documentRoot{AsciiDocHtmlNode::Type::DOCUMENT_ROOT};
  AsciiDocHtmlNode *current_block_node =
      nullptr; // Points to current H1, H2, P, etc.

  while (peek().type != AsciiDocTokenType::END_OF_FILE) {
    AsciiDocToken token = peek();

    if (token.type == AsciiDocTokenType::DOC_TITLE_MARKER) {
      consume(); // Consume marker
      documentRoot.children.emplace_back(AsciiDocHtmlNode::Type::DOC_TITLE);
      current_block_node = &documentRoot.children.back();
      parseLineContent(*current_block_node); // Content until NEWLINE

      if (peek().type == AsciiDocTokenType::NEWLINE)
        consume(); // Consume the NEWLINE
    } else if (token.type == AsciiDocTokenType::SECTION_L1_MARKER) {
      consume(); // Consume marker

      documentRoot.children.emplace_back(AsciiDocHtmlNode::Type::SECTION_H2);
      current_block_node = &documentRoot.children.back();
      parseLineContent(*current_block_node);

      if (peek().type == AsciiDocTokenType::NEWLINE)
        consume();
    } else if (token.type == AsciiDocTokenType::SECTION_L2_MARKER) {
      consume(); // Consume marker
      documentRoot.children.emplace_back(AsciiDocHtmlNode::Type::SECTION_H3);
      current_block_node = &documentRoot.children.back();
      parseLineContent(*current_block_node);
      if (peek().type == AsciiDocTokenType::NEWLINE)
        consume();
    } else if (token.type == AsciiDocTokenType::NEWLINE) {
      consume(); // Consume newline
      // Blank lines might separate paragraphs.
      // If the last block was a paragraph and it's empty, we might not want to
      // add another. For simplicity, newlines primarily terminate header lines
      // here or act as separators. Actual paragraph logic would be more complex
      // (e.g. collecting non-empty lines)
      current_block_node = nullptr; // Reset current block focus
    }
    // Detect start of a paragraph
    // A paragraph starts if we have TEXT, BOLD, or ITALIC and not currently in
    // a header definition
    else if (token.type == AsciiDocTokenType::TEXT ||
             token.type == AsciiDocTokenType::BOLD_MARKER ||
             token.type == AsciiDocTokenType::ITALIC_MARKER) {
      // If previous block was not a paragraph, or no block, start new paragraph
      if (!current_block_node ||
          current_block_node->type != AsciiDocHtmlNode::Type::PARAGRAPH) {
        documentRoot.children.emplace_back(AsciiDocHtmlNode::Type::PARAGRAPH);
        current_block_node = &documentRoot.children.back();
      }
      parseLineContent(
          *current_block_node); // Parse the line into the paragraph
      if (peek().type == AsciiDocTokenType::NEWLINE) {
        consume();                    // Consume the NEWLINE
        current_block_node = nullptr; // End of paragraph block after its line
      }
    } else {
      consume(); // Unknown token, skip
    }
  }
  return documentRoot;
}
