import { useNodesState, Position, useEdgesState } from '@xyflow/react';
import '@xyflow/react/dist/style.css';
import { BaseNodeHeaderTitle } from '@/components/base-node';
import redis from "@/assets/redis.png";
import { Badge } from '@/components/ui/badge';
import { LabeledHandle } from '@/components/labeled-handle';
import { useSelector } from 'react-redux';
import XyFlow from '@/components/XyFlow';
import type { RootState } from '@/store/store';
import type { BaseNodePropChildrenDataType } from '@/components/XyFlow/RMBaseNode';


export default function () {
    useSelector((state: RootState) => state.services)

    const [edges, , onEdgesChange] = useEdgesState([
        {
            id: crypto.randomUUID(),
            source: "mysql_0",
            target: "node_0"
        }
    ]);

    const [nodes, , onNodesChange] = useNodesState([
        {
            type: "baseNodeFull",
            id: "mysql_0",
            position: { x: 140, y: 0 },
            data: {
                header: <>
                    <img src={redis} className="w-6" />
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
        }
    ]);

    return <XyFlow
        nodes={nodes}
        edges={edges}
        onEdgesChange={onEdgesChange}
        onNodesChange={onNodesChange}
    />
}