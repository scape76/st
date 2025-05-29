import {
  Dialog,
  DialogContent,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { useRouter } from "@tanstack/react-router";
import { useState } from "react";
import { indexRoute } from "../routes/index";
import { useForm } from "@tanstack/react-form";
import { Button } from "./ui/button";
import { PlusIcon } from "lucide-react";
import { Textarea } from "./ui/textarea";

export function CreateSubjectDialog() {
  const [open, setOpen] = useState(false);
  const router = useRouter();

  const at = indexRoute.useRouteContext({
    select: (c) => c.at,
  });

  const form = useForm({
    defaultValues: {
      name: "",
      code: "",
      description: "",
    },
    onSubmit: ({ value }) => {
      console.log("value is ", value);
      at.createSubject(value.name, value.code, value.description);
      router.invalidate({
        filter: (c) => c.fullPath === "/",
      });
      setOpen(false);
    },
  });

  return (
    <Dialog open={open} onOpenChange={setOpen}>
      <DialogTrigger asChild>
        <Button variant="outline">
          <PlusIcon />
          Створити Новий
        </Button>
      </DialogTrigger>
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          <DialogTitle>Новий Предмет</DialogTitle>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="name" className="text-right">
              Назва
            </Label>
            <form.Field
              name="name"
              children={(field) => (
                <Input
                  id={field.name}
                  value={field.state.value}
                  className="col-span-3"
                  onChange={(e) => field.handleChange(e.target.value)}
                />
              )}
            />
          </div>
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="code" className="text-right">
              Код
            </Label>
            <form.Field
              name="code"
              children={(field) => (
                <Input
                  id={field.name}
                  value={field.state.value}
                  className="col-span-3"
                  onChange={(e) => field.handleChange(e.target.value)}
                />
              )}
            />
          </div>
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="code" className="text-right">
              Опис
            </Label>
            <form.Field
              name="description"
              children={(field) => (
                <Textarea
                  id={field.name}
                  value={field.state.value}
                  className="col-span-3"
                  onChange={(e) => field.handleChange(e.target.value)}
                />
              )}
            />
          </div>
        </div>
        <DialogFooter>
          <Button type="submit" onClick={form.handleSubmit}>
            Зберегти
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}
