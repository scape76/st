#ifndef ASSIGNMENT_PROCESS_H
#define ASSIGNMENT_PROCESS_H

#include "Task.h"
#include <iostream>
#include <memory>
#include <string>

class TaskProcess {
protected:
  virtual void validateTask(std::shared_ptr<Task> assignment) = 0;
  virtual bool checkPrerequisites(std::shared_ptr<Task> assignment) = 0;
  virtual void processSubmission(std::shared_ptr<Task> assignment,
                                 const std::string &submission) = 0;
  virtual int evaluateTask(std::shared_ptr<Task> assignment) = 0;
  virtual void provideFeedback(std::shared_ptr<Task> assignment, int score) = 0;
  virtual void updateRecords(std::shared_ptr<Task> assignment, int score) = 0;

  virtual void beforeSubmission(std::shared_ptr<Task> assignment) {
    std::cout << "Preparing to process assignment: " << assignment->getTitle()
              << std::endl;
  }

  virtual void afterCompletion(std::shared_ptr<Task> assignment) {
    std::cout << "Task process completed for: " << assignment->getTitle()
              << std::endl;
  }

public:
  virtual ~TaskProcess() = default;

  void processTask(std::shared_ptr<Task> assignment,
                   const std::string &submission) {
    beforeSubmission(assignment);

    validateTask(assignment);

    if (!checkPrerequisites(assignment)) {
      std::cout << "Prerequisites not met for assignment: "
                << assignment->getTitle() << std::endl;
      return;
    }

    processSubmission(assignment, submission);
    int score = evaluateTask(assignment);
    provideFeedback(assignment, score);
    updateRecords(assignment, score);

    afterCompletion(assignment);
  }
};

class LabProcess : public TaskProcess {
protected:
  void validateTask(std::shared_ptr<Task> assignment) override;
  bool checkPrerequisites(std::shared_ptr<Task> assignment) override;
  void processSubmission(std::shared_ptr<Task> assignment,
                         const std::string &submission) override;
  int evaluateTask(std::shared_ptr<Task> assignment) override;
  void provideFeedback(std::shared_ptr<Task> assignment, int score) override;
  void updateRecords(std::shared_ptr<Task> assignment, int score) override;
};

class ProjectProcess : public TaskProcess {
protected:
  void validateTask(std::shared_ptr<Task> assignment) override;
  bool checkPrerequisites(std::shared_ptr<Task> assignment) override;
  void processSubmission(std::shared_ptr<Task> assignment,
                         const std::string &submission) override;
  int evaluateTask(std::shared_ptr<Task> assignment) override;
  void provideFeedback(std::shared_ptr<Task> assignment, int score) override;
  void updateRecords(std::shared_ptr<Task> assignment, int score) override;

  void beforeSubmission(std::shared_ptr<Task> assignment) override;
};

class ExamProcess : public TaskProcess {
protected:
  void validateTask(std::shared_ptr<Task> assignment) override;
  bool checkPrerequisites(std::shared_ptr<Task> assignment) override;
  void processSubmission(std::shared_ptr<Task> assignment,
                         const std::string &submission) override;
  int evaluateTask(std::shared_ptr<Task> assignment) override;
  void provideFeedback(std::shared_ptr<Task> assignment, int score) override;
  void updateRecords(std::shared_ptr<Task> assignment, int score) override;

  void afterCompletion(std::shared_ptr<Task> assignment) override;
};

#endif