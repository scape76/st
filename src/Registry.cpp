#include "../include/Registry.h"
#include "PerformanceVisitor.h"

Registry &Registry::instance() {
  static Registry s;
  return s;
}

double Registry::accept(PerformanceVisitor &visitor) {
  return visitor.visit(*this);
}
