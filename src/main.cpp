#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../include/CachingHtmlProviderProxy.h"
#include "../include/HtmlProvider.h"
#include "../include/Notification.h"
#include "../include/Observer.h"
#include "../include/Registry.h"
#include "../include/Subject.h"
#include "../include/Task.h"
#include "../include/TaskBuilder.h"
#include "../include/TaskDecorator.h"
#include "../include/TaskProcess.h"
#include "Internship.h"
#include "ShortcodeExpanderDecorator.h"

void populateRegistry() {
  auto &registry = Registry::instance();

  registry.internships["internship_1"] = std::make_shared<Internship>(
      "internship_1", "Google", "Software Engineer Intern",
      InternshipStatus::STARTED, "2024-06-01", "2024-08-31");
}

int main() {
  populateRegistry();

  std::unique_ptr<HtmlProvider> baseMarkdownProvider =
      std::make_unique<MarkdownAdapter>();

  std::unique_ptr<HtmlProvider> asciidocProvider =
      std::make_unique<AsciiDocAdapter>();

  std::unique_ptr<CachingHtmlProviderProxy> markdownProviderWithCache =
      std::make_unique<CachingHtmlProviderProxy>(
          std::move(baseMarkdownProvider));

  std::unique_ptr<ShortcodeExpanderDecorator> markdownProvider =
      std::make_unique<ShortcodeExpanderDecorator>(
          std::move(markdownProviderWithCache));

  std::string markdown = "# Heading 1\n"
                         "A reference to internship: **[internship_1]**";

  std::cout << "For markdown: \n";

  std::cout << markdownProvider->getHtml(markdown) << std::endl;

  std::string asciidoc =
      "= My Document Title\n"
      "\n"
      "This is an AsciiDoc paragraph with *bold* and _italic_.\n"
      "\n"
      "== Section One\n"
      "Text under section one.";

  std::cout << "For asciidoc: \n";

  std::cout << asciidocProvider->getHtml(asciidoc) << std::endl;

  return 0;
}
