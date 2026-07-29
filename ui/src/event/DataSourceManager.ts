import GlobalWebViewEbent from "./GlobalWebViewEbent";

export function DataSourceManagerStartAll() {
    GlobalWebViewEbent.sendOnce({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: "startALl"
        }
    })
}

export function DataSourceManagerZip() {
    GlobalWebViewEbent.sendOnce({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: "init"
        }
    },)
}