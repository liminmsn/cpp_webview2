import { Button } from "@/components/ui/button";
import { DataSourceManagerZip } from "@/event/DataSourceManager";
import { useEffect } from "react";

export default function () {
    useEffect(() => {
        // DataSourceManagerStartAll()
    }, [])

    return <div>
        <Button onClick={DataSourceManagerZip}>解压数据库zip</Button>
    </div>
}