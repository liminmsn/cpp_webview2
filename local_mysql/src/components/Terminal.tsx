import { useEffect, useRef } from "react";
import { Terminal } from "xterm";
import { FitAddon } from "xterm-addon-fit";
import GlobalWebViewEvent from "../event/GlobalWebViewEbent";

export default function TerminalPage() {
    const terminalRef = useRef<HTMLDivElement>(null);

    useEffect(() => {
        if (!terminalRef.current) return;

        const term = new Terminal({
            cursorBlink: true,
            cursorStyle: "block",
            fontSize: 14,
            fontFamily: "Cascadia Mono, Cascadia Code, Consolas, monospace",
            allowTransparency: true,
            convertEol: false,
            windowsMode: true,
            scrollback: 5000,
            theme: {
                background: "#00000000",
                foreground: "#e6e6e6",
                cursor: "#fcb52c",
                selectionBackground: "#3b82f680"
            }
        });

        const fitAddon = new FitAddon();
        term.loadAddon(fitAddon);
        term.open(terminalRef.current);
        fitAddon.fit();
        const resize = () => fitAddon.fit();
        window.addEventListener("resize", resize);

        GlobalWebViewEvent.send(
            {
                id: crypto.randomUUID(),
                data: {
                    type: "mysql",
                    data: {
                        type: "StartTerminal"
                    }
                }
            },
            () => { }
        );

        term.write("\x1b[32mLocal Mysql Terminal\x1b[0m\r\n");
        term.onData((data) => {
            GlobalWebViewEvent.sendOnce({
                id: crypto.randomUUID(),
                data: {
                    type: "mysql",
                    data: {
                        type: "webTowin",
                        data
                    }
                }
            });
        });
        const messageHandler = ({ data }: any) => {
            if (data.type === "terminal") {
                term.write(data.data);
            }
        };
        window.chrome.webview.addEventListener("message", messageHandler);

        let fontSize = 14;
        term.focus();
        terminalRef.current.addEventListener("click", () => {
            term.focus();
        });
        term.attachCustomKeyEventHandler((e) => {
            // Ctrl+Shift+C 复制
            if (e.ctrlKey && e.shiftKey && e.code === "KeyC") {
                const text = term.getSelection();
                if (text)
                    navigator.clipboard.writeText(text);
                return false;
            }

            // Ctrl+Shift+V 粘贴
            if (e.ctrlKey && e.shiftKey && e.code === "KeyV") {
                navigator.clipboard.readText().then(text => {
                    GlobalWebViewEvent.sendOnce({
                        id: crypto.randomUUID(),
                        data: {
                            type: "mysql",
                            data: {
                                type: "webTowin",
                                data: text
                            }
                        }
                    });
                });
                return false;
            }

            // Ctrl+L 清屏
            if (e.ctrlKey && e.code === "KeyL") {
                term.reset();
                return false;
            }

            // Ctrl+K 清空滚动区
            if (e.ctrlKey && e.code === "KeyK") {
                term.clear();
                return false;
            }

            // Ctrl+= 放大
            if (e.ctrlKey && (e.code === "Equal" || e.code === "NumpadAdd")) {
                fontSize++;
                term.options.fontSize = fontSize;
                fitAddon.fit();
                return false;
            }

            // Ctrl+- 缩小
            if (e.ctrlKey && (e.code === "Minus" || e.code === "NumpadSubtract")) {
                fontSize = Math.max(8, fontSize - 1);
                term.options.fontSize = fontSize;
                fitAddon.fit();
                return false;
            }

            // Ctrl+0 恢复字体
            if (e.ctrlKey && e.code === "Digit0") {
                fontSize = 14;
                term.options.fontSize = fontSize;
                fitAddon.fit();
                return false;
            }

            return true;
        });

        return () => {
            window.removeEventListener("resize", resize);
            window.chrome.webview.removeEventListener("message", messageHandler);
            term.dispose();
        };
    }, []);

    return (
        <div
            ref={terminalRef}
            className="w-full h-full overflow-hidden"
        />
    );
}