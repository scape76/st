#include "CachingHtmlProviderProxy.h"
#include <iostream>

CachingHtmlProviderProxy::CachingHtmlProviderProxy(
    std::unique_ptr<HtmlProvider> provider)
    : real_provider(std::move(provider)) {}

std::string CachingHtmlProviderProxy::getHtml(const std::string &input) {
  auto it = cache.find(input);
  if (it != cache.end()) {
    std::cout << "[CACHE HIT] Returning cached HTML for input." << std::endl;
    return it->second;
  }

  std::cout << "[CACHE MISS] Processing input and caching result..."
            << std::endl;
  if (!real_provider) {
    throw std::runtime_error(
        "CachingHtmlProviderProxy: Real provider is not set.");
  }

  std::string result = real_provider->getHtml(input);

  cache[input] = result;

  return result;
}

void CachingHtmlProviderProxy::clearCache() {
  cache.clear();
  std::cout << "[CACHE CLEARED]" << std::endl;
}

size_t CachingHtmlProviderProxy::getCacheSize() const { return cache.size(); }
