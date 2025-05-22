// Academic Tracker JavaScript Module
class AcademicTracker {
  constructor(wasmModule) {
    this.module = wasmModule;
    this.initialized = false;
  }

  // Initialize the module
  init() {
    if (!this.initialized) {
      this.module.initApp();
      this.initialized = true;
    }
  }

  // Subject management
  createSubject(name, code, description = "") {
    if (!name || !code) {
      throw new Error("Subject name and code are required");
    }
    const index = this.module.createSubject(name, code, description);
    if (index < 0) {
      throw new Error("Failed to create subject");
    }
    return index;
  }

  getAllSubjects() {
    const subjects = this.module.getAllSubjects();
    const result = [];
    for (let i = 0; i < subjects.size(); i++) {
      result.push(subjects.get(i));
    }
    return result;
  }

  getSubject(index) {
    const subject = this.module.getSubject(index);
    if (subject === null) {
      return null;
    }
    return subject;
  }

  // Task management
  createTask(subjectIndex, title, description, deadline, taskType) {
    if (typeof subjectIndex !== "number" || !title || !deadline) {
      throw new Error("Subject index, title, and deadline are required");
    }

    // Format deadline if it's a Date object
    let deadlineStr = deadline;
    if (deadline instanceof Date) {
      deadlineStr = deadline
        .toISOString()
        .replace("Z", "")
        .replace(/\.\d+/, "");
    }

    const index = this.module.createTask(
      subjectIndex,
      title,
      description || "",
      deadlineStr,
      taskType || 1,
    );

    if (index < 0) {
      throw new Error("Failed to create task");
    }
    return index;
  }

  getTasksForSubject(subjectIndex) {
    const tasks = this.module.getTasksForSubject(subjectIndex);
    const result = [];
    for (let i = 0; i < tasks.size(); i++) {
      result.push(tasks.get(i));
    }
    return result;
  }

  markTaskAsCompleted(subjectIndex, taskIndex, marks) {
    if (
      typeof subjectIndex !== "number" ||
      typeof taskIndex !== "number" ||
      typeof marks !== "number"
    ) {
      throw new Error("Subject index, task index, and marks must be numbers");
    }

    if (marks < 0 || marks > 100) {
      throw new Error("Marks must be between 0 and 100");
    }

    const success = this.module.markTaskAsCompleted(
      subjectIndex,
      taskIndex,
      marks,
    );
    if (!success) {
      throw new Error("Failed to mark task as completed");
    }
    return true;
  }

  getGradeForMarks(marks) {
    if (typeof marks !== "number" || marks < 0 || marks > 100) {
      throw new Error("Marks must be a number between 0 and 100");
    }
    return this.module.getGradeForMarks(marks);
  }

  // Helper to convert task data from JavaScript to a format suitable for C++
  formatTaskData(task) {
    if (!task.deadline) {
      return task;
    }

    // Ensure deadline is in the right format
    let deadline = task.deadline;
    if (deadline instanceof Date) {
      deadline = deadline.toISOString().replace("Z", "").replace(/\.\d+/, "");
    }

    return {
      ...task,
      deadline,
    };
  }
}

// Create and export the module loader
export async function createAcademicTracker() {
  try {
    // Import the WebAssembly module
    const Module = await import("../build_wasm/academic_tracker.js");
    const wasmModule = await Module.default();

    // Create and initialize the tracker
    const tracker = new AcademicTracker(wasmModule);
    tracker.init();

    return tracker;
  } catch (error) {
    console.error("Failed to load WebAssembly module:", error);
    throw error;
  }
}
