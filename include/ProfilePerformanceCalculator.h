#include "PerformanceStrategy.h"
#include "PerformanceVisitor.h"
#include "Registry.h"
#include "Subject.h"

class ProfilePerformanceCalculator : public PerformanceVisitor {
private:
  std::unique_ptr<PerformanceStrategy> subjectStrategy;

public:
  explicit ProfilePerformanceCalculator(
      std::unique_ptr<PerformanceStrategy> subStrategy)
      : subjectStrategy(std::move(subStrategy)) {}

  double visit(Subject &subject) override {
    return subjectStrategy->calculate(subject.getTasks());
  }

  double visit(Registry &registry) override {
    if (registry.subjects.empty())
      return 0.0;

    double totalPerformanceScore = 0.0;
    int count = 0;

    for (const auto &pair : registry.subjects) {
      if (pair.second) {
        totalPerformanceScore += pair.second->accept(*this);
        count++;
      }
    }

    return (count == 0) ? 0.0 : totalPerformanceScore / count;
  }
};
