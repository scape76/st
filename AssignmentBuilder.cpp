#include "AssignmentBuilder.h"
#include "Exceptions.h"
#include "IdGenerator.h"

AssignmentBuilder::AssignmentBuilder(int subjectId) : p_subjectId(subjectId) {}

AssignmentBuilder &AssignmentBuilder::withName(const std::string &name) {
  if (name.empty()) {
    throw ValidationError("Assignment name cannot be empty.");
  }
  p_name = name;
  return *this;
}

AssignmentBuilder &
AssignmentBuilder::withDeadline(const std::string &deadline) {
  // Basic validation, could be more sophisticated (e.g., date format)
  if (deadline.empty()) {
    throw ValidationError("Assignment deadline cannot be empty.");
  }
  p_deadline = deadline;
  return *this;
}

std::unique_ptr<Assignment> AssignmentBuilder::build() {
  if (p_name.empty() || p_deadline.empty()) {
    throw ValidationError(
        "Assignment name and deadline must be set before building.");
  }
  int newId = IdGenerator::getInstance().generateAssignmentId();
  // Use placement new or pass parameters to a public constructor if you prefer
  // For simplicity, Assignment's constructor is made accessible to
  // AssignmentBuilder via friend class
  return std::make_unique<Assignment>(newId, p_name, p_subjectId, p_deadline);
}
