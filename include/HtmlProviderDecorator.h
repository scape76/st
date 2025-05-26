#ifndef HTML_PROVIDER_DECORATOR_H
#define HTML_PROVIDER_DECORATOR_H

#include "HtmlProvider.h"
#include <memory>
#include <string>

class HtmlProviderDecorator : public HtmlProvider {
protected:
  std::unique_ptr<HtmlProvider> wrapped_provider;

public:
  HtmlProviderDecorator(std::unique_ptr<HtmlProvider> provider)
      : wrapped_provider(std::move(provider)) {}

  std::string getHtml(const std::string &input) override {
    if (wrapped_provider) {
      return wrapped_provider->getHtml(input);
    }

    return "Error: No wrapped provider in decorator.";
  }

  ~HtmlProviderDecorator() override = default;
};

#endif // HTML_PROVIDER_DECORATOR_H
