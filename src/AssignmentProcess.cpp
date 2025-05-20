#include "AssignmentProcess.h"
#include <iostream>
#include <random>
#include <ctime>

// HomeworkProcess implementation
void HomeworkProcess::validateAssignment(std::shared_ptr<Assignment> assignment) {
    std::cout << "Validating homework assignment: " << assignment->getTitle() << std::endl;
    
    if (assignment->getTitle().empty()) {
        throw std::runtime_error("Assignment title cannot be empty");
    }
    
    if (auto hwAssignment = std::dynamic_pointer_cast<HomeworkAssignment>(assignment)) {
        std::cout << "Valid homework assignment confirmed." << std::endl;
    } else {
        std::cout << "Warning: Processing non-homework assignment with homework process." << std::endl;
    }
}

bool HomeworkProcess::checkPrerequisites(std::shared_ptr<Assignment> assignment) {
    std::cout << "Checking prerequisites for homework assignment: " << assignment->getTitle() << std::endl;
    
    // For homework, we just check if it's overdue
    auto now = std::chrono::system_clock::now();
    bool isOverdue = (now > assignment->getDeadline());
    
    if (isOverdue) {
        std::cout << "This assignment is past its deadline. Late submission policy applies." << std::endl;
    } else {
        std::cout << "Assignment is being submitted before the deadline." << std::endl;
    }
    
    // We allow late submissions for homework, but with penalties
    return true;
}

void HomeworkProcess::processSubmission(std::shared_ptr<Assignment> assignment, const std::string& submission) {
    std::cout << "Processing homework submission for: " << assignment->getTitle() << std::endl;
    std::cout << "Submission content length: " << submission.length() << " characters" << std::endl;
    
    // Simulating submission processing
    std::cout << "Checking for plagiarism..." << std::endl;
    std::cout << "Saving submission to database..." << std::endl;
    
    // Update assignment progress to 100%
    assignment->setProgress(100.0f);
}

int HomeworkProcess::evaluateAssignment(std::shared_ptr<Assignment> assignment) {
    std::cout << "Evaluating homework: " << assignment->getTitle() << std::endl;
    
    // Simulating automatic grading based on length of title (just for demo)
    // A real implementation would have actual grading logic
    int baseScore = 70; // Base score for homework
    
    // Random component for demo
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 30);
    int randomScore = dis(gen);
    
    int finalScore = baseScore + randomScore;
    if (finalScore > 100) finalScore = 100;
    
    std::cout << "Homework evaluated with score: " << finalScore << "/100" << std::endl;
    return finalScore;
}

void HomeworkProcess::provideFeedback(std::shared_ptr<Assignment> assignment, int score) {
    std::cout << "Providing feedback for homework: " << assignment->getTitle() << std::endl;
    
    if (score >= 90) {
        std::cout << "Excellent work! Very thorough understanding demonstrated." << std::endl;
    } else if (score >= 80) {
        std::cout << "Good work. Shows solid understanding of concepts." << std::endl;
    } else if (score >= 70) {
        std::cout << "Satisfactory work. Some areas could be improved." << std::endl;
    } else if (score >= 60) {
        std::cout << "Passing, but needs improvement in several areas." << std::endl;
    } else {
        std::cout << "Not satisfactory. Please review the material and consider resubmitting." << std::endl;
    }
}

void HomeworkProcess::updateRecords(std::shared_ptr<Assignment> assignment, int score) {
    std::cout << "Updating records for homework: " << assignment->getTitle() << std::endl;
    
    // Update the assignment
    assignment->setCompleted(true);
    assignment->setMarks(score);
    std::cout << "Assignment status updated: Completed with " << score << " marks" << std::endl;
}

// ProjectProcess implementation
void ProjectProcess::validateAssignment(std::shared_ptr<Assignment> assignment) {
    std::cout << "Validating project assignment: " << assignment->getTitle() << std::endl;
    
    if (assignment->getTitle().empty()) {
        throw std::runtime_error("Project title cannot be empty");
    }
    
    if (auto projAssignment = std::dynamic_pointer_cast<ProjectAssignment>(assignment)) {
        std::cout << "Valid project assignment confirmed." << std::endl;
    } else {
        std::cout << "Warning: Processing non-project assignment with project process." << std::endl;
    }
}

bool ProjectProcess::checkPrerequisites(std::shared_ptr<Assignment> assignment) {
    std::cout << "Checking prerequisites for project: " << assignment->getTitle() << std::endl;
    
    // For projects, check if it's not too overdue (within 3 days)
    auto now = std::chrono::system_clock::now();
    auto deadline = assignment->getDeadline();
    auto daysLate = std::chrono::duration_cast<std::chrono::hours>(now - deadline).count() / 24;
    
    bool isAcceptable = (now <= deadline || daysLate <= 3);
    
    if (now > deadline) {
        std::cout << "This project is " << daysLate << " days late." << std::endl;
        if (isAcceptable) {
            std::cout << "Late submission is still acceptable (within 3-day grace period)." << std::endl;
        } else {
            std::cout << "Project is too late for submission (beyond 3-day grace period)." << std::endl;
        }
    } else {
        std::cout << "Project is being submitted before the deadline." << std::endl;
    }
    
    return isAcceptable;
}

void ProjectProcess::processSubmission(std::shared_ptr<Assignment> assignment, const std::string& submission) {
    std::cout << "Processing project submission for: " << assignment->getTitle() << std::endl;
    std::cout << "Project documentation length: " << submission.length() << " characters" << std::endl;
    
    // Simulating project submission processing
    std::cout << "Validating project structure..." << std::endl;
    std::cout << "Checking for required components..." << std::endl;
    std::cout << "Running automated tests..." << std::endl;
    std::cout << "Archiving project files..." << std::endl;
    
    // Update assignment progress to 100%
    assignment->setProgress(100.0f);
}

int ProjectProcess::evaluateAssignment(std::shared_ptr<Assignment> assignment) {
    std::cout << "Evaluating project: " << assignment->getTitle() << std::endl;
    
    // Projects typically have a higher base score due to their complexity
    int baseScore = 75;
    
    // Random component for demo
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 25);
    int randomScore = dis(gen);
    
    int finalScore = baseScore + randomScore;
    if (finalScore > 100) finalScore = 100;
    
    // Late penalty if applicable
    auto now = std::chrono::system_clock::now();
    if (now > assignment->getDeadline()) {
        auto daysLate = std::chrono::duration_cast<std::chrono::hours>(now - assignment->getDeadline()).count() / 24;
        int penalty = daysLate * 5; // 5 points per day late
        finalScore = std::max(0, finalScore - penalty);
        std::cout << "Applied late penalty of " << penalty << " points." << std::endl;
    }
    
    std::cout << "Project evaluated with score: " << finalScore << "/100" << std::endl;
    return finalScore;
}

void ProjectProcess::provideFeedback(std::shared_ptr<Assignment> assignment, int score) {
    std::cout << "Providing feedback for project: " << assignment->getTitle() << std::endl;
    
    if (score >= 90) {
        std::cout << "Outstanding project! Professional quality work with excellent attention to detail." << std::endl;
    } else if (score >= 80) {
        std::cout << "Very good project. Well structured and implemented with minor areas for improvement." << std::endl;
    } else if (score >= 70) {
        std::cout << "Good project that meets requirements. Some design and implementation aspects could be enhanced." << std::endl;
    } else if (score >= 60) {
        std::cout << "Project meets minimum requirements but needs significant improvements in design and implementation." << std::endl;
    } else {
        std::cout << "Project does not meet minimum requirements. Major revisions required." << std::endl;
    }
}

void ProjectProcess::updateRecords(std::shared_ptr<Assignment> assignment, int score) {
    std::cout << "Updating records for project: " << assignment->getTitle() << std::endl;
    
    // Update the assignment
    assignment->setCompleted(true);
    assignment->setMarks(score);
    std::cout << "Project status updated: Completed with " << score << " marks" << std::endl;
    
    // For projects, we might also update portfolio or showcase information
    std::cout << "Adding project to student portfolio..." << std::endl;
}

void ProjectProcess::beforeSubmission(std::shared_ptr<Assignment> assignment) {
    // Override the hook method to add project-specific pre-processing
    std::cout << "=== PROJECT SUBMISSION PROCESS STARTED ===" << std::endl;
    std::cout << "Running pre-submission checks for project: " << assignment->getTitle() << std::endl;
    std::cout << "Checking for required project components..." << std::endl;
    std::cout << "Verifying project structure..." << std::endl;
}

// ExamProcess implementation
void ExamProcess::validateAssignment(std::shared_ptr<Assignment> assignment) {
    std::cout << "Validating exam assignment: " << assignment->getTitle() << std::endl;
    
    if (assignment->getTitle().empty()) {
        throw std::runtime_error("Exam title cannot be empty");
    }
    
    if (auto examAssignment = std::dynamic_pointer_cast<ExamAssignment>(assignment)) {
        std::cout << "Valid exam assignment confirmed." << std::endl;
    } else {
        std::cout << "Warning: Processing non-exam assignment with exam process." << std::endl;
    }
}

bool ExamProcess::checkPrerequisites(std::shared_ptr<Assignment> assignment) {
    std::cout << "Checking prerequisites for exam: " << assignment->getTitle() << std::endl;
    
    // For exams, usually no late submissions are allowed at all
    auto now = std::chrono::system_clock::now();
    bool isOnTime = (now <= assignment->getDeadline());
    
    if (!isOnTime) {
        std::cout << "Error: This exam is past its deadline. No late submissions allowed for exams." << std::endl;
    } else {
        std::cout << "Exam is being submitted within the allotted time." << std::endl;
    }
    
    return isOnTime;
}

void ExamProcess::processSubmission(std::shared_ptr<Assignment> assignment, const std::string& submission) {
    std::cout << "Processing exam submission for: " << assignment->getTitle() << std::endl;
    std::cout << "Submission length: " << submission.length() << " characters" << std::endl;
    
    // Simulating exam processing steps
    std::cout << "Logging submission time..." << std::endl;
    std::cout << "Verifying exam integrity..." << std::endl;
    std::cout << "Preparing for blind grading..." << std::endl;
    
    // Update assignment progress to 100%
    assignment->setProgress(100.0f);
}

int ExamProcess::evaluateAssignment(std::shared_ptr<Assignment> assignment) {
    std::cout << "Evaluating exam: " << assignment->getTitle() << std::endl;
    
    // Simulating rigorous exam grading
    // A real implementation would have actual grading logic
    
    // Random component for demo
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(40, 100);
    int finalScore = dis(gen);
    
    std::cout << "Exam evaluated with score: " << finalScore << "/100" << std::endl;
    return finalScore;
}

void ExamProcess::provideFeedback(std::shared_ptr<Assignment> assignment, int score) {
    std::cout << "Providing feedback for exam: " << assignment->getTitle() << std::endl;
    
    // Exams often have different feedback mechanics
    std::cout << "Score breakdown by section:" << std::endl;
    
    // Simulating section scores (would be pulled from actual grading in a real system)
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> section1(0, 25);
    std::uniform_int_distribution<> section2(0, 35);
    std::uniform_int_distribution<> section3(0, 40);
    
    int s1 = section1(gen);
    int s2 = section2(gen);
    int s3 = section3(gen);
    
    // Adjust to match the given total score
    float scaleFactor = static_cast<float>(score) / (s1 + s2 + s3);
    s1 = static_cast<int>(s1 * scaleFactor);
    s2 = static_cast<int>(s2 * scaleFactor);
    s3 = score - s1 - s2; // Ensure all sections add up to the total score
    
    std::cout << "Section 1 (Multiple Choice): " << s1 << "/25" << std::endl;
    std::cout << "Section 2 (Short Answer): " << s2 << "/35" << std::endl;
    std::cout << "Section 3 (Essay Questions): " << s3 << "/40" << std::endl;
    std::cout << "Total: " << score << "/100" << std::endl;
    
    // General feedback based on score
    if (score >= 90) {
        std::cout << "Excellent understanding of all concepts!" << std::endl;
    } else if (score >= 80) {
        std::cout << "Very good grasp of most concepts." << std::endl;
    } else if (score >= 70) {
        std::cout << "Good understanding with some areas to review." << std::endl;
    } else if (score >= 60) {
        std::cout << "Basic understanding demonstrated. Need to focus on several key concepts." << std::endl;
    } else {
        std::cout << "Did not demonstrate sufficient understanding. Please schedule a review session." << std::endl;
    }
}

void ExamProcess::updateRecords(std::shared_ptr<Assignment> assignment, int score) {
    std::cout << "Updating records for exam: " << assignment->getTitle() << std::endl;
    
    // Update the assignment
    assignment->setCompleted(true);
    assignment->setMarks(score);
    std::cout << "Exam status updated: Completed with " << score << " marks" << std::endl;
    
    // For exams, additional record updates might be required
    std::cout << "Updating academic transcript..." << std::endl;
    std::cout << "Checking if this affects course completion status..." << std::endl;
}

void ExamProcess::afterCompletion(std::shared_ptr<Assignment> assignment) {
    // Override the hook method to add exam-specific post-processing
    std::cout << "Exam processing completed for: " << assignment->getTitle() << std::endl;
    std::cout << "Generating performance analytics..." << std::endl;
    std::cout << "Scheduling review session if needed..." << std::endl;
    std::cout << "=== EXAM SUBMISSION PROCESS COMPLETED ===" << std::endl;
}