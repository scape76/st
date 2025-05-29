#ifndef RESUME_H
#define RESUME_H

#include <memory>
#include <string>

struct Resume {
  std::string id;
  std::string title;
  std::string htmlBody;

  Resume(std::string resumeId, std::string resumeTitle, std::string body)
      : id(std::move(resumeId)), title(std::move(resumeTitle)),
        htmlBody(std::move(body)) {}

  const std::string &getId() const { return id; }
  const std::string &getTitle() const { return title; }
  const std::string &getHtmlBody() const { return htmlBody; }
};

#endif // RESUME_H
