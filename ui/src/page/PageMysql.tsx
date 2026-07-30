import type { RootState } from "@/store/store";
import { useEffect, useState } from "react";
import { Button } from "@/components/ui/button";
import { DataSourceManager_GetState, DataSourceManager_Init } from "@/event/DataSourceManager";
import { useSelector, useDispatch } from "react-redux";
import { updateState } from "@/store/features/serviceSlice";

function Empty() {
    const dispatch = useDispatch();
    const [msg, setMsg] = useState('ps:解压程序内置的Mysql压缩包【仅1次】');

    useEffect(() => {
        function onMessage({ data }: WebView2Event) {
            console.log(data);
            const d = data?.data;
            if (d.type == "Init") {
                setMsg(JSON.stringify(d));
                // 解压完成
                if (d['data']['end']) {
                    DataSourceManager_GetState((state) => dispatch(updateState(state)));
                }
            }
        }
        window.chrome.webview.addEventListener("message", onMessage)
        return function () {
            window.chrome.webview.removeEventListener("message", onMessage)
        }
    }, [dispatch]);

    return <div className="h-full flex flex-col items-center justify-center">
        <div className="text-xs text-accent mb-1 max-w-5/10">{msg}</div>
        <Button onClick={() => DataSourceManager_Init("InitMysql")}>解压包</Button>
    </div>
}

export default function () {
    const services = useSelector((state: RootState) => state.services)

    function Content() {
        return <div>内容页面</div>
    }

    if (services.mysql.InitialState)
        return <Content />
    return <Empty />
}