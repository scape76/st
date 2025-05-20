#ifndef MOCK_EXTERNAL_SYSTEMS_H
#define MOCK_EXTERNAL_SYSTEMS_H

#include "GradingSystemAdapter.h"
#include <string>
#include <map>
#include <unordered_map>
#include <iostream>

// Mock implementation of the university grading system
class MockUniversityGradingSystem : public ExternalUniversityGradingSystem {
private:
    // Data structure to store submissions: studentId -> courseCode -> assignmentName -> content
    std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> submissions;
    
    // Data structure to store scores: studentId -> courseCode -> assignmentName -> score
    std::map<std::string, std::map<std::string, std::map<std::string, double>>> scores;
    
    // Data structure to store submission timestamps
    std::map<std::string, std::map<std::string, std::map<std::string, std::time_t>>> submissionTimes;
    
    // Deadlines for assignments: courseCode -> assignmentName -> deadline
    std::map<std::string, std::map<std::string, std::time_t>> deadlines;

public:
    MockUniversityGradingSystem() {
        // Set some example deadlines
        std::time_t now = std::time(nullptr);
        deadlines["CS101"]["Homework 1"] = now + 86400; // 1 day from now
        deadlines["CS101"]["Project"] = now + 604800;   // 1 week from now
        deadlines["MATH202"]["Assignment 1"] = now + 259200; // 3 days from now
    }
    
    void submitAssignment(const std::string& studentId, 
                        const std::string& courseCode, 
                        const std::string& assignmentName,
                        const std::string& submissionContent) override {
        // Store the submission
        submissions[studentId][courseCode][assignmentName] = submissionContent;
        
        // Record submission time
        submissionTimes[studentId][courseCode][assignmentName] = std::time(nullptr);
        
        // Auto-grade based on content length (just a demo)
        double grade = std::min(10.0, submissionContent.length() / 100.0);
        scores[studentId][courseCode][assignmentName] = grade;
        
        std::cout << "University system: Assignment submitted for " << studentId 
                  << " in " << courseCode << ": " << assignmentName << std::endl;
    }
    
    double retrieveScore(const std::string& studentId, 
                       const std::string& courseCode, 
                       const std::string& assignmentName) override {
        if (scores.count(studentId) && 
            scores[studentId].count(courseCode) && 
            scores[studentId][courseCode].count(assignmentName)) {
            return scores[studentId][courseCode][assignmentName];
        }
        return 0.0;
    }
    
    std::string getLetterGrade(double score) override {
        if (score >= 9.0) return "A";
        if (score >= 8.0) return "B";
        if (score >= 7.0) return "C";
        if (score >= 6.0) return "D";
        return "F";
    }
    
    bool isSubmissionLate(const std::string& studentId, 
                        const std::string& courseCode, 
                        const std::string& assignmentName) override {
        // Check if submission exists
        if (!submissionTimes.count(studentId) || 
            !submissionTimes[studentId].count(courseCode) || 
            !submissionTimes[studentId][courseCode].count(assignmentName)) {
            return true; // No submission = late
        }
        
        // Check if deadline exists
        if (!deadlines.count(courseCode) || 
            !deadlines[courseCode].count(assignmentName)) {
            return false; // No deadline = not late
        }
        
        // Compare submission time with deadline
        std::time_t submissionTime = submissionTimes[studentId][courseCode][assignmentName];
        std::time_t deadline = deadlines[courseCode][assignmentName];
        
        return submissionTime > deadline;
    }
};

// Mock implementation of the online platform grading system
class MockOnlinePlatformGradingSystem : public ExternalOnlinePlatformGradingSystem {
private:
    // Data structure to store homework: userId -> moduleId -> taskId -> content
    std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> homeworks;
    
    // Data structure to store points: userId -> moduleId -> taskId -> points
    std::map<std::string, std::map<std::string, std::map<std::string, int>>> points;
    
    // Data structure to store max points: moduleId -> taskId -> maxPoints
    std::map<std::string, std::map<std::string, int>> maxPoints;

public:
    MockOnlinePlatformGradingSystem() {
        // Set some example max points
        maxPoints["CS101"]["Lab 1"] = 100;
        maxPoints["CS101"]["Quiz 1"] = 50;
        maxPoints["MATH202"]["Problem Set 1"] = 75;
    }
    
    int uploadHomework(const std::string& userId, 
                      const std::string& moduleId, 
                      const std::string& taskId, 
                      const std::string& fileContent) override {
        // Store the homework
        homeworks[userId][moduleId][taskId] = fileContent;
        
        // Auto-grade based on content length (just a demo)
        int earnedPoints = std::min(getMaxPoints(moduleId, taskId), 
                                  static_cast<int>(fileContent.length()) / 2);
        points[userId][moduleId][taskId] = earnedPoints;
        
        std::cout << "Online platform: Homework uploaded for " << userId 
                  << " in " << moduleId << ": " << taskId << std::endl;
        
        return earnedPoints;
    }
    
    int getPoints(const std::string& userId, 
                const std::string& moduleId, 
                const std::string& taskId) override {
        if (points.count(userId) && 
            points[userId].count(moduleId) && 
            points[userId][moduleId].count(taskId)) {
            return points[userId][moduleId][taskId];
        }
        return 0;
    }
    
    bool taskCompleted(const std::string& userId, 
                     const std::string& moduleId, 
                     const std::string& taskId) override {
        return homeworks.count(userId) && 
               homeworks[userId].count(moduleId) && 
               homeworks[userId][moduleId].count(taskId);
    }
    
    int getMaxPoints(const std::string& moduleId, 
                   const std::string& taskId) override {
        if (maxPoints.count(moduleId) && maxPoints[moduleId].count(taskId)) {
            return maxPoints[moduleId][taskId];
        }
        // Default max points
        return 100;
    }
};

#endif // MOCK_EXTERNAL_SYSTEMS_H