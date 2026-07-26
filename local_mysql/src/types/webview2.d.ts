export { };

declare global {
    interface Window {
        chrome: Chrome;
    }

    interface Chrome {
        app: App;
        webview: WebView;
        csi: () => any;
        loadTimes: () => any;
    }

    interface App {
        isInstalled: boolean;
        InstallState: {
            DISABLED: string;
            INSTALLED: string;
            NOT_INSTALLED: string;
        };
        RunningState: {
            CANNOT_RUN: string;
            READY_TO_RUN: string;
            RUNNING: string;
        };
    }

    interface WebView {
        /**
         * 向 C++ 发送消息
         */
        postMessage(message: any): void;

        /**
         * 带额外对象的发送（共享缓冲区等）
         */
        postMessageWithAdditionalObjects(
            message: any,
            additionalObjects: any[]
        ): void;

        /**
         * 释放 buffer（WebView2 高级 API）
         */
        releaseBuffer(bufferId: number | string): void;

        /**
         * 事件监听（message / navigation 等）
         */
        addEventListener(
            type: WebView2EventType,
            listener: (event: WebView2Event) => void
        ): void;

        removeEventListener(
            type: WebView2EventType,
            listener: (event: WebView2Event) => void
        ): void;

        dispatchEvent(event: Event): boolean;

        /**
         * Edge WebView2 远程对象代理
         */
        hostObjects: HostObjectsProxy;

        /**
         * 运行时内部代理组（你看到的 RemoteProxyGroup）
         */
        RemoteProxyGroup?: any;
    }

    type WebView2EventType =
        | "message"
        | "navigationstarting"
        | "navigationcompleted"
        | "contentloading"
        | "DOMContentLoaded"
        | "webmessage";

    interface WebView2EventData {
        id: string;
        data: any;
    }

    interface WebView2Event {
        data?: WebView2EventData;
        source?: any;
        origin?: string;
    }

    /**
     * WebView2 hostObjects（C++ COM 对象映射）
     */
    interface HostObjectsProxy {
        /**
         * 同步 COM 对象
         */
        sync: Record<string, any>;

        /**
         * 异步 COM 对象
         */
        async: Record<string, any>;

        /**
         * WebView2 内部 Proxy
         */
        [key: string]: any;
    };



    type HttpMethod = "GET" | "POST" | "PUT" | "DELETE";

    type CallbackType = (status: number, body: string) => void;

    interface Bridge {
        [key: string]: () => Promise<any>;
        id: string;

        player(url: string): void;
        request(
            id: string,
            method: HttpMethod,
            url: string,
            headers: Record<string, string>,
            body: string
        ): void;
    }
}