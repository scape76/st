#ifndef HTML_PROVIDER_H
#define HTML_PROVIDER_H

#include <memory>
#include <string>

class HtmlProvider {
public:
  virtual ~HtmlProvider() = default;
  virtual std::string getHtml(const std::string &input) = 0;
};

class AsciiDocAdapter : public HtmlProvider {
public:
  std::string getHtml(const std::string &input) override;
};

class MarkdownAdapter : public HtmlProvider {
public:
  std::string getHtml(const std::string &input) override;
};

#endif // HTML_PROVIDER_H
