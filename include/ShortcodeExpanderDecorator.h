#ifndef SHORTCODE_EXPANDER_DECORATOR_H
#define SHORTCODE_EXPANDER_DECORATOR_H

#include "HtmlProviderDecorator.h"
#include <memory>
#include <string>

class ShortcodeExpanderDecorator : public HtmlProviderDecorator {
public:
  ShortcodeExpanderDecorator(std::unique_ptr<HtmlProvider> provider);

  std::string getHtml(const std::string &input) override;

private:
  std::string expandShortcodes(const std::string &html_content) const;
};

#endif // SHORTCODE_EXPANDER_DECORATOR_H
