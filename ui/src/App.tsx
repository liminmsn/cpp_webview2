// import { Button } from "./components/ui/button"
import { SidebarProvider, SidebarTrigger } from "./components/ui/sidebar"
import { AppSidebar } from "@/components/app-sidebar"
export default function () {
  return <div className="h-screen">
    <SidebarProvider>
      <AppSidebar />
      <main className="relative p-2">
        <div className="absolute top-1 left-1">
          <SidebarTrigger />
        </div>
        fds
      </main>
    </SidebarProvider>
  </div>
}
