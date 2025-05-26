import type { AcademicTracker } from "@/types";
import { createRootRouteWithContext, Outlet } from "@tanstack/react-router";

export const rootRoute = createRootRouteWithContext<{
  at: AcademicTracker;
}>()({
  component: () => {
    return <Outlet />;
  },
});
