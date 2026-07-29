import { Button } from "@/components/ui/button";
import { DataSourceManagerInitAll } from "@/event/DataSourceManager";
import { useEffect } from "react";

export default function () {
    return <div>
        <Button onClick={() => DataSourceManagerInitAll()}>解压数据库zip</Button>
    </div>
}