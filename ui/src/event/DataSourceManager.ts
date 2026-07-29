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

export function DataSourceManagerInitAll() {
    GlobalWebViewEbent.sendOnce({
        id: crypto.randomUUID(),
        data: {
            type: "DataSourceManager",
            data: "InitAll"
        }
    })
}