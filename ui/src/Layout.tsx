import {
  Sidebar,
  SidebarContent,
  SidebarFooter,
  SidebarHeader,
  SidebarMenu,
  SidebarMenuItem,
  SidebarProvider,
  SidebarTrigger,
} from "@/components/ui/sidebar"
import { useEffect, useState } from "react";
import PageMysql from "./page/PageMysql";
import PageRedis from "./page/PageRedis";
import { GithubIcon, Information } from "@hugeicons/core-free-icons";
import { HugeiconsIcon } from "@hugeicons/react";
import sql from "@/assets/sql.png"
import redis from "@/assets/redis.png"
import { Toaster } from "./components/ui/sonner";
import { useDispatch } from "react-redux";
import { DataSourceManager_GetState } from "./event/DataSourceManager";
import { updateState } from "./store/features/serviceSlice";
import { Button } from "./components/ui/button";
import { Info } from "lucide-react";
import { Dialog, DialogClose, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle, DialogTrigger } from "./components/ui/dialog";

type SidebarMenuItemType = {
  name: string;
  icon: () => React.JSX.Element;
  page: () => React.JSX.Element;
}

const projects: SidebarMenuItemType[] = [
  {
    name: "Mysql",
    icon: () => <img srcSet={sql} className="h-6 min-w-6" />,
    page: PageMysql
  },
  {
    name: "Redis",
    icon: () => <img srcSet={redis} className="h-6 min-w-6" />,
    page: PageRedis
  }
]


export default function () {
  const dispatch = useDispatch()
  const [selectPage, setSelectPage] = useState(projects[0]);

  useEffect(() => {
    DataSourceManager_GetState((state) => dispatch(updateState(state)))
  }, [dispatch]);

  return <div className="h-screen">
    <Toaster />
    <SidebarProvider defaultOpen={false}>
      <div className="max-w-35">
        <Sidebar className="w-35" collapsible="icon">
          <SidebarHeader>
            <SidebarMenu>
              {projects.map((project) => {
                return <SidebarMenuItem key={project.name}>
                  <div className={`flex gap-x-2 rounded-sm overflow-x-clip p-1 cursor-pointer select-none ${project == selectPage && "bg-primary"}`} onClick={() => setSelectPage(project)}>
                    <project.icon />
                    <span>{project.name}</span>
                  </div>
                </SidebarMenuItem>
              })}
            </SidebarMenu>
          </SidebarHeader>
          <SidebarContent>
          </SidebarContent>
          <SidebarFooter>
            <SidebarTrigger className="cursor-pointer" />
            <a className="w-full flex items-end overflow-x-clip" target="_blank" href="https://github.com/liminmsn/cpp_webview2/tree/LocalRM" >
              <span className="p-1 bg-accent inline-block rounded-sm">
                <HugeiconsIcon icon={GithubIcon} size={20} />
              </span>
              <span className="inline-block pl-2">GitHub</span>
            </a>

            <Dialog>
              <DialogTrigger render={
                <div className="w-full flex items-end overflow-x-clip cursor-pointer">
                  <span className="p-1 bg-orange-500 inline-block rounded-sm">
                    <HugeiconsIcon icon={Information} size={20} />
                  </span>
                  <span className="inline-block pl-2">About</span>
                </div>
              } />
              <DialogContent showCloseButton={false}>
                <DialogHeader>
                  <DialogTitle>关于本程序开发灵感</DialogTitle>
                  <DialogDescription>
                    This dialog has a sticky footer that stays visible while the content
                    scrolls.
                  </DialogDescription>
                </DialogHeader>
                <div className="-mx-4 no-scrollbar max-h-[50vh] overflow-y-auto px-4">
                  {Array.from({ length: 10 }).map((_, index) => (
                    <p key={index} className="mb-4 leading-normal">
                      Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do
                      eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut
                      enim ad minim veniam, quis nostrud exercitation ullamco laboris
                      nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in
                      reprehenderit in voluptate velit esse cillum dolore eu fugiat
                      nulla pariatur. Excepteur sint occaecat cupidatat non proident,
                      sunt in culpa qui officia deserunt mollit anim id est laborum.
                    </p>
                  ))}
                </div>
                <DialogFooter>
                  <DialogClose render={<Button variant="outline">Close</Button>} />
                </DialogFooter>
              </DialogContent>
            </Dialog>
          </SidebarFooter>
        </Sidebar>
      </div>

      <main className="w-full h-screen">
        {<selectPage.page />}
      </main>
    </SidebarProvider>
  </div>
}
