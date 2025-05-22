import type { AcademicTracker } from "./types";

const at = await (async () => {
  const Module = await import("./academic_tracker.js" as any);
  return (await Module.default()) as AcademicTracker;
})();

export { at };
