import { DataSourceManager_Injection, DataSourceManager_MYSQL_Initd, DataSourceManager_MYSQL_IsRun, DataSourceManager_MYSQL_Run } from '@/event/DataSourceManager';
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
import { Edit, FileSliders, Info, Rocket } from 'lucide-react';
import { useDispatch, useSelector } from 'react-redux';
import { useNodesState, Position, useEdgesState } from '@xyflow/react';
import { useEffect, useState } from 'react';
import sql from "@/assets/sql.png";
import '@xyflow/react/dist/style.css';
import { toast } from 'sonner';


export default function () {
    const services = useSelector((state: RootState) => state.services)


    const dispatch = useDispatch();
    const current = useSelector((state: RootState) => state.current)
    useEffect(() => {
        DataSourceManager_MYSQL_IsRun((bool) => {
            dispatch(updateCurrentState({
                ...current,
                ismysqlRun: bool
            }));
        })
    }, []);

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
        },
        {
            id: crypto.randomUUID(),
            source: "node_1",
            target: "node_2",
            type: "default"
        }
    ]);
    const [nodes, , onNodesChange] = useNodesState([
        {
            type: "baseNodeFull",
            id: "mysql_0",
            position: { x: 140, y: 0 },
            data: {
                header: <>
                    <img src={sql} className="w-6" />
                    <BaseNodeHeaderTitle>MYSQL</BaseNodeHeaderTitle>
                </>,
                ContentComponent: function () {
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
            type: "baseNodeFull",
            id: "mysql_out_log",
            position: { x: -100, y: 200 },
            data: {
                targetData: {
                    log: localStorage.getItem("mysql_out_log") || ""
                },
                HeaderComponent: function ({ targetData }: BaseNodePropChildrenDataType) {
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
                ContentComponent: function ({ targetData }: BaseNodePropChildrenDataType) {
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
                        className='nodrag nopan nowheel w-90 min-h-50 text-[10px]!'
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
                ContentComponent: function ({ targetData }: BaseNodePropChildrenDataType) {
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
            type: "baseNodeFull",
        },
        {
            id: "node_1",
            position: { x: 300, y: 380 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Top} />
                    <BaseHandle type="source" position={Position.Right} />
                    <FileSliders className="size-5" />
                    <BaseNodeHeaderTitle>
                        配置管理
                    </BaseNodeHeaderTitle>
                </>,
                ContentComponent: function ({ sourceData }: BaseNodePropChildrenDataType) {
                    const [data] = sourceData;

                    const [initd, setInitd] = useState(false);
                    const [disabled, setDisabled] = useState(false);
                    useEffect(() => {
                        DataSourceManager_MYSQL_Initd(setInitd);
                    }, [])

                    if (initd) {
                        return <div className='bg-chart-3/40 p-2 rounded-sm'>
                            <Label>已写入my.ini</Label>
                        </div>
                    }

                    return <div className='nodrag nopan'>
                        <Button disabled={disabled} onClickCapture={() => {
                            setDisabled(true);
                            DataSourceManager_Injection("MYSQL", { key: "CreateConfig", val: data.configLabel }, () => {
                                setTimeout(() => {
                                    setDisabled(false);
                                }, 1000);
                            })
                        }}>写入my.ini</Button>
                    </div>
                }
            },
            type: "baseNodeFull",
        },
        {
            id: "node_2",
            position: { x: 300, y: 480 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Left} />
                    <Rocket className="size-5" />
                    <BaseNodeHeaderTitle>
                        运行
                    </BaseNodeHeaderTitle>
                </>,
                ContentComponent: function () {
                    const dispatch = useDispatch();
                    const current = useSelector((state: RootState) => state.current)
                    const [disabled, setDisabled] = useState(false);
                    useEffect(() => { }, [current, dispatch]);

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
                                    toast(`服务已经${bool ? "启动" : "停止"}`, { position: "top-right" });
                                });
                            }, 100);
                        });
                    }

                    function useForm(formData: FormData) {
                        const user = formData.get("user") as any;
                        const password = formData.get("password") as any;

                        const mysqladminArgs = (() => {
                            if (password != "") {
                                return `--password=${password} -u ${user}`;
                            }
                            return `-u ${user}`;
                        })();
                        onToggle("Stop", mysqladminArgs);
                    }
                    return < div className='nodrag nopan' >
                        {
                            current.ismysqlRun ?
                                <form action={useForm}>
                                    <div className='flex flex-col gap-y-1.5'>
                                        <input
                                            className='outline-none border-chart-1/25 border-2 pl-1'
                                            placeholder='用户名(默认root无密码)'
                                            required
                                            name="user"
                                            type='text' />
                                        <input
                                            className='outline-none border-chart-1/25 border-2 pl-1'
                                            placeholder='密码(可选)'
                                            name="password"
                                            type='password' />
                                        <Button disabled={disabled} type='submit' variant="destructive">终止服务</Button>
                                    </div>
                                </form> :
                                <Button disabled={disabled} onClickCapture={() => onToggle("Run", "")} > 启动服务</Button>
                        }

                    </div >
                }
            },
            type: "baseNodeFull",
        }
    ]);

    return <XyFlow
        nodes={nodes}
        edges={edges}
        onEdgesChange={onEdgesChange}
        onNodesChange={onNodesChange}
    />
}