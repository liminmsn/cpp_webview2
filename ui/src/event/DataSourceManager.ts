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
export function DataSourceManager_OpenWithExplorer(path: string, call: (states: ServiceStateType) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: "OpenWithExplorer",
            path
        }
    }, call)
}
export function DataSourceManager_OpenNewTerminal(path: string, args: string, call: (states: ServiceStateType) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: "OpenNewTerminal",
            path, args
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

export function DataSourceManager_MYSQL_InitdServer(call: (bool: any) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: "MYSQL",
                data: {
                    key: "InitdServer"
                }
            }
        }
    }, call)
}
export function DataSourceManager_MYSQL_InitializeMysql(call: (bool: any) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: "MYSQL",
                data: {
                    key: "InitializeMysql"
                }
            }
        }
    }, call)
}

export function DataSourceManager_MYSQL_Run(key: "Run" | "Stop", args?: string, call?: (state: any) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: "MYSQL",
                data: {
                    key,
                    mysqladminArgs: args
                }
            }
        }
    }, call || function () { });
}

export function DataSourceManager_MYSQL_IsRun(call: (bool: any) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: "MYSQL",
                data: {
                    key: "IsRun"
                }
            }
        }
    }, call)
}

export function DataSourceManager_REDIS_GetConfigFile(call: (blob: any) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: "REDIS",
                data: {
                    key: "GetConfigFile"
                }
            }
        }
    }, call)
}

export function DataSourceManager_REDIS_WriteConfigFile(content: string, call: (blob: any) => void) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: {
                type: "REDIS",
                data: {
                    key: "WriteConfigFile",
                    content
                }
            }
        }
    }, call)
}