#include "../include/GradingSystemAdapter.h"
#include "../include/Subject.h"
#include <stdexcept>

// Concrete implementations of adapters for external grading systems

// University Grading System Adapter
void UniversityGradingSystemAdapter::submitTask(
    std::shared_ptr<Task> task, const std::string &studentId,
    const std::string &submissionContent) {

  if (!task || !task->getSubject()) {
    throw std::runtime_error("Task or subject not set properly");
  }

  // Adapt to external system interface
  externalSystem->submitTask(studentId, task->getSubject()->getCode(),
                             task->getTitle(), submissionContent);
}

int UniversityGradingSystemAdapter::getScore(std::shared_ptr<Task> task,
                                             const std::string &studentId) {

  if (!task || !task->getSubject()) {
    return 0;
  }

  // Adapt to external system interface and convert to our scale (0-100)
  double externalScore = externalSystem->retrieveScore(
      studentId, task->getSubject()->getCode(), task->getTitle());

  // Assume external system uses 0-10 scale, convert to 0-100
  return static_cast<int>(externalScore * 10);
}

bool UniversityGradingSystemAdapter::isCompleted(std::shared_ptr<Task> task,
                                                 const std::string &studentId) {

  if (!task || !task->getSubject()) {
    return false;
  }

  // Task is completed if score is greater than 0
  double score = externalSystem->retrieveScore(
      studentId, task->getSubject()->getCode(), task->getTitle());

  return score > 0;
}

std::string UniversityGradingSystemAdapter::getGradeRepresentation(int score) {
  // Convert score to 0-10 scale for external system
  double externalScore = score / 10.0;

  // Get letter grade from external system
  return externalSystem->getLetterGrade(externalScore);
}

// Online Platform Grading System Adapter
void OnlinePlatformGradingSystemAdapter::submitTask(
    std::shared_ptr<Task> task, const std::string &studentId,
    const std::string &submissionContent) {

  if (!task || !task->getSubject()) {
    throw std::runtime_error("Task or subject not set properly");
  }

  // Adapt to external system interface
  // Map subject code to moduleId and task title to taskId
  externalSystem->uploadLab(
      studentId,
      task->getSubject()->getCode(), // Using subject code as moduleId
      task->getTitle(),              // Using title as taskId
      submissionContent);
}

int OnlinePlatformGradingSystemAdapter::getScore(std::shared_ptr<Task> task,
                                                 const std::string &studentId) {

  if (!task || !task->getSubject()) {
    return 0;
  }

  // Adapt to external system interface
  int points = externalSystem->getPoints(
      studentId,
      task->getSubject()->getCode(), // Using subject code as moduleId
      task->getTitle()               // Using title as taskId
  );

  // Convert to percentage based on max points
  int maxPoints = externalSystem->getMaxPoints(task->getSubject()->getCode(),
                                               task->getTitle());

  return maxPoints > 0 ? (points * 100) / maxPoints : 0;
}

bool OnlinePlatformGradingSystemAdapter::isCompleted(
    std::shared_ptr<Task> task, const std::string &studentId) {

  if (!task || !task->getSubject()) {
    return false;
  }

  // Direct mapping to external system's completion status
  return externalSystem->taskCompleted(
      studentId,
      task->getSubject()->getCode(), // Using subject code as moduleId
      task->getTitle()               // Using title as taskId
  );
}

std::string
OnlinePlatformGradingSystemAdapter::getGradeRepresentation(int score) {
  // Implement custom grading scale for the online platform
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

// GradingSystemFacade implementation
GradingSystemFacade::GradingSystemFacade(const std::string &studentId)
    : studentId(studentId), activeSystem("") {}

void GradingSystemFacade::registerGradingSystem(
    const std::string &name, std::shared_ptr<IGradingSystem> system) {

  gradingSystems[name] = system;

  // Set as active if it's the first system registered
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

  // Use the active grading system
  gradingSystems[activeSystem]->submitTask(task, studentId, submissionContent);
}

int GradingSystemFacade::getScore(std::shared_ptr<Task> task) {
  if (activeSystem.empty() ||
      gradingSystems.find(activeSystem) == gradingSystems.end()) {
    return 0;
  }

  // Use the active grading system
  return gradingSystems[activeSystem]->getScore(task, studentId);
}

bool GradingSystemFacade::isCompleted(std::shared_ptr<Task> task) {
  if (activeSystem.empty() ||
      gradingSystems.find(activeSystem) == gradingSystems.end()) {
    return false;
  }

  // Use the active grading system
  return gradingSystems[activeSystem]->isCompleted(task, studentId);
}

std::string GradingSystemFacade::getGradeRepresentation(int score) {
  if (activeSystem.empty() ||
      gradingSystems.find(activeSystem) == gradingSystems.end()) {
    // Default grade representation
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

  // Use the active grading system
  return gradingSystems[activeSystem]->getGradeRepresentation(score);
}

std::string GradingSystemFacade::getActiveSystemName() const {
  return activeSystem.empty()
             ? "None"
             : gradingSystems.at(activeSystem)->getSystemName();
}
