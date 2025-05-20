#include "Assignment.h"
#include "Exceptions.h" // For AssignmentAlreadyCompletedError

Assignment::Assignment(int id, std::string name, int subjectId,
                       std::string deadline)
    : assignmentId(id), name(std::move(name)), subjectId(subjectId),
      deadline(std::move(deadline)), status(AssignmentStatus::PENDING),
      grade(std::nullopt) {}

int Assignment::getAssignmentId() const { return assignmentId; }
const std::string &Assignment::getName() const { return name; }
int Assignment::getSubjectId() const { return subjectId; }
const std::string &Assignment::getDeadline() const { return deadline; }
AssignmentStatus Assignment::getStatus() const { return status; }
const std::optional<int> &Assignment::getGrade() const { return grade; }

void Assignment::markCompleted(std::optional<int> newGrade) {
  if (status == AssignmentStatus::COMPLETED) {
    throw AssignmentAlreadyCompletedError("Assignment " +
                                          std::to_string(assignmentId) +
                                          " is already completed.");
  }
  status = AssignmentStatus::COMPLETED;
  if (newGrade.has_value()) {
    grade = newGrade;
  }
}

void Assignment::setStatus(AssignmentStatus newStatus) { status = newStatus; }
