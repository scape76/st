#pragma once

#include "Task.h"
#include <memory>
#include <vector>

class PerformanceStrategy {
public:
  virtual ~PerformanceStrategy() = default;
  virtual double
  calculate(const std::vector<std::shared_ptr<Task>> &tasks) const = 0;
};

class AverageMarksStrategy : public PerformanceStrategy {
public:
  double
  calculate(const std::vector<std::shared_ptr<Task>> &tasks) const override {
    if (tasks.empty()) {
      return 0.0;
    }
    double totalMarks = 0;
    int completedTasksCount = 0;
    for (const auto &task : tasks) {
      if (task && task->isCompleted()) {
        totalMarks += task->getMarks();
        completedTasksCount++;
      }
    }
    return (completedTasksCount == 0) ? 0.0 : totalMarks / completedTasksCount;
  }
};

class CompletionRateStrategy : public PerformanceStrategy {
public:
  double
  calculate(const std::vector<std::shared_ptr<Task>> &tasks) const override {
    if (tasks.empty()) {
      return 0.0;
    }

    int completedCount = 0;
    for (const auto &task : tasks) {
      if (task && task->isCompleted()) {
        completedCount++;
      }
    }
    return (static_cast<double>(completedCount) / tasks.size()) * 100.0;
  }
};

class AverageProgressStrategy : public PerformanceStrategy {
public:
  double
  calculate(const std::vector<std::shared_ptr<Task>> &tasks) const override {
    if (tasks.empty()) {
      return 0.0;
    }

    double totalProgress = 0.0;

    for (const auto &task : tasks) {
      if (task) {
        totalProgress += task->getProgress();
      }
    }

    return totalProgress / tasks.size();
  }
};
