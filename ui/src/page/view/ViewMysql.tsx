import { useNodesState, Position, useEdgesState } from '@xyflow/react';
import '@xyflow/react/dist/style.css';
import { BaseNodeHeaderTitle } from '@/components/base-node';
import { Edit, Info, Rocket, Server } from 'lucide-react';
import sql from "@/assets/sql.png";
import { Badge } from '@/components/ui/badge';
import { LabeledHandle } from '@/components/labeled-handle';
import { BaseHandle } from '@/components/base-handle';
import { Textarea } from '@/components/ui/textarea';
import { Switch } from '@/components/ui/switch';
import { Label } from '@/components/ui/label';
import { Button } from '@/components/ui/button';
import { useSelector } from 'react-redux';
import { DataSourceManager_Injection } from '@/event/DataSourceManager';
import { useEffect } from 'react';
import { toast } from 'sonner';
import XyFlow from '@/components/XyFlow';
import type { RootState } from '@/store/store';
import type { BaseNodePropChildrenDataType } from '@/components/XyFlow/RMBaseNode';


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
                ContentComponent: () => {
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
            position: { x: -20, y: 200 },
            data: {
                targetData: {
                    logLabel: ""
                },
                HeaderComponent: function ({ targetData }: BaseNodePropChildrenDataType) {
                    if (targetData) {
                        const [, setData] = targetData;
                        return <>
                            <BaseHandle type="target" position={Position.Top} />
                            <Info className="size-4" />
                            <BaseNodeHeaderTitle>
                                输出日志
                            </BaseNodeHeaderTitle>
                            <Button className="nodrag nopan cursor-pointer" variant="ghost" onClick={() => setData({ logLabel: "" })}>清空日志</Button>
                        </>
                    }
                },
                ContentComponent: function ({ targetData }: BaseNodePropChildrenDataType) {
                    const [data_log, setData] = targetData;

                    const onMessage = ({ data: d }: WebView2Event) => {
                        setData({ data_log, logLabel: JSON.stringify(d) });
                    }
                    useEffect(() => {
                        window.chrome.webview.addEventListener("message", onMessage);
                        return function () {
                            window.chrome.webview.removeEventListener("message", onMessage);
                        }
                    }, [])
                    return <div className='h-50 overflow-y-auto nodrag nopan'>
                        <Textarea
                            className='nodrag nopan max-w-70 w-70 h-full'
                            placeholder="服务日志..."
                            value={data_log?.logLabel}
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
basedir=${services.mysql.outDir}
datadir=${services.mysql.outDir}\\data
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
                                className='nodrag nopan w-110 h-70'
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
                    if (sourceData) {
                        const [data] = sourceData;
                        return <div className='nodrag nopan'>
                            <Button onClickCapture={() => {
                                DataSourceManager_Injection("MYSQL",
                                    { key: "CreateConfig", val: data.configLabel },
                                    (data) => toast(data))
                            }}>安装服务</Button>
                        </div>
                    }
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
                content: <div className='nodrag nopan'>
                    <div className="flex items-center space-x-2">
                        <Label htmlFor="airplane-mode">ON|OFF:</Label>
                        <Switch id="airplane-mode" />
                    </div>
                </div>
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