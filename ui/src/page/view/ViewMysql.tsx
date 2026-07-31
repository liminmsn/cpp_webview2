import { ReactFlow, Background, Controls, useNodesState, Position, useEdgesState } from '@xyflow/react';
import { defaultEdgeOptions, nodeTypes } from '@/xyflow';
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
import type { RootState } from '@/store/store';
import { useSelector } from 'react-redux';


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
            target: "node_2"
        }
    ]);
    const [nodes, , onNodesChange] = useNodesState([
        {
            type: "baseNodeFull",
            id: "mysql_0",
            position: { x: 60, y: 0 },
            data: {
                header: <>
                    <img src={sql} className="w-6" />
                    <BaseNodeHeaderTitle>Mysql</BaseNodeHeaderTitle>
                </>,
                content: <div className='w-30 text-center'>
                    <span>运行状态</span>
                    <div>
                        <Badge variant="destructive">
                            OFF
                        </Badge>
                    </div>
                </div>,
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
            position: { x: -20, y: 210 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Top} />
                    <Info className="size-4" />
                    <BaseNodeHeaderTitle>
                        输出日志
                    </BaseNodeHeaderTitle>
                    <Button variant="destructive">清空日志</Button>
                </>,
                content: <div className='h-50 overflow-y-auto nodrag nopan'>
                    <Textarea className='max-w-70 w-70 h-full' placeholder='这里是mysql服务输出的所有日志...' />
                </div>
            }
        },
        {
            id: "node_0",
            position: { x: 300, y: 0 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Left} />
                    <BaseHandle type="source" position={Position.Bottom} />
                    <Edit className="size-4" />
                    <BaseNodeHeaderTitle>
                        My.init
                    </BaseNodeHeaderTitle>
                </>,
                content: (() => {
                    return <Textarea className='nodrag nopan w-110 h-70' placeholder="Type your message here." defaultValue={
                        `[mysqld]
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
                    } />
                })()
            },
            type: "baseNodeFull",
        },
        {
            id: "node_1",
            position: { x: 300, y: 370 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Top} />
                    <BaseHandle type="source" position={Position.Right} />
                    <Server className="size-4" />
                    <BaseNodeHeaderTitle>
                        服务
                    </BaseNodeHeaderTitle>
                </>,
                content: <div className='nodrag nopan'>
                    <div className="flex items-center space-x-2">
                        <Label htmlFor="airplane-mode">ON|OFF:</Label>
                        <Switch id="airplane-mode" />
                    </div>
                    <Button onClickCapture={() => {

                    }}>测试</Button>
                </div>
            },
            type: "baseNodeFull",
        },
        {
            id: "node_2",
            position: { x: 450, y: 370 },
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
                    <Button onClickCapture={() => {

                    }}>测试</Button>
                </div>
            },
            type: "baseNodeFull",
        }
    ]);

    return <div className='h-full w-full'>
        <ReactFlow
            nodes={nodes}
            nodeTypes={nodeTypes}
            edges={edges}
            defaultEdgeOptions={defaultEdgeOptions}
            onEdgesChange={onEdgesChange}
            onNodesChange={onNodesChange}
            nodesDraggable={false}
            fitView
            fitViewOptions={{
                padding: 1.2
            }}
        >
            <Background />
            <Controls
                showZoom={false}
                showInteractive={false}
            />
        </ReactFlow>
    </div>
}