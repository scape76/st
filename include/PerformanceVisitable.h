#pragma once

class PerformanceVisitor;

class PerformanceVisitable {
public:
  virtual ~PerformanceVisitable() = default;
  virtual double accept(PerformanceVisitor &visitor) = 0;
};
