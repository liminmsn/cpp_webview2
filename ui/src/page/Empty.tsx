import { Button } from "@/components/ui/button";
import { DataSourceManager_GetState, DataSourceManager_Init } from "@/event/DataSourceManager";
import { updateState } from "@/store/features/serviceSlice";
import { PackageOpen } from "lucide-react";
import { useState, useEffect } from "react";
import { useDispatch } from "react-redux";

export default function ({ InitKey }: { InitKey: "InitMysql" | "InitRedis" }) {
    const dispatch = useDispatch();
    const [msg, setMsg] = useState(`解压程序内置的${InitKey.replaceAll("Init", "")}压缩包【仅1次】`);

    const [disabled, setDisabled] = useState(false);

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
        <div className="text-xs text-accent mb-1 max-w-5/10 overflow-hidden text-ellipsis">{msg}</div>
        <Button disabled={disabled} onClick={() => {
            DataSourceManager_Init(InitKey)
            setDisabled(true)
        }}>
            初始化
            <PackageOpen />
        </Button>
    </div>
}