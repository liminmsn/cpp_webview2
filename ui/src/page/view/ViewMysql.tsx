import { useNodesState, Position, useEdgesState } from '@xyflow/react';
import '@xyflow/react/dist/style.css';
import { BaseNodeHeaderTitle } from '@/components/base-node';
import { Edit, Info, Rocket, Server } from 'lucide-react';
import sql from "@/assets/sql.png";
import { Badge } from '@/components/ui/badge';
import { LabeledHandle } from '@/components/labeled-handle';
import { BaseHandle } from '@/components/base-handle';
import { Textarea } from '@/components/ui/textarea';
import { Button } from '@/components/ui/button';
import { useDispatch, useSelector } from 'react-redux';
import { DataSourceManager_Injection, DataSourceManager_MYSQL_Initd } from '@/event/DataSourceManager';
import { useEffect, useState } from 'react';
import XyFlow from '@/components/XyFlow';
import type { RootState } from '@/store/store';
import type { BaseNodePropChildrenDataType } from '@/components/XyFlow/RMBaseNode';
import { updateCurrentState } from '@/store/features/currentSlice';


export default function () {
    const services = useSelector((state: RootState) => state.services)

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
                    <BaseNodeHeaderTitle>Mysql</BaseNodeHeaderTitle>
                </>,
                ContentComponent: function () {

                    return <div className='w-30 text-center'>
                        <span>服务状态</span>
                        <div>
                            <Badge variant="destructive">
                                未安装
                            </Badge>
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
            position: { x: -140, y: 200 },
            data: {
                HeaderComponent: function () {
                    const dispatch = useDispatch()
                    const current = useSelector((state: RootState) => state.current)
                    return <>
                        <BaseHandle type="target" position={Position.Top} />
                        <Info className="size-4" />
                        <BaseNodeHeaderTitle>
                            日志
                        </BaseNodeHeaderTitle>
                        <Button className="nodrag nopan cursor-pointer" variant="destructive" onClick={() => {
                            dispatch(updateCurrentState({ ...current, mysql: { log: "" } }))
                        }}>清空日志</Button>
                    </>
                },
                ContentComponent: function () {
                    const dispatch = useDispatch()
                    const current = useSelector((state: RootState) => state.current)

                    useEffect(() => {
                        const onMessage = ({ data: d }: WebView2Event) => {
                            if (d && d.data.type == "MysqlLog") {
                                const msg = d.data.msg;
                                current.mysql.log = current.mysql.log += `${msg}\n`;
                                dispatch(updateCurrentState(current));
                            }
                        }

                        window.chrome.webview.addEventListener("message", onMessage);
                        return function () {
                            window.chrome.webview.removeEventListener("message", onMessage);
                        }
                    }, [current, dispatch])
                    return <div className='h-80 overflow-y-auto nodrag nopan'>
                        <Textarea
                            className='nodrag nopan nowheel w-100 h-full text-[10px]!'
                            placeholder="服务日志..."
                            value={current.mysql.log}
                            onChange={() => { }}
                        />
                    </div>
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
                    <Edit className="size-4" />
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
            position: { x: 300, y: 375 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Top} />
                    <BaseHandle type="source" position={Position.Right} />
                    <Server className="size-4" />
                    <BaseNodeHeaderTitle>
                        服务管理
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
                        return <div>
                            <div>
                                <Badge variant="secondary" className="bg-chart-3">
                                    配置完成
                                </Badge>
                            </div>
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
                        }}>初始化</Button>
                    </div>
                }
            },
            type: "baseNodeFull",
        },
        {
            id: "node_2",
            position: { x: 430, y: 375 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Left} />
                    <Rocket className="size-4" />
                    <BaseNodeHeaderTitle>
                        运行
                    </BaseNodeHeaderTitle>
                </>,
                ContentComponent: function () {
                    return < div className='nodrag nopan' >
                        {/* <div className='flex gap-x-1.5 items-center mb-0.5'>
                            <Label htmlFor="airplane-mode">状态:</Label>
                            {
                                state ?
                                    <Badge variant="secondary" className="bg-chart-3">ON</Badge> :
                                    <Badge variant="secondary" className="bg-chart-2">OFF</Badge>
                            }
                        </div>
                        <Switch id="airplane-mode" onCheckedChange={(checked) => {
                            DataSourceManager_MYSQL_Run(checked ? "Run" : "Stop", setState);
                        }} /> */}
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