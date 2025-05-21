#ifndef GRADING_SYSTEM_ADAPTER_H
#define GRADING_SYSTEM_ADAPTER_H

#include "Task.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

// Simulated external grading systems that we need to adapt to

// External university grading system interface
class ExternalUniversityGradingSystem {
public:
  virtual ~ExternalUniversityGradingSystem() = default;

  // External interface methods
  virtual void submitTask(const std::string &studentId,
                          const std::string &courseCode,
                          const std::string &taskName,
                          const std::string &submissionContent) = 0;

  virtual double retrieveScore(const std::string &studentId,
                               const std::string &courseCode,
                               const std::string &taskName) = 0;

  virtual std::string getLetterGrade(double score) = 0;

  virtual bool isSubmissionLate(const std::string &studentId,
                                const std::string &courseCode,
                                const std::string &taskName) = 0;
};

// External online learning platform grading system
class ExternalOnlinePlatformGradingSystem {
public:
  virtual ~ExternalOnlinePlatformGradingSystem() = default;

  // Completely different interface
  virtual int uploadLab(const std::string &userId, const std::string &moduleId,
                        const std::string &taskId,
                        const std::string &fileContent) = 0;

  virtual int getPoints(const std::string &userId, const std::string &moduleId,
                        const std::string &taskId) = 0;

  virtual bool taskCompleted(const std::string &userId,
                             const std::string &moduleId,
                             const std::string &taskId) = 0;

  virtual int getMaxPoints(const std::string &moduleId,
                           const std::string &taskId) = 0;
};

// Our application's grading system interface
class IGradingSystem {
public:
  virtual ~IGradingSystem() = default;

  // Common interface for working with tasks
  virtual void submitTask(std::shared_ptr<Task> task,
                          const std::string &studentId,
                          const std::string &submissionContent) = 0;

  virtual int getScore(std::shared_ptr<Task> task,
                       const std::string &studentId) = 0;

  virtual bool isCompleted(std::shared_ptr<Task> task,
                           const std::string &studentId) = 0;

  virtual std::string getGradeRepresentation(int score) = 0;

  virtual std::string getSystemName() const = 0;
};

// Adapter for the university grading system
class UniversityGradingSystemAdapter : public IGradingSystem {
private:
  std::shared_ptr<ExternalUniversityGradingSystem> externalSystem;

public:
  UniversityGradingSystemAdapter(
      std::shared_ptr<ExternalUniversityGradingSystem> system)
      : externalSystem(system) {}

  void submitTask(std::shared_ptr<Task> task, const std::string &studentId,
                  const std::string &submissionContent) override;

  int getScore(std::shared_ptr<Task> task,
               const std::string &studentId) override;

  bool isCompleted(std::shared_ptr<Task> task,
                   const std::string &studentId) override;

  std::string getGradeRepresentation(int score) override;

  std::string getSystemName() const override {
    return "University Grading System";
  }
};

// Adapter for the online platform grading system
class OnlinePlatformGradingSystemAdapter : public IGradingSystem {
private:
  std::shared_ptr<ExternalOnlinePlatformGradingSystem> externalSystem;

public:
  OnlinePlatformGradingSystemAdapter(
      std::shared_ptr<ExternalOnlinePlatformGradingSystem> system)
      : externalSystem(system) {}

  void submitTask(std::shared_ptr<Task> task, const std::string &studentId,
                  const std::string &submissionContent) override;

  int getScore(std::shared_ptr<Task> task,
               const std::string &studentId) override;

  bool isCompleted(std::shared_ptr<Task> task,
                   const std::string &studentId) override;

  std::string getGradeRepresentation(int score) override;

  std::string getSystemName() const override {
    return "Online Learning Platform";
  }
};

// Facade class to simplify working with different grading systems
class GradingSystemFacade {
private:
  std::map<std::string, std::shared_ptr<IGradingSystem>> gradingSystems;
  std::string activeSystem;
  std::string studentId;

public:
  GradingSystemFacade(const std::string &studentId);

  // Register a grading system
  void registerGradingSystem(const std::string &name,
                             std::shared_ptr<IGradingSystem> system);

  // Set the active grading system
  void setActiveGradingSystem(const std::string &name);

  // Get list of available grading systems
  std::vector<std::string> getAvailableGradingSystems() const;

  // Simplified methods for clients to use
  void submitTask(std::shared_ptr<Task> task,
                  const std::string &submissionContent);

  int getScore(std::shared_ptr<Task> task);

  bool isCompleted(std::shared_ptr<Task> task);

  std::string getGradeRepresentation(int score);

  std::string getActiveSystemName() const;
};

#endif // GRADING_SYSTEM_ADAPTER_H
