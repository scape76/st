#include "../include/TaskProcess.h"
#include <ctime>
#include <iostream>
#include <random>

void LabProcess::validateTask(std::shared_ptr<Task> task) {
  std::cout << "Validating lab task: " << task->getTitle() << std::endl;

  if (task->getTitle().empty()) {
    throw std::runtime_error("Task title cannot be empty");
  }

  if (auto hwTask = std::dynamic_pointer_cast<LabTask>(task)) {
    std::cout << "Valid lab task confirmed." << std::endl;
  } else {
    std::cout << "Warning: Processing non-lab task with lab process."
              << std::endl;
  }
}

bool LabProcess::checkPrerequisites(std::shared_ptr<Task> task) {
  std::cout << "Checking prerequisites for lab task: " << task->getTitle()
            << std::endl;

  auto now = std::chrono::system_clock::now();
  bool isOverdue = (now > task->getDeadline());

  if (isOverdue) {
    std::cout << "This task is past its deadline. Late submission policy "
                 "applies."
              << std::endl;
  } else {
    std::cout << "Task is being submitted before the deadline." << std::endl;
  }

  return true;
}

void LabProcess::processSubmission(std::shared_ptr<Task> task,
                                   const std::string &submission) {
  std::cout << "Processing lab submission for: " << task->getTitle()
            << std::endl;
  std::cout << "Submission content length: " << submission.length()
            << " characters" << std::endl;

  std::cout << "Checking for plagiarism..." << std::endl;
  std::cout << "Saving submission to database..." << std::endl;

  task->setProgress(100.0f);
}

int LabProcess::evaluateTask(std::shared_ptr<Task> task) {
  std::cout << "Evaluating lab: " << task->getTitle() << std::endl;

  int baseScore = 70;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 30);
  int randomScore = dis(gen);

  int finalScore = baseScore + randomScore;
  if (finalScore > 100)
    finalScore = 100;

  std::cout << "Lab evaluated with score: " << finalScore << "/100"
            << std::endl;
  return finalScore;
}

void LabProcess::provideFeedback(std::shared_ptr<Task> task, int score) {
  std::cout << "Providing feedback for lab: " << task->getTitle() << std::endl;

  if (score >= 90) {
    std::cout << "Excellent work! Very thorough understanding demonstrated."
              << std::endl;
  } else if (score >= 80) {
    std::cout << "Good work. Shows solid understanding of concepts."
              << std::endl;
  } else if (score >= 70) {
    std::cout << "Satisfactory work. Some areas could be improved."
              << std::endl;
  } else if (score >= 60) {
    std::cout << "Passing, but needs improvement in several areas."
              << std::endl;
  } else {
    std::cout << "Not satisfactory. Please review the material and consider "
                 "resubmitting."
              << std::endl;
  }
}

void LabProcess::updateRecords(std::shared_ptr<Task> task, int score) {
  std::cout << "Updating records for lab: " << task->getTitle() << std::endl;

  task->setCompleted(true);
  task->setMarks(score);
  std::cout << "Task status updated: Completed with " << score << " marks"
            << std::endl;
}

void ProjectProcess::validateTask(std::shared_ptr<Task> task) {
  std::cout << "Validating project task: " << task->getTitle() << std::endl;

  if (task->getTitle().empty()) {
    throw std::runtime_error("Project title cannot be empty");
  }

  if (auto projTask = std::dynamic_pointer_cast<ProjectTask>(task)) {
    std::cout << "Valid project task confirmed." << std::endl;
  } else {
    std::cout << "Warning: Processing non-project task with project process."
              << std::endl;
  }
}

bool ProjectProcess::checkPrerequisites(std::shared_ptr<Task> task) {
  std::cout << "Checking prerequisites for project: " << task->getTitle()
            << std::endl;

  auto now = std::chrono::system_clock::now();
  auto deadline = task->getDeadline();
  auto daysLate =
      std::chrono::duration_cast<std::chrono::hours>(now - deadline).count() /
      24;

  bool isAcceptable = (now <= deadline || daysLate <= 3);

  if (now > deadline) {
    std::cout << "This project is " << daysLate << " days late." << std::endl;
    if (isAcceptable) {
      std::cout
          << "Late submission is still acceptable (within 3-day grace period)."
          << std::endl;
    } else {
      std::cout
          << "Project is too late for submission (beyond 3-day grace period)."
          << std::endl;
    }
  } else {
    std::cout << "Project is being submitted before the deadline." << std::endl;
  }

  return isAcceptable;
}

void ProjectProcess::processSubmission(std::shared_ptr<Task> task,
                                       const std::string &submission) {
  std::cout << "Processing project submission for: " << task->getTitle()
            << std::endl;
  std::cout << "Project documentation length: " << submission.length()
            << " characters" << std::endl;

  std::cout << "Validating project structure..." << std::endl;
  std::cout << "Checking for required components..." << std::endl;
  std::cout << "Running automated tests..." << std::endl;
  std::cout << "Archiving project files..." << std::endl;

  task->setProgress(100.0f);
}

int ProjectProcess::evaluateTask(std::shared_ptr<Task> task) {
  std::cout << "Evaluating project: " << task->getTitle() << std::endl;

  int baseScore = 75;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 25);
  int randomScore = dis(gen);

  int finalScore = baseScore + randomScore;
  if (finalScore > 100)
    finalScore = 100;

  auto now = std::chrono::system_clock::now();
  if (now > task->getDeadline()) {
    auto daysLate = std::chrono::duration_cast<std::chrono::hours>(
                        now - task->getDeadline())
                        .count() /
                    24;
    int penalty = daysLate * 5; // 5 points per day late
    finalScore = std::max(0, finalScore - penalty);
    std::cout << "Applied late penalty of " << penalty << " points."
              << std::endl;
  }

  std::cout << "Project evaluated with score: " << finalScore << "/100"
            << std::endl;
  return finalScore;
}

void ProjectProcess::provideFeedback(std::shared_ptr<Task> task, int score) {
  std::cout << "Providing feedback for project: " << task->getTitle()
            << std::endl;

  if (score >= 90) {
    std::cout << "Outstanding project! Professional quality work with "
                 "excellent attention to detail."
              << std::endl;
  } else if (score >= 80) {
    std::cout << "Very good project. Well structured and implemented with "
                 "minor areas for improvement."
              << std::endl;
  } else if (score >= 70) {
    std::cout << "Good project that meets requirements. Some design and "
                 "implementation aspects could be enhanced."
              << std::endl;
  } else if (score >= 60) {
    std::cout << "Project meets minimum requirements but needs significant "
                 "improvements in design and implementation."
              << std::endl;
  } else {
    std::cout << "Project does not meet minimum requirements. Major revisions "
                 "required."
              << std::endl;
  }
}

void ProjectProcess::updateRecords(std::shared_ptr<Task> task, int score) {
  std::cout << "Updating records for project: " << task->getTitle()
            << std::endl;

  task->setCompleted(true);
  task->setMarks(score);
  std::cout << "Project status updated: Completed with " << score << " marks"
            << std::endl;

  std::cout << "Adding project to student portfolio..." << std::endl;
}

void ProjectProcess::beforeSubmission(std::shared_ptr<Task> task) {
  std::cout << "=== PROJECT SUBMISSION PROCESS STARTED ===" << std::endl;
  std::cout << "Running pre-submission checks for project: " << task->getTitle()
            << std::endl;
  std::cout << "Checking for required project components..." << std::endl;
  std::cout << "Verifying project structure..." << std::endl;
}

void ExamProcess::validateTask(std::shared_ptr<Task> task) {
  std::cout << "Validating exam task: " << task->getTitle() << std::endl;

  if (task->getTitle().empty()) {
    throw std::runtime_error("Exam title cannot be empty");
  }

  if (auto examTask = std::dynamic_pointer_cast<ExamTask>(task)) {
    std::cout << "Valid exam task confirmed." << std::endl;
  } else {
    std::cout << "Warning: Processing non-exam task with exam process."
              << std::endl;
  }
}

bool ExamProcess::checkPrerequisites(std::shared_ptr<Task> task) {
  std::cout << "Checking prerequisites for exam: " << task->getTitle()
            << std::endl;

  auto now = std::chrono::system_clock::now();
  bool isOnTime = (now <= task->getDeadline());

  if (!isOnTime) {
    std::cout << "Error: This exam is past its deadline. No late submissions "
                 "allowed for exams."
              << std::endl;
  } else {
    std::cout << "Exam is being submitted within the allotted time."
              << std::endl;
  }

  return isOnTime;
}

void ExamProcess::processSubmission(std::shared_ptr<Task> task,
                                    const std::string &submission) {
  std::cout << "Processing exam submission for: " << task->getTitle()
            << std::endl;
  std::cout << "Submission length: " << submission.length() << " characters"
            << std::endl;

  std::cout << "Logging submission time..." << std::endl;
  std::cout << "Verifying exam integrity..." << std::endl;
  std::cout << "Preparing for blind grading..." << std::endl;

  task->setProgress(100.0f);
}

int ExamProcess::evaluateTask(std::shared_ptr<Task> task) {
  std::cout << "Evaluating exam: " << task->getTitle() << std::endl;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(40, 100);
  int finalScore = dis(gen);

  std::cout << "Exam evaluated with score: " << finalScore << "/100"
            << std::endl;
  return finalScore;
}

void ExamProcess::provideFeedback(std::shared_ptr<Task> task, int score) {
  std::cout << "Providing feedback for exam: " << task->getTitle() << std::endl;

  std::cout << "Score breakdown by section:" << std::endl;

  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<> section1(0, 25);
  std::uniform_int_distribution<> section2(0, 35);
  std::uniform_int_distribution<> section3(0, 40);

  int s1 = section1(gen);
  int s2 = section2(gen);
  int s3 = section3(gen);

  float scaleFactor = static_cast<float>(score) / (s1 + s2 + s3);
  s1 = static_cast<int>(s1 * scaleFactor);
  s2 = static_cast<int>(s2 * scaleFactor);
  s3 = score - s1 - s2;

  std::cout << "Section 1 (Multiple Choice): " << s1 << "/25" << std::endl;
  std::cout << "Section 2 (Short Answer): " << s2 << "/35" << std::endl;
  std::cout << "Section 3 (Essay Questions): " << s3 << "/40" << std::endl;
  std::cout << "Total: " << score << "/100" << std::endl;

  if (score >= 90) {
    std::cout << "Excellent understanding of all concepts!" << std::endl;
  } else if (score >= 80) {
    std::cout << "Very good grasp of most concepts." << std::endl;
  } else if (score >= 70) {
    std::cout << "Good understanding with some areas to review." << std::endl;
  } else if (score >= 60) {
    std::cout << "Basic understanding demonstrated. Need to focus on several "
                 "key concepts."
              << std::endl;
  } else {
    std::cout << "Did not demonstrate sufficient understanding. Please "
                 "schedule a review session."
              << std::endl;
  }
}

void ExamProcess::updateRecords(std::shared_ptr<Task> task, int score) {
  std::cout << "Updating records for exam: " << task->getTitle() << std::endl;

  task->setCompleted(true);
  task->setMarks(score);
  std::cout << "Exam status updated: Completed with " << score << " marks"
            << std::endl;

  std::cout << "Updating academic transcript..." << std::endl;
  std::cout << "Checking if this affects course completion status..."
            << std::endl;
}

void ExamProcess::afterCompletion(std::shared_ptr<Task> task) {
  std::cout << "Exam processing completed for: " << task->getTitle()
            << std::endl;
  std::cout << "Generating performance analytics..." << std::endl;
  std::cout << "Scheduling review session if needed..." << std::endl;
  std::cout << "=== EXAM SUBMISSION PROCESS COMPLETED ===" << std::endl;
}
