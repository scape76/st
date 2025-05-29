#ifndef REGISTRY_H
#define REGISTRY_H

#include "Internship.h"
#include "PerformanceVisitable.h"
#include "PerformanceVisitor.h"
#include "Resume.h"
#include "Subject.h"
#include <memory>
#include <string>
#include <unordered_map>

struct Registry : public PerformanceVisitable {
  std::unordered_map<std::string, std::shared_ptr<Subject>> subjects;
  std::unordered_map<std::string, std::shared_ptr<Internship>> internships;
  std::unordered_map<std::string, std::shared_ptr<Resume>> resumes;

  static Registry &instance();

  double accept(PerformanceVisitor &visitor) override;

  Registry(const Registry &) = delete;
  Registry &operator=(const Registry &) = delete;

private:
  Registry() {}
  ~Registry() {}
};

#endif
