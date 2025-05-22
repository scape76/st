import { createRoute } from "@tanstack/react-router";
import { rootRoute } from "./root";

const subjectRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: "/subject/$code",
  loader: ({ context, params }) => {
    console.log("params ", params);
    const subject = context.at.getSubject(params.code);
    const tasks = context.at.getTasksForSubject(params.code);

    return {
      subject,
      tasks,
    };
  },
  component: Route,
  notFoundComponent: () => "not Found!!",
});

function Route() {
  const { subject, tasks } = subjectRoute.useLoaderData();

  console.log("subject", subject);
  console.log("tasks", tasks);

  return (
    <main className="container mx-auto p-8">
      <div className="flex justify-between sm:items-center flex-col sm:flex-row gap-2">
        <h1 className="text-2xl font-bold">{subject.name}</h1>
      </div>
    </main>
  );
}

export { subjectRoute };
