import type { AcademicTracker } from "./types";

const at = await (async () => {
  const Module = await import("./academic_tracker.js" as any);
  const module = (await Module.default()) as AcademicTracker;
  module.seed();

  return module;
})();

export { at };
