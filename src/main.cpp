#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../include/CachingHtmlProviderProxy.h"
#include "../include/HtmlProvider.h"
#include "../include/Notification.h"
#include "../include/Observer.h"
#include "../include/PerformanceStrategy.h"
#include "../include/ProfilePerformanceCalculator.h"
#include "../include/Registry.h"
#include "../include/Subject.h"
#include "../include/Task.h"
#include "../include/TaskBuilder.h"
#include "Internship.h"
#include "ShortcodeExpanderDecorator.h"

void populateRegistry() {
  auto &registry = Registry::instance();

  registry.internships["internship_1"] = std::make_shared<Internship>(
      "internship_1", "Google", "Software Engineer Intern",
      InternshipStatus::STARTED, "2024-06-01", "2024-08-31");

  SubjectBuilder subjectBuilder;

  auto math = subjectBuilder.setName("Mathematics")
                  .setCode("MATH101")
                  .setDescription("Fundamental concepts of mathematics.")
                  .build();

  auto cs = subjectBuilder.setName("Computer Science")
                .setCode("CS101")
                .setDescription("Introduction to programming and algorithms.")
                .build();

  auto physics = subjectBuilder.setName("Physics")
                     .setCode("PHYS101")
                     .setDescription("Classical mechanics and thermodynamics.")
                     .build();

  TaskBuilder taskBuilder;

  LabFactory labFactory;
  ProjectFactory projectFactory;
  ExamFactory examFactory;

  auto now = std::chrono::system_clock::now();
  DateTime deadline_past = now - std::chrono::hours(72);
  DateTime deadline_soon = now + std::chrono::hours(24 * 7);
  DateTime deadline_future = now + std::chrono::hours(24 * 30);

  auto math_lab1 = labFactory.createTask("Lab 1: Algebra", deadline_past,
                                         "Basic algebraic manipulations");
  math_lab1->setCompleted(true);
  math_lab1->setMarks(90);
  math_lab1->setProgress(100.0f);
  math_lab1->setSubject(math);
  math->addTask(math_lab1);

  auto math_exam = examFactory.createTask("Midterm Exam", deadline_soon,
                                          "Covers first half of the course");
  math_exam->setCompleted(false);
  math_exam->setMarks(0);
  math_exam->setProgress(30.0f);
  math_exam->setSubject(math);
  math->addTask(math_exam);

  auto cs_project = projectFactory.createTask(
      "Project: Web App", deadline_future, "Develop a simple web application");
  cs_project->setCompleted(false);
  cs_project->setMarks(0);
  cs_project->setProgress(15.0f);
  cs_project->setSubject(cs);
  cs->addTask(cs_project);

  auto cs_lab1 = labFactory.createTask("Lab 1: Python Basics", deadline_past,
                                       "Introduction to Python syntax");
  cs_lab1->setCompleted(true);
  cs_lab1->setMarks(95);
  cs_lab1->setProgress(100.0f);
  cs_lab1->setSubject(cs);
  cs->addTask(cs_lab1);

  auto cs_lab2 = labFactory.createTask("Lab 2: Data Structures", deadline_soon,
                                       "Implement lists and dictionaries");
  cs_lab2->setCompleted(true);
  cs_lab2->setMarks(88);
  cs_lab2->setProgress(100.0f);
  cs_lab2->setSubject(cs);
  cs->addTask(cs_lab2);

  auto physics_lab = labFactory.createTask("Lab: Kinematics", deadline_soon,
                                           "Experiments on motion");
  physics_lab->setCompleted(false);
  physics_lab->setProgress(50.0f);
  physics_lab->setSubject(physics);
  physics->addTask(physics_lab);

  auto physics_exam = examFactory.createTask(
      "Final Exam", deadline_future, "Comprehensive exam on all topics");
  physics_exam->setCompleted(false);
  physics_exam->setProgress(5.0f);
  physics_exam->setSubject(physics);
  physics->addTask(physics_exam);

  registry.subjects[math->getCode()] = math;
  registry.subjects[cs->getCode()] = cs;
  registry.subjects[physics->getCode()] = physics;
}

void adapters();
void performancePatterns();

int main() {
  populateRegistry();
  performancePatterns();

  return 0;
}

void adapters() {
  std::unique_ptr<HtmlProvider> baseMarkdownProvider =
      std::make_unique<MarkdownAdapter>();

  std::unique_ptr<HtmlProvider> asciidocProvider =
      std::make_unique<AsciiDocAdapter>();

  std::unique_ptr<CachingHtmlProviderProxy> markdownProviderWithCache =
      std::make_unique<CachingHtmlProviderProxy>(
          std::move(baseMarkdownProvider));

  std::unique_ptr<ShortcodeExpanderDecorator> markdownProvider =
      std::make_unique<ShortcodeExpanderDecorator>(
          std::move(markdownProviderWithCache));

  std::string markdown = "# Heading 1\n"
                         "A reference to internship: **[internship_1]**";

  std::cout << "For markdown: \n";

  std::cout << markdownProvider->getHtml(markdown) << std::endl;

  std::string asciidoc =
      "= My Document Title\n"
      "\n"
      "This is an AsciiDoc paragraph with *bold* and _italic_.\n"
      "\n"
      "== Section One\n"
      "Text under section one.";

  std::cout << "For asciidoc: \n";

  std::cout << asciidocProvider->getHtml(asciidoc) << std::endl;
}

void performancePatterns() {
  std::cout << "\n--- Testing Performance Patterns using "
               "ProfilePerformanceCalculator ---"
            << std::endl;
  auto &registry = Registry::instance();

  if (registry.subjects.empty()) {
    std::cout << "Registry is empty. Cannot run performance tests."
              << std::endl;
    return;
  }

  std::cout << std::fixed << std::setprecision(2);

  std::cout << "\n=== Performance Calculation for Each Subject ==="
            << std::endl;

  for (const auto &pair : registry.subjects) {
    auto subject = pair.second;
    if (subject) {
      std::cout << "\nSubject: " << subject->getName() << " ("
                << subject->getCode() << ")" << std::endl;

      ProfilePerformanceCalculator avgCalc(
          std::make_unique<AverageMarksStrategy>());
      double avgMarks = subject->accept(avgCalc);
      std::cout << "  Average Marks: " << avgMarks << std::endl;

      ProfilePerformanceCalculator completionCalc(
          std::make_unique<CompletionRateStrategy>());
      double completionRate = subject->accept(completionCalc);
      std::cout << "  Completion Rate: " << completionRate << "%" << std::endl;

      ProfilePerformanceCalculator progressCalc(
          std::make_unique<AverageProgressStrategy>());
      double avgProgress = subject->accept(progressCalc);
      std::cout << "  Average Progress: " << avgProgress << "%" << std::endl;
    }
  }

  std::cout << "\n=== Overall Performance Calculation for Registry ==="
            << std::endl;
  std::cout << "(Registry score is the average of individual subject scores "
               "using the specified strategy)"
            << std::endl;

  ProfilePerformanceCalculator registryAvgCalc(
      std::make_unique<AverageMarksStrategy>());
  double overallAvgMarks = registry.accept(registryAvgCalc);
  std::cout << "Overall Average Subject Marks (Registry): " << overallAvgMarks
            << std::endl;

  ProfilePerformanceCalculator registryCompletionCalc(
      std::make_unique<CompletionRateStrategy>());
  double overallCompletionRate = registry.accept(registryCompletionCalc);
  std::cout << "Overall Average Subject Completion Rate (Registry): "
            << overallCompletionRate << "%" << std::endl;

  ProfilePerformanceCalculator registryProgressCalc(
      std::make_unique<AverageProgressStrategy>());
  double overallAvgProgress = registry.accept(registryProgressCalc);
  std::cout << "Overall Average Subject Progress (Registry): "
            << overallAvgProgress << "%" << std::endl;

  std::cout << "\n--- End of Performance Patterns Test ---" << std::endl;
}
