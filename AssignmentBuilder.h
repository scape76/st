#ifndef ASSIGNMENTBUILDER_H
#define ASSIGNMENTBUILDER_H

#include "Assignment.h"
#include <memory>
#include <string>

class AssignmentBuilder {
public:
  AssignmentBuilder(int subjectId);

  AssignmentBuilder &withName(const std::string &name);
  AssignmentBuilder &withDeadline(const std::string &deadline);
  std::unique_ptr<Assignment> build();

private:
  int p_subjectId;
  std::string p_name;
  std::string p_deadline;
};

#endif // ASSIGNMENTBUILDER_H
