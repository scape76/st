#ifndef MEMENTO_H
#define MEMENTO_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

// Forward declarations
class Assignment;
class Course;

// Memento for Assignment state
class AssignmentMemento {
private:
    friend class Assignment;
    friend class MementoCaretaker;
    
    std::string title;
    std::string description;
    std::chrono::system_clock::time_point deadline;
    bool completed;
    int marks;
    float progress;
    std::string state;
    std::string courseCode; // Reference to parent course

public:
    // Constructor made public for std::make_shared
    AssignmentMemento(
        const std::string& title,
        const std::string& description,
        const std::chrono::system_clock::time_point& deadline,
        bool completed,
        int marks,
        float progress,
        const std::string& state,
        const std::string& courseCode
    ) : title(title), description(description), deadline(deadline),
        completed(completed), marks(marks), progress(progress),
        state(state), courseCode(courseCode) {}
        
    // Getters for accessing private members from outside
    std::string getTitle() const { return title; }
    std::string getDescription() const { return description; }
    std::chrono::system_clock::time_point getDeadline() const { return deadline; }
    bool isCompleted() const { return completed; }
    int getMarks() const { return marks; }
    float getProgress() const { return progress; }
    std::string getState() const { return state; }
    std::string getCourseCode() const { return courseCode; }
};

// Memento for Course state
class CourseMemento {
private:
    friend class Course;
    friend class MementoCaretaker;
    
    std::string name;
    std::string code;
    std::string description;
    std::vector<std::string> assignmentTitles; // References to assignments
    
public:
    // Constructor made public for std::make_shared
    CourseMemento(
        const std::string& name,
        const std::string& code,
        const std::string& description,
        const std::vector<std::string>& assignmentTitles
    ) : name(name), code(code), description(description),
        assignmentTitles(assignmentTitles) {}
        
    // Getters for accessing private members
    std::string getName() const { return name; }
    std::string getCode() const { return code; }
    std::string getDescription() const { return description; }
    const std::vector<std::string>& getAssignmentTitles() const { return assignmentTitles; }
};

// AcademicProgressMemento stores the entire application state
class AcademicProgressMemento {
private:
    std::vector<std::shared_ptr<CourseMemento>> courses;
    std::vector<std::shared_ptr<AssignmentMemento>> assignments;
    std::string timestamp;
    std::string description;
    
public:
    AcademicProgressMemento(const std::string& description = "")
        : description(description) {
        // Set timestamp to current time
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d_%H-%M-%S");
        timestamp = ss.str();
    }
    
    void addCourseMemento(std::shared_ptr<CourseMemento> memento) {
        courses.push_back(memento);
    }
    
    void addAssignmentMemento(std::shared_ptr<AssignmentMemento> memento) {
        assignments.push_back(memento);
    }
    
    const std::vector<std::shared_ptr<CourseMemento>>& getCourses() const {
        return courses;
    }
    
    const std::vector<std::shared_ptr<AssignmentMemento>>& getAssignments() const {
        return assignments;
    }
    
    std::string getTimestamp() const {
        return timestamp;
    }
    
    std::string getDescription() const {
        return description;
    }
};

// Caretaker class to manage mementos
class MementoCaretaker {
private:
    std::vector<std::shared_ptr<AcademicProgressMemento>> savedStates;
    
public:
    void addMemento(std::shared_ptr<AcademicProgressMemento> memento) {
        savedStates.push_back(memento);
    }
    
    std::shared_ptr<AcademicProgressMemento> getMemento(size_t index) {
        if (index < savedStates.size()) {
            return savedStates[index];
        }
        return nullptr;
    }
    
    std::shared_ptr<AcademicProgressMemento> getLatestMemento() {
        if (!savedStates.empty()) {
            return savedStates.back();
        }
        return nullptr;
    }
    
    std::vector<std::shared_ptr<AcademicProgressMemento>> getAllMementos() const {
        return savedStates;
    }
    
    size_t getMementoCount() const {
        return savedStates.size();
    }
    
    bool saveToFile(const std::string& filename) {
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << "AcademicProgressTracker_SaveFile_v1" << std::endl;
            file << savedStates.size() << std::endl;
            
            for (const auto& memento : savedStates) {
                file << memento->getTimestamp() << std::endl;
                file << memento->getDescription() << std::endl;
                
                // Save courses
                const auto& courses = memento->getCourses();
                file << courses.size() << std::endl;
                for (const auto& course : courses) {
                    file << course->getName() << std::endl;
                    file << course->getCode() << std::endl;
                    file << course->getDescription() << std::endl;
                    
                    // Save assignment references
                    file << course->getAssignmentTitles().size() << std::endl;
                    for (const auto& title : course->getAssignmentTitles()) {
                        file << title << std::endl;
                    }
                }
                
                // Save assignments
                const auto& assignments = memento->getAssignments();
                file << assignments.size() << std::endl;
                for (const auto& assignment : assignments) {
                    file << assignment->getTitle() << std::endl;
                    file << assignment->getDescription() << std::endl;
                    
                    // Save deadline timestamp
                    auto time = std::chrono::system_clock::to_time_t(assignment->getDeadline());
                    file << time << std::endl;
                    
                    file << assignment->isCompleted() << std::endl;
                    file << assignment->getMarks() << std::endl;
                    file << assignment->getProgress() << std::endl;
                    file << assignment->getState() << std::endl;
                    file << assignment->getCourseCode() << std::endl;
                }
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error saving memento: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool loadFromFile(const std::string& filename) {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            std::string version;
            std::getline(file, version);
            if (version != "AcademicProgressTracker_SaveFile_v1") {
                return false;
            }
            
            size_t mementoCount;
            file >> mementoCount;
            file.ignore(); // Consume newline
            
            savedStates.clear();
            
            for (size_t i = 0; i < mementoCount; ++i) {
                std::string timestamp, description;
                std::getline(file, timestamp);
                std::getline(file, description);
                
                auto memento = std::make_shared<AcademicProgressMemento>(description);
                
                // Load courses
                size_t courseCount;
                file >> courseCount;
                file.ignore(); // Consume newline
                
                for (size_t j = 0; j < courseCount; ++j) {
                    std::string name, code, courseDescription;
                    std::getline(file, name);
                    std::getline(file, code);
                    std::getline(file, courseDescription);
                    
                    std::vector<std::string> assignmentTitles;
                    size_t titleCount;
                    file >> titleCount;
                    file.ignore(); // Consume newline
                    
                    for (size_t k = 0; k < titleCount; ++k) {
                        std::string title;
                        std::getline(file, title);
                        assignmentTitles.push_back(title);
                    }
                    
                    auto courseMemento = std::make_shared<CourseMemento>(
                        name, code, courseDescription, assignmentTitles);
                    memento->addCourseMemento(courseMemento);
                }
                
                // Load assignments
                size_t assignmentCount;
                file >> assignmentCount;
                file.ignore(); // Consume newline
                
                for (size_t j = 0; j < assignmentCount; ++j) {
                    std::string title, description, state, courseCode;
                    std::getline(file, title);
                    std::getline(file, description);
                    
                    // Load deadline timestamp
                    std::time_t timestamp;
                    file >> timestamp;
                    file.ignore(); // Consume newline
                    auto deadline = std::chrono::system_clock::from_time_t(timestamp);
                    
                    bool completed;
                    int marks;
                    float progress;
                    
                    file >> completed;
                    file >> marks;
                    file >> progress;
                    file.ignore(); // Consume newline
                    
                    std::getline(file, state);
                    std::getline(file, courseCode);
                    
                    auto assignmentMemento = std::make_shared<AssignmentMemento>(
                        title, description, deadline, completed, marks, progress, 
                        state, courseCode);
                    memento->addAssignmentMemento(assignmentMemento);
                }
                
                savedStates.push_back(memento);
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error loading memento: " << e.what() << std::endl;
            return false;
        }
    }
    
    void clearMementos() {
        savedStates.clear();
    }
};

#endif // MEMENTO_H