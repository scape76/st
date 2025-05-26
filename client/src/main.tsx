import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import { router } from "./routes.tsx";
import { RouterProvider } from "@tanstack/react-router";
import { at } from "./at.ts";
import "./index.css";

function WithAt() {
  return <RouterProvider router={router} context={{ at }} />;
}

createRoot(document.getElementById("root")!).render(
  <StrictMode>
    <WithAt />
  </StrictMode>,
);
