#ifndef REGISTRY_H
#define REGISTRY_H

#include "Internship.h"
#include "Subject.h"
#include <memory>
#include <string>
#include <unordered_map>

struct Registry {
  std::unordered_map<std::string, std::shared_ptr<Subject>> subjects;
  std::unordered_map<std::string, std::shared_ptr<Internship>> internships;

  static Registry &instance() {
    static Registry s;
    return s;
  }

  Registry(const Registry &) = delete;
  Registry &operator=(const Registry &) = delete;

private:
  Registry() {}
  ~Registry() {}
};

#endif
