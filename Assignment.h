#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include "Enums.h" // For AssignmentStatus
#include <optional>
#include <string>

class AssignmentBuilder; // Forward declaration

class Assignment {
public:
  friend class AssignmentBuilder; // Allow builder to access private
                                  // constructor/members

  int getAssignmentId() const;
  int getSubjectId() const;
  AssignmentStatus getStatus() const;
  const std::string &getName() const;
  const std::string &getDeadline() const;
  const std::optional<int> &getGrade() const;

  void markCompleted(std::optional<int> grade);
  void setStatus(AssignmentStatus status);

  Assignment(int id, std::string name, int subjectId, std::string deadline);

private:
  int assignmentId;
  int subjectId;
  AssignmentStatus status;
  std::string name;
  std::string deadline;
  std::optional<int> grade;
};

#endif // ASSIGNMENT_H
