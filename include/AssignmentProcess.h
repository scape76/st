#ifndef ASSIGNMENT_PROCESS_H
#define ASSIGNMENT_PROCESS_H

#include <string>
#include <memory>
#include <iostream>
#include "Assignment.h"

// Abstract base class defining template methods for assignment processing
class AssignmentProcess {
protected:
    // Steps that can be overridden by subclasses
    virtual void validateAssignment(std::shared_ptr<Assignment> assignment) = 0;
    virtual bool checkPrerequisites(std::shared_ptr<Assignment> assignment) = 0;
    virtual void processSubmission(std::shared_ptr<Assignment> assignment, const std::string& submission) = 0;
    virtual int evaluateAssignment(std::shared_ptr<Assignment> assignment) = 0;
    virtual void provideFeedback(std::shared_ptr<Assignment> assignment, int score) = 0;
    virtual void updateRecords(std::shared_ptr<Assignment> assignment, int score) = 0;
    
    // Hook methods with default implementations (can be overridden)
    virtual void beforeSubmission(std::shared_ptr<Assignment> assignment) {
        std::cout << "Preparing to process assignment: " << assignment->getTitle() << std::endl;
    }
    
    virtual void afterCompletion(std::shared_ptr<Assignment> assignment) {
        std::cout << "Assignment process completed for: " << assignment->getTitle() << std::endl;
    }
    
public:
    virtual ~AssignmentProcess() = default;
    
    // The template method that defines the algorithm structure
    void processAssignment(std::shared_ptr<Assignment> assignment, const std::string& submission) {
        beforeSubmission(assignment);
        
        validateAssignment(assignment);
        
        if (!checkPrerequisites(assignment)) {
            std::cout << "Prerequisites not met for assignment: " << assignment->getTitle() << std::endl;
            return;
        }
        
        processSubmission(assignment, submission);
        int score = evaluateAssignment(assignment);
        provideFeedback(assignment, score);
        updateRecords(assignment, score);
        
        afterCompletion(assignment);
    }
};

// Concrete implementation for homework assignments
class HomeworkProcess : public AssignmentProcess {
protected:
    void validateAssignment(std::shared_ptr<Assignment> assignment) override;
    bool checkPrerequisites(std::shared_ptr<Assignment> assignment) override;
    void processSubmission(std::shared_ptr<Assignment> assignment, const std::string& submission) override;
    int evaluateAssignment(std::shared_ptr<Assignment> assignment) override;
    void provideFeedback(std::shared_ptr<Assignment> assignment, int score) override;
    void updateRecords(std::shared_ptr<Assignment> assignment, int score) override;
};

// Concrete implementation for project assignments
class ProjectProcess : public AssignmentProcess {
protected:
    void validateAssignment(std::shared_ptr<Assignment> assignment) override;
    bool checkPrerequisites(std::shared_ptr<Assignment> assignment) override;
    void processSubmission(std::shared_ptr<Assignment> assignment, const std::string& submission) override;
    int evaluateAssignment(std::shared_ptr<Assignment> assignment) override;
    void provideFeedback(std::shared_ptr<Assignment> assignment, int score) override;
    void updateRecords(std::shared_ptr<Assignment> assignment, int score) override;
    
    // Override hook method
    void beforeSubmission(std::shared_ptr<Assignment> assignment) override;
};

// Concrete implementation for exam assignments
class ExamProcess : public AssignmentProcess {
protected:
    void validateAssignment(std::shared_ptr<Assignment> assignment) override;
    bool checkPrerequisites(std::shared_ptr<Assignment> assignment) override;
    void processSubmission(std::shared_ptr<Assignment> assignment, const std::string& submission) override;
    int evaluateAssignment(std::shared_ptr<Assignment> assignment) override;
    void provideFeedback(std::shared_ptr<Assignment> assignment, int score) override;
    void updateRecords(std::shared_ptr<Assignment> assignment, int score) override;
    
    // Override hook method
    void afterCompletion(std::shared_ptr<Assignment> assignment) override;
};

#endif // ASSIGNMENT_PROCESS_H