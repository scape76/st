#include <iostream>
#include <string>

class Assignment {
  std::string name;
  std::string description;
};

class AssignmentBuilder {
  void setName(std::string name);
  void setDescription(std::string description);
}
