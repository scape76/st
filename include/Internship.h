#ifndef INTERNSHIP_H
#define INTERNSHIP_H

#include <iostream>
#include <string>

enum class InternshipStatus { PENDING, STARTED, ENDED, CANCELLED };

inline std::string internshipStatusToString(InternshipStatus status) {
  switch (status) {
  case InternshipStatus::PENDING:
    return "Pending";
  case InternshipStatus::STARTED:
    return "Started";
  case InternshipStatus::ENDED:
    return "Ended";
  case InternshipStatus::CANCELLED:
    return "Cancelled";
  default:
    return "Unknown";
  }
}

struct Internship {
  std::string id;
  std::string company;
  std::string position;
  InternshipStatus status;
  std::string startDate;
  std::string endDate;

  Internship(std::string id, std::string comp, std::string pos,
             InternshipStatus stat, std::string start, std::string end)
      : id(std::move(id)), company(std::move(comp)), position(std::move(pos)),
        status(stat), startDate(std::move(start)), endDate(std::move(end)) {}

  std::string getDetails() const {
    return position + " at " + company +
           " (Status: " + internshipStatusToString(status) +
           ", Start: " + startDate + ", End: " + endDate + ")";
  }
};

#endif // INTERNSHIP_H
