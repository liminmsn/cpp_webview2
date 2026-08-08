import { DataSourceManager_Injection, DataSourceManager_MYSQL_Initd, DataSourceManager_MYSQL_InitdServer, DataSourceManager_MYSQL_InitializeMysql, DataSourceManager_MYSQL_IsRun, DataSourceManager_MYSQL_Run, DataSourceManager_OpenNewTerminal, DataSourceManager_OpenWithExplorer } from '@/event/DataSourceManager';
import { useNodesState, Position, useEdgesState, useReactFlow } from '@xyflow/react';
import { updateCurrentState } from '@/store/features/currentSlice';
import { BaseNodeHeaderTitle } from '@/components/base-node';
import { LabeledHandle } from '@/components/labeled-handle';
import { BaseHandle } from '@/components/base-handle';
import { Textarea } from '@/components/ui/textarea';
import { Button } from '@/components/ui/button';
import { Label } from '@/components/ui/label';
import XyFlow from '@/components/XyFlow';
import type { BaseNodePropChildrenDataType } from '@/components/XyFlow/RMBaseNode';
import type { RootState } from '@/store/store';
import { Edit, FileSliders, Folder, Info, Rocket } from 'lucide-react';
import { useDispatch, useSelector } from 'react-redux';
import { useEffect, useState } from 'react';
import sql from "@/assets/sql.png";
import '@xyflow/react/dist/style.css';
/**Data管理文件夹 */
const DataNode = {
    id: "node_3",
    position: { x: 350, y: 380 },
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
                }}>初始化</Button>
            </div>
        },
        type: "default"
    },
    type: "RMBaseNode"
}
/***运行节点**/
const RunNode = {
    id: "node_2",
    position: { x: 520, y: 380 },
    data: {
        header: <>
            <BaseHandle type="target" position={Position.Left} />
            <Rocket className="size-5" />
            <BaseNodeHeaderTitle>
                运行
            </BaseNodeHeaderTitle>
        </>,
        ContentComponent() {
            const flow = useReactFlow()
            const dispatch = useDispatch();

            const services = useSelector((state: RootState) => state.services)
            const current = useSelector((state: RootState) => state.current)
            const [disabled, setDisabled] = useState(false);

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
                            // toast(`服务已经${bool ? "启动" : "停止"}`, { type: "info", position: "bottom-left" });
                        });
                    }, 1000);
                });
            }


            function onSubmit(e: React.SubmitEvent<HTMLFormElement>) {
                e.preventDefault();
                const { target, submitter } = e.nativeEvent;
                const form = target as HTMLFormElement;
                const formData = new FormData(form);
                formData.append("action", submitter?.getAttribute("name") || "")


                const username = formData.get("user") as string;
                const password = formData.get("password") as string;
                const action = formData.get("action") as string;

                const mysqladminArgs = `-u ${username} -p'${password}'`;
                if (action === "stop_server") {
                    onToggle("Stop", `${mysqladminArgs.replaceAll("'", '')} shutdown`)
                } else if (action === "open_terminal") {
                    DataSourceManager_OpenNewTerminal(`${services.mysql.outDir}\\bin\\mysql.exe`, mysqladminArgs, (bool) => {
                        console.log(bool);
                    })
                }
            }

            useEffect(() => {
                // flow.fitView()
                // setTimeout(() => {
                // }, 1000);
            }, [flow, current.ismysqlRun, disabled, dispatch]);

            if (current.ismysqlRun) {
                return < div className='nodrag nopan' >
                    <form onSubmit={onSubmit}>
                        <div className='flex flex-col gap-y-1.5'>
                            <input
                                alt=''
                                required
                                className='outline-none border-chart-1/25 border-2 pl-1'
                                placeholder='用户名(默认root)'
                                name="user"
                                type='text' />
                            <input
                                required
                                className='outline-none border-chart-1/25 border-2 pl-1'
                                placeholder='密码(注意多余空格)'
                                name="password"
                                type='password' />
                            <Button name="open_terminal" type='submit'>终端MYSQL</Button>
                            <Button name="stop_server" type='submit' variant="destructive" disabled={disabled}>停止服务</Button>
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
            position: { x: 0, y: 0 },
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
            position: { x: -180, y: 200 },
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
                        <Button className="nodrag nopan nowheel cursor-pointer" variant="destructive" onClick={() => {
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
                        className='nodrag nopan nowheel w-90 min-h-60 max-h-60'
                        placeholder="服务日志..."
                        value={data.log}
                        onChange={() => { }}
                    />
                }
            }
        },
        {
            id: "node_0",
            position: { x: 220, y: 0 },
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
            position: { x: 220, y: 380 },
            data: {
                targetData: { initd: false },
                HeaderComponent({ targetData }: BaseNodePropChildrenDataType) {
                    const [data] = targetData;
                    return <>
                        <BaseHandle type="target" position={Position.Top} />
                        {data.initd && <BaseHandle type="source" position={Position.Right} />}
                        <FileSliders className="size-5" />
                        <BaseNodeHeaderTitle>
                            配置
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
                                }, 100);
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
            id: "node_label_0",
            position: { x: 200, y: 520 },
            data: {
                label: <div className='max-w-35 font_zhka border-r border-accent'>
                    <span className='text-primary'>重写配置</span>
                    <p>会删除Data目录（myql的所有数据）请备份好数据慎重重写配置</p>
                </div>
            },
            type: "RMAnnotationNode"
        },
        {
            id: "node_label_1",
            position: { x: 350, y: 520 },
            data: {
                label: <div className='max-w-35 font_zhka'>
                    <span className='text-primary'>Data目录</span>
                    <p>初始化日志Info会携带初始<span className='text-primary'>临时密码</span></p>
                </div>
            },
            type: "RMAnnotationNode"
        },
        {
            id: "node_label_2",
            position: { x: -190, y: 750 },
            data: {
                label: <div className='max-w-110'>
                    <span className='text-primary font_zhka'>忘记密码【解决方式】</span>
                    <p><span className='font_zhka'>1.管理员运行命令窗口执行</span><span className='text-primary'>taskkill /IM mysqld.exe /F</span></p>
                    <p className='py-1'><span className='mr-1'>2.</span>
                        <Button size='sm' variant='outline' onClick={() => {
                            DataSourceManager_OpenWithExplorer(services.mysql.outDir, (bool) => {
                                console.log(bool);
                            })
                        }}>打开mysql目录</Button>
                    </p>
                    <p className='font_zhka'>3.删除目录下的data文件夹、my.ini文件</p>
                    <p className='font_zhka text-chart-3'>4.重启程序</p>
                </div>
            },
            type: "RMAnnotationNode"
        },
        {
            id: "node_label_3",
            position: { x: -190, y: 610 },
            data: {
                label: <ol className='flex flex-col gap-y-1max-w-80'>
                    <li className='text-primary font_zhka'>第一次运行</li>
                    <li className='text-primary font_zhka'>服务启动第一件事！！！</li>
                    <li className='font_zhka'>1.输入用户名、临时密码。点击终端mysql</li>
                    <li><span className='font_zhka mr-2 select-text'>2.创建永久密码执行</span><span>ALTER USER root@localhost IDENTIFIED BY '你需要设置的密码';</span></li>
                    <li className='font_zhka text-chart-3'>3.本地mysql正常【停止、运行】服务</li>
                </ol>
            },
            type: "RMAnnotationNode"
        },
    ]);

    return <XyFlow
        nodes={nodes}
        edges={edges}
        onEdgesChange={onEdgesChange}
        onNodesChange={onNodesChange}
    />
}