import GlobalWebViewEbent from "../event/GlobalWebViewEbent";
type CallType = (state: boolean) => void;

export function GetState(call: CallType) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "mysql",
            data: {
                "type": "GetState"
            }
        }
    }, (state: any) => {
        if (state) {
            document.body.style.setProperty("--bg_color", state.running ? "var(--accent)" : "var(--color-warning)");
            document.body.style.animation = state.running ? "bgMove 3s linear infinite" : "";
        }
        call(state);
    })
}
export function InstallService(call: CallType) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "mysql",
            data: {
                "type": "InstallService"
            }
        }
    }, call)
}

export function RemoveService(call: CallType) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "mysql",
            data: {
                "type": "RemoveService"
            }
        }
    }, call)
}


export function Start(call: CallType) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "mysql",
            data: {
                "type": "Start"
            }
        }
    }, call)
}

export function Stop(call: CallType) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "mysql",
            data: {
                "type": "Stop"
            }
        }
    }, call)
}

export function IsInstallService(call: CallType) {
    GlobalWebViewEbent.send({
        id: crypto.randomUUID(),
        data: {
            type: "mysql",
            data: {
                "type": "IsInstallService"
            }
        }
    }, call)
}

export function OpenDir(path: string) {
    GlobalWebViewEbent.sendOnce({
        id: crypto.randomUUID(),
        data: {
            type: "mysql",
            data: { type: "OpenDir", path }
        }
    })
}

export function OpenExe(info: string) {
    GlobalWebViewEbent.sendOnce({
        id: crypto.randomUUID(),
        data: {
            type: "mysql",
            data: { type: "OpenExE", info }
        }
    })
}