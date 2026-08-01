import { ReactFlow, Background, Controls } from "@xyflow/react";
import { nodeTypes, defaultEdgeOptions } from "./config";

type Props = {
    nodes: any;
    edges: any;
    onEdgesChange: any;
    onNodesChange: any;
}

export default function ({ nodes, edges, onEdgesChange, onNodesChange }: Props) {
    return <div className='h-full w-full'>
        <ReactFlow
            nodes={nodes}
            edges={edges}
            onEdgesChange={onEdgesChange}
            onNodesChange={onNodesChange}
            nodeTypes={nodeTypes}
            defaultEdgeOptions={defaultEdgeOptions}
            nodesDraggable={false}
            fitView
        >
            <Background />
            <Controls
                showZoom={false}
                showInteractive={false}
            />
        </ReactFlow>
    </div>
}