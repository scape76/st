#include "ShortcodeExpanderDecorator.h"
#include "Registry.h"
#include <iostream>
#include <regex>

ShortcodeExpanderDecorator::ShortcodeExpanderDecorator(
    std::unique_ptr<HtmlProvider> provider)
    : HtmlProviderDecorator(std::move(provider)) {}

std::string ShortcodeExpanderDecorator::getHtml(const std::string &input) {
  std::string initial_html;
  if (wrapped_provider) {
    initial_html = wrapped_provider->getHtml(input);
  } else {
    std::cerr << "Error: ShortcodeExpanderDecorator has no wrapped provider!"
              << std::endl;
    return "Error: No wrapped provider.";
  }

  // 2. Post-process the HTML to expand shortcodes
  std::cout << "[DECORATOR] Expanding shortcodes..." << std::endl;
  return expandShortcodes(initial_html);
}

std::string ShortcodeExpanderDecorator::expandShortcodes(
    const std::string &html_content) const {
  std::string result_html;                      // Build the result here
  std::string subject_to_search = html_content; // The string to search within

  // Regex to find [internship_KEY]
  // The KEY can contain alphanumeric characters and underscores
  std::regex shortcode_regex("\\[internship_([a-zA-Z0-9_]+)\\]");

  auto &registry = Registry::instance();

  std::smatch match;
  auto search_start =
      subject_to_search.cbegin(); // Start searching from the beginning

  while (std::regex_search(search_start, subject_to_search.cend(), match,
                           shortcode_regex)) {
    // Append the part of the string before the current match
    result_html.append(match.prefix().first, match.prefix().second);
    // Or, more simply: result_html += match.prefix().str();

    std::string full_match_str =
        match[0].str(); // e.g., "[internship_software_dev]"
    std::string internship_key_part =
        match[1].str(); // e.g., "software_dev" or "1"

    // Construct the key as it would appear in the registry map
    std::string map_key = "internship_" + internship_key_part;

    auto it = registry.internships.find(map_key);
    if (it != registry.internships.end() && it->second) {
      std::cout << "[DECORATOR] Found shortcode: " << full_match_str
                << " -> Expanding with details for key: " << map_key
                << std::endl;
      result_html +=
          it->second->getDetails(); // Replace with internship details
    } else {
      std::cout << "[DECORATOR] Shortcode " << full_match_str
                << " (key: " << map_key
                << ") not found in registry or invalid. Keeping original."
                << std::endl;
      result_html += full_match_str; // Shortcode not found, keep it as is
    }

    // Move the search start to the end of the current match to find subsequent
    // matches
    search_start = match.suffix().first;
  }

  // Append the rest of the string after the last match (if any)
  result_html.append(search_start, subject_to_search.cend());

  return result_html;
}
