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
export function DataSourceManager_Injection(key: "MYSQL" | "REDIS", data: any, call: (data: any) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: key,
                data,
            }
        }
    }, call)
}
export function DataSourceManager_MYSQL_Initd(call: (bool: any) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: "MYSQL",
                data: {
                    key: "Initd"
                }
            }
        }
    }, call)
}

export function DataSourceManager_MYSQL_Run(key: "Run" | "Stop", call: (bool: any) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: "MYSQL",
                data: { key }
            }
        }
    }, call)
}