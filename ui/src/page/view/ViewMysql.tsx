import { ReactFlow, Background, Controls, useNodesState, Position, useEdgesState } from '@xyflow/react';
import { defaultEdgeOptions, nodeTypes } from '@/xyflow';
import '@xyflow/react/dist/style.css';
import { BaseNodeHeaderTitle } from '@/components/base-node';
import { Edit, Info, Rocket } from 'lucide-react';
import sql from "@/assets/sql.png";
import { Badge } from '@/components/ui/badge';
import { LabeledHandle } from '@/components/labeled-handle';
import { BaseHandle } from '@/components/base-handle';
import { Textarea } from '@/components/ui/textarea';
import { Switch } from '@/components/ui/switch';
import { Label } from '@/components/ui/label';
import { Button } from '@/components/ui/button';


export default function () {
    const [edges, setEdges, onEdgesChange] = useEdgesState([
        {
            id: "mysql_to_node",
            source: "mysql_0",
            target: "node_0"
        },
        {
            id: "mysql_to_log",
            source: "mysql_0",
            target: "mysql_out_log",
            // type: "default"
        },
        {
            id: "mysql_to_node1",
            source: "node_0",
            target: "node_1",
            type: "straight"
        }
    ]);
    const [nodes, , onNodesChange] = useNodesState([
        {
            type: "baseNodeFull",
            id: "mysql_0",
            position: { x: 180, y: 0 },
            data: {
                header: <>
                    <img src={sql} className="w-6" />
                    <BaseNodeHeaderTitle>Mysql</BaseNodeHeaderTitle>
                </>,
                content: <div>
                    <span>服务状态</span>
                    <div className="text-center">
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
                content: <div className='h-50 overflow-y-auto'>
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
                content: <Textarea className='text-sm' placeholder="Type your message here." defaultValue={
                    `[mysqld]
console
basedir=C:/Users/limin/AppData/Local/LocalMysql/mysql
datadir=C:/Users/limin/AppData/Local/LocalMysql/mysql/data
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
            },
            type: "baseNodeFull",
        },
        {
            id: "node_1",
            position: { x: 300, y: 400 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Top} />
                    <Rocket className="size-4" />
                    <BaseNodeHeaderTitle>
                        运行
                    </BaseNodeHeaderTitle>
                </>,
                content: <div>
                    <div className="flex items-center space-x-2">
                        <Label htmlFor="airplane-mode">ON|OFF:</Label>
                        <Switch id="airplane-mode" />
                    </div>
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
            nodesDraggable={!false}
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