#ifndef CACHING_HTML_PROVIDER_PROXY_H
#define CACHING_HTML_PROVIDER_PROXY_H

#include "HtmlProvider.h"
#include <map>
#include <memory>
#include <string>

class CachingHtmlProviderProxy : public HtmlProvider {
private:
  std::unique_ptr<HtmlProvider> real_provider;
  std::map<std::string, std::string> cache;

public:
  explicit CachingHtmlProviderProxy(std::unique_ptr<HtmlProvider> provider);

  std::string getHtml(const std::string &input) override;

  void clearCache();

  size_t getCacheSize() const;
};

#endif // CACHING_HTML_PROVIDER_PROXY_H
