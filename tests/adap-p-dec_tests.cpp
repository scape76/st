#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "../include/CachingHtmlProviderProxy.h"
#include "../include/HtmlProvider.h"
#include "../include/HtmlProviderDecorator.h"
#include "../include/Internship.h"
#include "../include/Registry.h"

#include "../include/AsciiDocParser.h"
#include "../include/MarkdownParser.h"
#include "../include/ShortcodeExpanderDecorator.h"

TEST(AdapterPatternTest, AsciiDocAdapterBasicConversion) {
  AsciiDocAdapter adapter;
  std::string asciidoc_input = "= Document Title\n\nThis is a paragraph.";
  std::string expected_html =
      "<h1>Document Title</h1>\n<p>This is a paragraph.</p>\n";

  std::string simple_asciidoc = "= My Title";
  std::string simple_expected = "<h1>My Title</h1>\n";
  EXPECT_EQ(simple_expected, adapter.getHtml(simple_asciidoc));

  std::string section_asciidoc = "== Section 1";
  std::string section_expected = "<h2>Section 1</h2>\n";
  EXPECT_EQ(section_expected, adapter.getHtml(section_asciidoc));
}

class MockHtmlProvider : public HtmlProvider {
public:
  mutable int call_count = 0;
  std::string last_input;
  std::string canned_response = "<p>Default Mock Response</p>";

  std::string getHtml(const std::string &input) override {
    call_count++;
    last_input = input;
    return canned_response;
  }
};

TEST(ProxyPatternTest, CachingProxyWorks) {
  auto mock_provider = std::make_unique<MockHtmlProvider>();
  MockHtmlProvider *raw_mock_provider = mock_provider.get();

  CachingHtmlProviderProxy proxy(std::move(mock_provider));

  std::string input1 = "test_input_1";
  std::string input2 = "test_input_2";

  std::string result1_call1 = proxy.getHtml(input1);
  EXPECT_EQ(1, raw_mock_provider->call_count);
  EXPECT_EQ(input1, raw_mock_provider->last_input);
  EXPECT_EQ(raw_mock_provider->canned_response, result1_call1);
  EXPECT_EQ(1, proxy.getCacheSize());

  std::string result1_call2 = proxy.getHtml(input1);
  EXPECT_EQ(1, raw_mock_provider->call_count);
  EXPECT_EQ(result1_call1, result1_call2);
  EXPECT_EQ(1, proxy.getCacheSize());

  std::string result2_call1 = proxy.getHtml(input2);
  EXPECT_EQ(2, raw_mock_provider->call_count);
  EXPECT_EQ(input2, raw_mock_provider->last_input);
  EXPECT_EQ(raw_mock_provider->canned_response, result2_call1);
  EXPECT_EQ(2, proxy.getCacheSize());

  std::string result2_call2 = proxy.getHtml(input2);
  EXPECT_EQ(2, raw_mock_provider->call_count);
  EXPECT_EQ(result2_call1, result2_call2);
  EXPECT_EQ(2, proxy.getCacheSize());
}

TEST(ProxyPatternTest, CacheClearing) {
  auto mock_provider = std::make_unique<MockHtmlProvider>();
  CachingHtmlProviderProxy proxy(std::move(mock_provider));

  proxy.getHtml("input_A");
  ASSERT_EQ(1, proxy.getCacheSize());

  proxy.clearCache();
  EXPECT_EQ(0, proxy.getCacheSize());

  MockHtmlProvider *raw_mock_provider_after_clear = new MockHtmlProvider();

  proxy = CachingHtmlProviderProxy(
      std::unique_ptr<HtmlProvider>(raw_mock_provider_after_clear));

  proxy.getHtml("input_A");
  EXPECT_EQ(1, raw_mock_provider_after_clear->call_count);
  EXPECT_EQ(1, proxy.getCacheSize());
}

class DecoratorPatternTest : public ::testing::Test {
protected:
  void SetUp() override {
    Registry::instance().internships.clear();

    Registry::instance().internships["internship_alpha"] =
        std::make_shared<Internship>(
            "alpha_id", "Alpha Corp", "Software Developer Intern",
            InternshipStatus::STARTED, "2025-01-01", "2025-06-30");

    Registry::instance().internships["internship_beta"] =
        std::make_shared<Internship>(
            "beta_id", "Beta Solutions", "Data Analyst Intern",
            InternshipStatus::PENDING, "2025-07-01", "2025-12-31");
  }

  void TearDown() override { Registry::instance().internships.clear(); }
};

TEST_F(DecoratorPatternTest, ShortcodeExpanderReplacesKnownShortcodes) {
  auto mock_provider = std::make_unique<MockHtmlProvider>();

  mock_provider->canned_response =
      "Original HTML with [internship_alpha] and some other text.";

  ShortcodeExpanderDecorator decorator(std::move(mock_provider));

  std::string input_text = "any_input";
  std::string actual_html = decorator.getHtml(input_text);

  std::string expected_html =
      "Original HTML with Software Developer Intern at Alpha "
      "Corp (Status: Started, Start: 2025-01-01, End: "
      "2025-06-30) and some other text.";
  EXPECT_EQ(expected_html, actual_html);
}

TEST_F(DecoratorPatternTest, ShortcodeExpanderKeepsUnknownShortcodes) {
  auto mock_provider = std::make_unique<MockHtmlProvider>();
  mock_provider->canned_response =
      "Text with [internship_unknown] and [internship_alpha].";
  ShortcodeExpanderDecorator decorator(std::move(mock_provider));

  std::string actual_html = decorator.getHtml("any_input");
  std::string expected_html =
      "Text with [internship_unknown] and Software Developer Intern at Alpha "
      "Corp (Status: Started, Start: 2025-01-01, End: 2025-06-30).";
  EXPECT_EQ(expected_html, actual_html);
}

TEST_F(DecoratorPatternTest, ShortcodeExpanderHandlesNoShortcodes) {
  auto mock_provider = std::make_unique<MockHtmlProvider>();
  mock_provider->canned_response = "Just plain text without any shortcodes.";
  ShortcodeExpanderDecorator decorator(std::move(mock_provider));

  std::string actual_html = decorator.getHtml("any_input");
  std::string expected_html = "Just plain text without any shortcodes.";
  EXPECT_EQ(expected_html, actual_html);
}

TEST_F(DecoratorPatternTest, ShortcodeExpanderHandlesMultipleShortcodes) {
  auto mock_provider = std::make_unique<MockHtmlProvider>();
  mock_provider->canned_response =
      "[internship_alpha] then [internship_beta] and again [internship_alpha].";
  ShortcodeExpanderDecorator decorator(std::move(mock_provider));

  std::string actual_html = decorator.getHtml("any_input");
  std::string expected_html =
      "Software Developer Intern at Alpha Corp (Status: Started, Start: "
      "2025-01-01, End: 2025-06-30) then Data Analyst Intern at Beta Solutions "
      "(Status: Pending, Start: 2025-07-01, End: 2025-12-31) and again "
      "Software Developer Intern at Alpha Corp (Status: Started, Start: "
      "2025-01-01, End: 2025-06-30).";
  EXPECT_EQ(expected_html, actual_html);
}
