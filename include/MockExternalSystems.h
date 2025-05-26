#ifndef MOCK_EXTERNAL_SYSTEMS_H
#define MOCK_EXTERNAL_SYSTEMS_H

#include "GradingSystemAdapter.h"
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

class MockUniversityGradingSystem : public ExternalUniversityGradingSystem {
private:
  std::map<std::string,
           std::map<std::string, std::map<std::string, std::string>>>
      submissions;

  std::map<std::string, std::map<std::string, std::map<std::string, double>>>
      scores;

  std::map<std::string,
           std::map<std::string, std::map<std::string, std::time_t>>>
      submissionTimes;

  std::map<std::string, std::map<std::string, std::time_t>> deadlines;

public:
  MockUniversityGradingSystem() {
    std::time_t now = std::time(nullptr);
    deadlines["CS101"]["Lab 1"] = now + 86400;
    deadlines["CS101"]["Project"] = now + 604800;
    deadlines["MATH202"]["Task 1"] = now + 259200;
  }

  void submitTask(const std::string &studentId, const std::string &courseCode,
                  const std::string &taskName,
                  const std::string &submissionContent) override {
    submissions[studentId][courseCode][taskName] = submissionContent;

    submissionTimes[studentId][courseCode][taskName] = std::time(nullptr);

    double grade = std::min(10.0, submissionContent.length() / 100.0);
    scores[studentId][courseCode][taskName] = grade;

    std::cout << "University system: Task submitted for " << studentId << " in "
              << courseCode << ": " << taskName << std::endl;
  }

  double retrieveScore(const std::string &studentId,
                       const std::string &courseCode,
                       const std::string &taskName) override {
    if (scores.count(studentId) && scores[studentId].count(courseCode) &&
        scores[studentId][courseCode].count(taskName)) {
      return scores[studentId][courseCode][taskName];
    }
    return 0.0;
  }

  std::string getLetterGrade(double score) override {
    if (score >= 9.0)
      return "A";
    if (score >= 8.0)
      return "B";
    if (score >= 7.0)
      return "C";
    if (score >= 6.0)
      return "D";
    return "F";
  }

  bool isSubmissionLate(const std::string &studentId,
                        const std::string &courseCode,
                        const std::string &taskName) override {
    if (!submissionTimes.count(studentId) ||
        !submissionTimes[studentId].count(courseCode) ||
        !submissionTimes[studentId][courseCode].count(taskName)) {
      return true;
    }

    if (!deadlines.count(courseCode) ||
        !deadlines[courseCode].count(taskName)) {
      return false;
    }

    std::time_t submissionTime =
        submissionTimes[studentId][courseCode][taskName];
    std::time_t deadline = deadlines[courseCode][taskName];

    return submissionTime > deadline;
  }
};

class MockOnlinePlatformGradingSystem
    : public ExternalOnlinePlatformGradingSystem {
private:
  std::map<std::string,
           std::map<std::string, std::map<std::string, std::string>>>
      labs;

  std::map<std::string, std::map<std::string, std::map<std::string, int>>>
      points;

  std::map<std::string, std::map<std::string, int>> maxPoints;

public:
  MockOnlinePlatformGradingSystem() {
    maxPoints["CS101"]["Lab 1"] = 100;
    maxPoints["CS101"]["Quiz 1"] = 50;
    maxPoints["MATH202"]["Problem Set 1"] = 75;
  }

  int uploadLab(const std::string &userId, const std::string &moduleId,
                const std::string &taskId,
                const std::string &fileContent) override {
    labs[userId][moduleId][taskId] = fileContent;

    int earnedPoints = std::min(getMaxPoints(moduleId, taskId),
                                static_cast<int>(fileContent.length()) / 2);
    points[userId][moduleId][taskId] = earnedPoints;

    std::cout << "Online platform: Lab uploaded for " << userId << " in "
              << moduleId << ": " << taskId << std::endl;

    return earnedPoints;
  }

  int getPoints(const std::string &userId, const std::string &moduleId,
                const std::string &taskId) override {
    if (points.count(userId) && points[userId].count(moduleId) &&
        points[userId][moduleId].count(taskId)) {
      return points[userId][moduleId][taskId];
    }
    return 0;
  }

  bool taskCompleted(const std::string &userId, const std::string &moduleId,
                     const std::string &taskId) override {
    return labs.count(userId) && labs[userId].count(moduleId) &&
           labs[userId][moduleId].count(taskId);
  }

  int getMaxPoints(const std::string &moduleId,
                   const std::string &taskId) override {
    if (maxPoints.count(moduleId) && maxPoints[moduleId].count(taskId)) {
      return maxPoints[moduleId][taskId];
    }
    return 100;
  }
};

#endif