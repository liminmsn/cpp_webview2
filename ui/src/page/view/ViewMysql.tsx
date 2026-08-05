import { DataSourceManager_Injection, DataSourceManager_MYSQL_Initd, DataSourceManager_MYSQL_InitdServer, DataSourceManager_MYSQL_InitializeMysql, DataSourceManager_MYSQL_IsRun, DataSourceManager_MYSQL_Run, DataSourceManager_OpenNewTerminal, DataSourceManager_OpenWithExplorer } from '@/event/DataSourceManager';
import { BaseNodeHeaderTitle } from '@/components/base-node';
import { LabeledHandle } from '@/components/labeled-handle';
import { BaseHandle } from '@/components/base-handle';
import { Textarea } from '@/components/ui/textarea';
import { Button } from '@/components/ui/button';
import { Label } from '@/components/ui/label';
import XyFlow from '@/components/XyFlow';
import type { BaseNodePropChildrenDataType } from '@/components/XyFlow/RMBaseNode';
import type { RootState } from '@/store/store';
import { updateCurrentState } from '@/store/features/currentSlice';
import { Edit, FileSliders, Folder, Info, Rocket } from 'lucide-react';
import { useDispatch, useSelector } from 'react-redux';
import { useNodesState, Position, useEdgesState, useReactFlow } from '@xyflow/react';
import { useEffect, useState } from 'react';
import sql from "@/assets/sql.png";
import '@xyflow/react/dist/style.css';
import { toast } from 'sonner';
/**Data管理文件夹 */
const DataNode = {
    id: "node_3",
    position: { x: 470, y: 380 },
    data: {
        targetData: {
            initdServer: false
        },
        HeaderComponent({ targetData }: BaseNodePropChildrenDataType) {
            const [data] = targetData;

            useEffect(() => { }, [data.initdServer])
            return <>
                <BaseHandle type="target" position={Position.Left} />
                <BaseHandle className={data.initdServer ? "" : "opacity-0"} type="source" position={Position.Right} />
                <Folder className="size-5" />
                <BaseNodeHeaderTitle>
                    Data目录
                </BaseNodeHeaderTitle>
            </>
        },
        ContentComponent({ targetData }: BaseNodePropChildrenDataType) {
            const flow = useReactFlow();

            const [data, setData] = targetData;
            const [disabled, setDisabled] = useState(false);
            const services = useSelector((state: RootState) => state.services)

            useEffect(() => {
                if (data.initdServer) {
                    flow.addNodes(RunNode);
                    flow.addEdges({
                        id: crypto.randomUUID(),
                        source: "node_3",
                        target: "node_2",
                        // type: "default"
                    });

                }

                DataSourceManager_MYSQL_InitdServer(bool => setData({ initdServer: bool }));
            }, [data.initdServer, disabled, flow, setData]);

            if (data.initdServer) {
                return <div className='flex flex-col gap-y-1'>
                    <Button onClick={() => {
                        DataSourceManager_OpenWithExplorer(services.mysql.outDir + "\\data", (bool) => {
                            console.log(bool);
                        })
                    }}>打开目录</Button>
                    <div className='bg-chart-3/40 p-2 rounded-sm text-center'>
                        <Label>已经初始化</Label>
                    </div>
                </div>
            }


            return <div>
                <Button disabled={disabled} onClick={() => {
                    setDisabled(true);
                    DataSourceManager_MYSQL_InitializeMysql((bool) => {
                        setDisabled(false);
                        setData({ initdServer: bool })
                    })
                }}>DATA初始化</Button>
            </div>
        }
    },
    type: "RMBaseNode"
}
/***运行节点**/
const RunNode = {
    id: "node_2",
    position: { x: 650, y: 380 },
    data: {
        header: <>
            <BaseHandle type="target" position={Position.Left} />
            <Rocket className="size-5" />
            <BaseNodeHeaderTitle>
                运行
            </BaseNodeHeaderTitle>
        </>,
        ContentComponent() {
            const dispatch = useDispatch();

            const services = useSelector((state: RootState) => state.services)
            const current = useSelector((state: RootState) => state.current)
            const [disabled, setDisabled] = useState(false);
            useEffect(() => { }, [disabled, dispatch]);

            function onToggle(key: "Run" | "Stop", mysqladminArgs: string) {
                setDisabled(true);
                DataSourceManager_MYSQL_Run(key, mysqladminArgs, () => {
                    setTimeout(() => {
                        DataSourceManager_MYSQL_IsRun((bool) => {
                            setDisabled(false)
                            dispatch(updateCurrentState({
                                ...current,
                                ismysqlRun: bool
                            }));
                            // eslint-disable-next-line @typescript-eslint/ban-ts-comment
                            //@ts-ignore
                            toast(`服务已经${bool ? "启动" : "停止"}`, { type: "info", position: "bottom-left" });
                        });
                    }, 100);
                });
            }


            function onSubmit(e: React.SubmitEvent<HTMLFormElement>) {
                e.preventDefault();
                console.log(e.nativeEvent);
                const { target, submitter } = e.nativeEvent;
                const form = target as HTMLFormElement;
                const formData = new FormData(form);
                formData.append("action", submitter?.getAttribute("name") || "")
                onAction(formData);
            }

            function onAction(formData: FormData) {
                const user = formData.get("user") as string;
                const password = formData.get("password") as string;
                const action = formData.get("action") as string;

                const mysqladminArgs = (() => {
                    if (password) {
                        return `-u${user} -p${password}`;
                    }
                    return `-u ${user}`;
                })();

                switch (action) {
                    case "stop_server":
                        // onToggle("Stop", mysqladminArgs)
                        break;
                    case "open_terminal":
                        DataSourceManager_OpenNewTerminal(services.mysql.outDir + "\\bin\\mysql.exe", mysqladminArgs, (bool) => {
                            console.log(bool);
                        })
                        break;
                }
            }

            if (current.ismysqlRun) {
                return < div className='nodrag nopan' >
                    {/* <form action={useForm}> */}
                    <form onSubmit={onSubmit}>
                        <div className='flex flex-col gap-y-1.5'>
                            <input
                                required
                                className='outline-none border-chart-1/25 border-2 pl-1'
                                placeholder='用户名(默认root)'
                                name="user"
                                type='text' />
                            <input
                                required
                                className='outline-none border-chart-1/25 border-2 pl-1'
                                placeholder='密码(日志有默认密码信息)'
                                name="password"
                                type='password' />
                            <Button name="stop_server" type='submit' variant="destructive" disabled={disabled}>终止服务</Button>
                            <Button name="open_terminal" type='submit'>终端MYSQL</Button>
                        </div>
                    </form>
                </div>
            }

            return < div className='nodrag nopan' >
                <Button disabled={disabled} onClickCapture={() => onToggle("Run", "")} >启动服务</Button>
            </div >
        }
    },
    type: "RMBaseNode",
};
export default function () {
    const dispatch = useDispatch();

    const services = useSelector((state: RootState) => state.services)
    const current = useSelector((state: RootState) => state.current)
    useEffect(() => {
        DataSourceManager_MYSQL_IsRun((bool) => {
            dispatch(updateCurrentState({
                ...current,
                ismysqlRun: bool
            }));
        })

        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, [dispatch]);

    const [edges, , onEdgesChange] = useEdgesState([
        {
            id: crypto.randomUUID(),
            source: "mysql_0",
            target: "node_0"
        },
        {
            id: crypto.randomUUID(),
            source: "mysql_0",
            target: "mysql_out_log"
        },
        {
            id: crypto.randomUUID(),
            source: "node_0",
            target: "node_1"
        }
    ]);
    const [nodes, , onNodesChange] = useNodesState([
        {
            type: "RMBaseNode",
            id: "mysql_0",
            position: { x: 140, y: 0 },
            data: {
                header: <>
                    <img src={sql} className="w-6" />
                    <BaseNodeHeaderTitle>MYSQL</BaseNodeHeaderTitle>
                </>,
                ContentComponent() {
                    const current = useSelector((state: RootState) => state.current)
                    useEffect(() => { }, [current])
                    return <div className='w-30 text-center'>
                        <span>服务状态</span>
                        <div className={`p-2 rounded-sm inline-block mt-1 ${current.ismysqlRun ? "bg-chart-3" : "bg-chart-1"}`}>
                            <Label>{current.ismysqlRun ? "运行中" : "未启动"}</Label>
                        </div>
                    </div>
                },
                footer: <LabeledHandle
                    id="mysql_0_out"
                    type="source"
                    title="配置"
                    position={Position.Bottom}
                />
            }
        },
        {
            type: "RMBaseNode",
            id: "mysql_out_log",
            position: { x: -100, y: 200 },
            data: {
                targetData: {
                    log: localStorage.getItem("mysql_out_log") || ""
                },
                HeaderComponent({ targetData }: BaseNodePropChildrenDataType) {
                    const [data, setData] = targetData;
                    return <>
                        <BaseHandle type="target" position={Position.Top} />
                        <Info className="size-5" />
                        <BaseNodeHeaderTitle>
                            日志
                        </BaseNodeHeaderTitle>
                        <Button className="nodrag nopan cursor-pointer" variant="destructive" onClick={() => {
                            setData({ ...data, log: "" })
                            localStorage.setItem("mysql_out_log", "")
                        }}>清空日志</Button>
                    </>
                },
                ContentComponent({ targetData }: BaseNodePropChildrenDataType) {
                    const [data, setData] = targetData;

                    useEffect(() => {
                        const onMessage = ({ data: d }: WebView2Event) => {
                            if (d && d.data.type == "MysqlLog") {
                                const msg = d.data.msg;
                                const log = data.log += `${msg}\n`;
                                setData({ ...data, log: log });
                                localStorage.setItem("mysql_out_log", log);
                            }
                        }

                        window.chrome.webview.addEventListener("message", onMessage);
                        return function () {
                            window.chrome.webview.removeEventListener("message", onMessage);
                        }
                    }, [data, setData])
                    return <Textarea
                        className='nodrag nopan w-90 min-h-50'
                        placeholder="服务日志..."
                        value={data.log}
                        onChange={() => { }}
                    />
                }
            }
        },
        {
            id: "node_0",
            position: { x: 300, y: 0 },
            data: {
                targetData: {
                    configLabel: `[mysqld]
console
basedir=${services.mysql.outDir.replaceAll("\\", "/")}
datadir=${services.mysql.outDir.replaceAll("\\", "/")}/data
port=3306
character-set-server=utf8mb4
collation-server=utf8mb4_general_ci
default-time-zone=+08:00
max_connections=200
max_allowed_packet=64M
sql_mode=STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION

[client]
port=3306
default-character-set=utf8mb4`
                },
                header: <>
                    <BaseHandle type="target" position={Position.Left} />
                    <BaseHandle type="source" position={Position.Bottom} />
                    <Edit className="size-5" />
                    <BaseNodeHeaderTitle>
                        My.init
                    </BaseNodeHeaderTitle>
                </>,
                ContentComponent({ targetData }: BaseNodePropChildrenDataType) {
                    if (targetData) {
                        const [data, setData] = targetData;
                        return <>
                            <Textarea
                                className='nodrag nopan nowheel w-110 h-70'
                                placeholder="my.ini配置"
                                value={data?.configLabel}
                                onChange={({ target }) => {
                                    const newValue = target.value;
                                    setData({
                                        ...data,
                                        configLabel: newValue,
                                    });
                                }}
                            />
                        </>
                    }
                }
            },
            type: "RMBaseNode",
        },
        {
            id: "node_1",
            position: { x: 300, y: 380 },
            data: {
                targetData: { initd: false },
                HeaderComponent({ targetData }: BaseNodePropChildrenDataType) {
                    const [data] = targetData;
                    return <>
                        <BaseHandle type="target" position={Position.Top} />
                        <BaseHandle className={data} type="source" position={Position.Right} />
                        <FileSliders className="size-5" />
                        <BaseNodeHeaderTitle>
                            配置管理
                        </BaseNodeHeaderTitle>
                    </>
                },
                ContentComponent({ targetData, sourceData }: BaseNodePropChildrenDataType) {
                    const [data, setTargetData] = targetData;
                    const [data_source] = sourceData;
                    const current = useSelector((state: RootState) => state.current)
                    const flow = useReactFlow();

                    // const [initd, setInitd] = useState(false);
                    const [disabled, setDisabled] = useState(false);
                    useEffect(() => {
                        DataSourceManager_MYSQL_Initd(bool => setTargetData({ ...data, initd: bool }));


                        if (data.initd) {
                            flow.addNodes(DataNode);
                            flow.addEdges({
                                id: crypto.randomUUID(),
                                source: "node_1",
                                target: "node_3",
                                // type: "default"
                            });

                        }
                        // eslint-disable-next-line react-hooks/exhaustive-deps
                    }, [data.initd, disabled, setDisabled, setTargetData, current.ismysqlRun, data_source, flow])

                    return <div className='nodrag nopan'>
                        <Button className="w-full" disabled={disabled || current.ismysqlRun} onClickCapture={() => {
                            setDisabled(true);
                            DataSourceManager_Injection("MYSQL", { key: "CreateConfig", val: data_source.configLabel }, () => {
                                setTimeout(() => {
                                    setDisabled(false);
                                }, 1000);
                            })
                        }}>{data.initd ? "重写" : "写入"}配置</Button>
                        {data.initd && <div className='bg-chart-3/40 p-2 rounded-sm mt-1'>
                            <Label>已写入</Label>
                        </div>}
                    </div>
                }
            },
            type: "RMBaseNode",
        },
        {
            id: "node_4",
            position: { x: 400, y: 480 },
            data: { id: '', data: "" },
            type: "RMAnnotationNode"
        }
    ]);

    return <XyFlow
        nodes={nodes}
        edges={edges}
        onEdgesChange={onEdgesChange}
        onNodesChange={onNodesChange}
    />
}