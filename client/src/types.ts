type Subject = {
  name: string;
  code: string;
  description: string;
};

type LabType = "lab" | "exam" | "project";

type Task = {
  title: string;
  description: string;
  deadline: string;
  type: LabType;
  completed: boolean;
  marks: number;
};

export type AcademicTracker = {
  createSubject: (name: string, code: string, description: string) => void;
  getAllSubjects: () => Subject[];
  getSubject: (code: string) => Subject;
  getSubjectTasks: (subjectCode: string) => Task[];
  createTask: (
    subjectCode: string,
    title: string,
    description: string,
    deadline: string,
    type: LabType,
  ) => void;
};
