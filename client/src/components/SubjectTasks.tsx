import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import * as Kanban from "@/components/ui/kanban";
import { subjectRoute } from "@/routes/subject";
import type { Task } from "@/types";
import { useRouter } from "@tanstack/react-router";
import { formatDate } from "date-fns";
import { GripVertical } from "lucide-react";
import * as React from "react";

const COLUMN_TITLES: Record<string, string> = {
  Pending: "Не Розпочато",
  ["In Progress"]: "В Процесі",
  Completed: "Виконано",
};

const TASK_STATES = ["Pending", "In Progress", "Completed"];

export function SubjectTasks({ tasks }: { tasks: Task[] }) {
  const router = useRouter();

  const subjectCode = subjectRoute.useLoaderData({
    select: (data) => data.subject.code,
  });

  const at = subjectRoute.useRouteContext({
    select: (c) => c.at,
  });

  const [columns, setColumns] = React.useState<Record<string, Task[]>>(() => {
    return {
      Pending: tasks.filter((taks) => taks.stateName === "Pending"),
      ["In Progress"]: tasks.filter((taks) => taks.stateName === "In Progress"),
      Completed: tasks.filter((taks) => taks.stateName === "Completed"),
    };
  });

  return (
    <Kanban.Root
      value={columns}
      onValueChange={(newColumnsState) => {
        const oldTaskLocations: Record<string, string> = {};
        for (const columnName in columns) {
          if (Object.prototype.hasOwnProperty.call(columns, columnName)) {
            columns[columnName].forEach((task) => {
              oldTaskLocations[task.title] = columnName;
            });
          }
        }

        let movedTask: Task | undefined = undefined;
        let sourceColumnId: string | undefined = undefined;
        let destinationColumnId: string | undefined = undefined;

        for (const currentColumnKeyInNewSet in newColumnsState) {
          if (
            Object.prototype.hasOwnProperty.call(
              newColumnsState,
              currentColumnKeyInNewSet,
            )
          ) {
            for (const task of newColumnsState[currentColumnKeyInNewSet]) {
              const previousColumnKey = oldTaskLocations[task.title];

              if (
                previousColumnKey &&
                previousColumnKey !== currentColumnKeyInNewSet
              ) {
                movedTask = task;
                sourceColumnId = previousColumnKey;
                destinationColumnId = currentColumnKeyInNewSet;
                break;
              }
            }
          }
          if (movedTask) {
            break;
          }
        }

        if (movedTask && sourceColumnId && destinationColumnId) {
          const taskIndex = tasks.findIndex(
            (task) => task.title === movedTask.title,
          );

          at.changeTaskState(
            subjectCode,
            taskIndex,
            TASK_STATES.indexOf(destinationColumnId),
          );

          router.invalidate();
        }

        setColumns(newColumnsState);
      }}
      getItemValue={(item) => item.title}
    >
      <Kanban.Board className="grid auto-rows-fr grid-cols-3">
        {Object.entries(columns).map(([columnValue, tasks]) => (
          <TaskColumn key={columnValue} value={columnValue} tasks={tasks} />
        ))}
      </Kanban.Board>
      <Kanban.Overlay>
        {({ value, variant }) => {
          if (variant === "column") {
            const tasks = columns[value] ?? [];

            return <TaskColumn value={value} tasks={tasks} />;
          }

          const task = Object.values(columns)
            .flat()
            .find((task) => task.title === value);

          if (!task) return null;

          return <TaskCard task={task} />;
        }}
      </Kanban.Overlay>
    </Kanban.Root>
  );
}

interface TaskCardProps
  extends Omit<React.ComponentProps<typeof Kanban.Item>, "value"> {
  task: Task;
}

function TaskCard({ task, ...props }: TaskCardProps) {
  return (
    <Kanban.Item key={task.title} value={task.title} asChild {...props}>
      <div className="rounded-md border bg-card p-3 shadow-xs">
        <div className="flex flex-col gap-2">
          <div className="flex items-center justify-between gap-2">
            <span className="line-clamp-1 font-medium text-sm">
              {task.title}
            </span>
            <Badge
              variant={
                task.type === "Exam"
                  ? "default"
                  : task.type === "Lab"
                    ? "secondary"
                    : "outline"
              }
              className="pointer-events-none h-5 rounded-sm px-1.5 text-[11px] capitalize"
            >
              {task.type === "Exam" && "Екзамен"}
              {task.type === "Lab" && "Лабораторна"}
              {task.type === "Project" && "Проєкт"}
            </Badge>
          </div>
          <div className="flex items-center justify-between text-muted-foreground text-xs">
            {task.completed && (
              <div className="flex items-center gap-1">
                <span className="line-clamp-1">{task.description}</span>
              </div>
            )}
            {task.deadline && (
              <time className="text-[10px] tabular-nums">
                {formatDate(new Date(task.deadline), "dd.MM.yyyy")}
              </time>
            )}
          </div>
        </div>
      </div>
    </Kanban.Item>
  );
}

interface TaskColumnProps
  extends Omit<React.ComponentProps<typeof Kanban.Column>, "children"> {
  tasks: Task[];
}

function TaskColumn({ value, tasks, ...props }: TaskColumnProps) {
  return (
    <Kanban.Column value={value} {...props}>
      <div className="flex items-center justify-between">
        <div className="flex items-center gap-2">
          <span className="font-semibold text-sm">{COLUMN_TITLES[value]}</span>
          <Badge variant="secondary" className="pointer-events-none rounded-sm">
            {tasks.length}
          </Badge>
        </div>
        <Kanban.ColumnHandle asChild>
          <Button variant="ghost" size="icon">
            <GripVertical className="h-4 w-4" />
          </Button>
        </Kanban.ColumnHandle>
      </div>
      <div className="flex flex-col gap-2 p-0.5">
        {tasks.map((task) => (
          <TaskCard key={task.title} task={task} asHandle />
        ))}
      </div>
    </Kanban.Column>
  );
}
