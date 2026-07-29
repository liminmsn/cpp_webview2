export default class GlobalWebViewEbent {
    private static listenerArr: Map<string, <T>(data: T) => void> = new Map();
    /**只需要在入口函数调用一次 */
    static start() {
        console.log('初始化全局webview事件');
        window.chrome.webview.addEventListener("message", (event) => {
            const { data } = event;
            console.log(event);

            if (data) {
                const callBackFun = this.listenerArr.get(data.id);
                if (callBackFun) {
                    callBackFun(data.data);
                    //使用完删除掉
                    this.listenerArr.delete(data.id);
                }
            }

        })
    }
    static send(parameter: WebView2EventData, callback: (data: any) => void) {
        this.listenerArr.set(parameter.id, callback);
        window.chrome.webview.postMessage(parameter);
    }
    static sendOnce(parameter: WebView2EventData) {
        window.chrome.webview.postMessage(parameter)
    }
}