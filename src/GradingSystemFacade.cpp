#include "../include/GradingSystemAdapter.h"
#include "../include/Subject.h"
#include <stdexcept>

void UniversityGradingSystemAdapter::submitTask(
    std::shared_ptr<Task> task, const std::string &studentId,
    const std::string &submissionContent) {

  if (!task || !task->getSubject()) {
    throw std::runtime_error("Task or subject not set properly");
  }

  externalSystem->submitTask(studentId, task->getSubject()->getCode(),
                             task->getTitle(), submissionContent);
}

int UniversityGradingSystemAdapter::getScore(std::shared_ptr<Task> task,
                                             const std::string &studentId) {

  if (!task || !task->getSubject()) {
    return 0;
  }

  double externalScore = externalSystem->retrieveScore(
      studentId, task->getSubject()->getCode(), task->getTitle());

  return static_cast<int>(externalScore * 10);
}

bool UniversityGradingSystemAdapter::isCompleted(std::shared_ptr<Task> task,
                                                 const std::string &studentId) {

  if (!task || !task->getSubject()) {
    return false;
  }

  double score = externalSystem->retrieveScore(
      studentId, task->getSubject()->getCode(), task->getTitle());

  return score > 0;
}

std::string UniversityGradingSystemAdapter::getGradeRepresentation(int score) {
  double externalScore = score / 10.0;

  return externalSystem->getLetterGrade(externalScore);
}

void OnlinePlatformGradingSystemAdapter::submitTask(
    std::shared_ptr<Task> task, const std::string &studentId,
    const std::string &submissionContent) {

  if (!task || !task->getSubject()) {
    throw std::runtime_error("Task or subject not set properly");
  }

  externalSystem->uploadLab(
      studentId,
      task->getSubject()->getCode(),
      task->getTitle(),
      submissionContent);
}

int OnlinePlatformGradingSystemAdapter::getScore(std::shared_ptr<Task> task,
                                                 const std::string &studentId) {

  if (!task || !task->getSubject()) {
    return 0;
  }

  int points = externalSystem->getPoints(
      studentId,
      task->getSubject()->getCode(),
      task->getTitle()
  );

  int maxPoints = externalSystem->getMaxPoints(task->getSubject()->getCode(),
                                               task->getTitle());

  return maxPoints > 0 ? (points * 100) / maxPoints : 0;
}

bool OnlinePlatformGradingSystemAdapter::isCompleted(
    std::shared_ptr<Task> task, const std::string &studentId) {

  if (!task || !task->getSubject()) {
    return false;
  }

  return externalSystem->taskCompleted(
      studentId,
      task->getSubject()->getCode(),
      task->getTitle()
  );
}

std::string
OnlinePlatformGradingSystemAdapter::getGradeRepresentation(int score) {
  if (score >= 90)
    return "A";
  if (score >= 80)
    return "B";
  if (score >= 70)
    return "C";
  if (score >= 60)
    return "D";
  return "F";
}

GradingSystemFacade::GradingSystemFacade(const std::string &studentId)
    : studentId(studentId), activeSystem("") {}

void GradingSystemFacade::registerGradingSystem(
    const std::string &name, std::shared_ptr<IGradingSystem> system) {

  gradingSystems[name] = system;

  if (activeSystem.empty()) {
    activeSystem = name;
  }
}

void GradingSystemFacade::setActiveGradingSystem(const std::string &name) {
  if (gradingSystems.find(name) == gradingSystems.end()) {
    throw std::runtime_error("Grading system '" + name + "' not registered");
  }

  activeSystem = name;
}

std::vector<std::string>
GradingSystemFacade::getAvailableGradingSystems() const {
  std::vector<std::string> names;
  for (const auto &pair : gradingSystems) {
    names.push_back(pair.first);
  }
  return names;
}

void GradingSystemFacade::submitTask(std::shared_ptr<Task> task,
                                     const std::string &submissionContent) {

  if (activeSystem.empty() ||
      gradingSystems.find(activeSystem) == gradingSystems.end()) {
    throw std::runtime_error("No active grading system set");
  }

  gradingSystems[activeSystem]->submitTask(task, studentId, submissionContent);
}

int GradingSystemFacade::getScore(std::shared_ptr<Task> task) {
  if (activeSystem.empty() ||
      gradingSystems.find(activeSystem) == gradingSystems.end()) {
    return 0;
  }

  return gradingSystems[activeSystem]->getScore(task, studentId);
}

bool GradingSystemFacade::isCompleted(std::shared_ptr<Task> task) {
  if (activeSystem.empty() ||
      gradingSystems.find(activeSystem) == gradingSystems.end()) {
    return false;
  }

  return gradingSystems[activeSystem]->isCompleted(task, studentId);
}

std::string GradingSystemFacade::getGradeRepresentation(int score) {
  if (activeSystem.empty() ||
      gradingSystems.find(activeSystem) == gradingSystems.end()) {
    if (score >= 90)
      return "A";
    if (score >= 80)
      return "B";
    if (score >= 70)
      return "C";
    if (score >= 60)
      return "D";
    return "F";
  }

  return gradingSystems[activeSystem]->getGradeRepresentation(score);
}

std::string GradingSystemFacade::getActiveSystemName() const {
  return activeSystem.empty()
             ? "None"
             : gradingSystems.at(activeSystem)->getSystemName();
}