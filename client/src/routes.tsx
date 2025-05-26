import { createRouter } from "@tanstack/react-router";
import type { AcademicTracker } from "./types.js";
import { indexRoute } from "./routes/index.js";

import { rootRoute } from "./routes/root.js";
import { subjectRoute } from "./routes/subject.js";

const routeTree = rootRoute.addChildren([indexRoute, subjectRoute]);

const router = createRouter({
  routeTree,
  context: {
    at: undefined! as AcademicTracker,
  },
});

declare module "@tanstack/react-router" {
  interface Register {
    router: typeof router;
  }
}

export { router };
