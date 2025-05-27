#pragma once

class Subject;
class Registry;

class PerformanceVisitor {
public:
  virtual ~PerformanceVisitor() = default;
  virtual double visit(Subject &subject) = 0;

  virtual double visit(Registry &registry) = 0;
};
