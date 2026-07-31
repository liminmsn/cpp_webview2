import type { ServiceStateType } from "@/store/features/serviceSlice";
import GlobalWebViewEbent from "./GlobalWebViewEbent";


export interface DataSourceManagerInitAllRes {
    data: boolean[];
    type: string;
    msg: string;
}


export function DataSourceManager_GetState(call: (states: ServiceStateType) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: "GetState"
        }
    }, call)
}
export function DataSourceManager_Init(initKey: string) {
    GlobalWebViewEbent.sendOnce({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: initKey
        }
    })
}

export function DataSourceManager_GetOutDir(key: "MYSQL" | "REDIS", call: (outDir: string) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: "GetOutDir",
                key
            }
        }
    }, call)
}