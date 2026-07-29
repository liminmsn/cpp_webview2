import {
  Sidebar,
  SidebarContent,
  SidebarFooter,
  SidebarHeader,
  SidebarMenu,
  SidebarMenuButton,
  SidebarMenuItem,
  SidebarProvider,
  SidebarTrigger,
} from "@/components/ui/sidebar"
import { type LucideProps, Database, LayoutDashboardIcon } from "lucide-react";
import { useState, type ForwardRefExoticComponent, type RefAttributes } from "react";
import PageMysql from "./page/PageMysql";
import PageRedis from "./page/PageRedis";
import { Github, GithubFreeIcons, GithubIcon } from "@hugeicons/core-free-icons";
import { HugeiconsIcon } from "@hugeicons/react";
import sql from "@/assets/sql.png"
import redis from "@/assets/redis.png"

type SidebarMenuItemType = {
  name: string;
  icon: () => React.JSX.Element;
  page: () => React.JSX.Element;
}

const projects: SidebarMenuItemType[] = [
  {
    name: "Mysql",
    icon: () => <img srcSet={sql} className="h-6" />,
    page: PageMysql
  },
  {
    name: "Redis",
    icon: () => <img srcSet={redis} className="h-6" />,
    page: PageRedis
  }
]
export default function () {
  const [selectPage, setSelectPage] = useState(projects[0]);

  return <div className="h-screen">
    <SidebarProvider defaultOpen={false}>
      <div className="max-w-35">
        <Sidebar className="w-35" variant="sidebar" collapsible="icon">
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
            <a target="_blank" href="https://github.com/liminmsn/cpp_webview2/tree/LocalRM" className="p-0 flex items-end overflow-x-clip" >
              <div className="bg-accent inline-block p-1 rounded-sm">
                <HugeiconsIcon icon={GithubIcon} size={20} />
              </div>
              <span className="inline-block pl-2">GitHub</span>
            </a>
          </SidebarFooter>
        </Sidebar>
      </div>

      <main className="w-full h-screen p-2">
        {<selectPage.page />}
      </main>
    </SidebarProvider>
  </div>
}
