import { nodeTypes, defaultEdgeOptions } from "@/xyflow";
import { ReactFlow, Background, Controls, useEdgesState, useNodesState } from "@xyflow/react";

export default function () {
    const [edges, setEdges, onEdgesChange] = useEdgesState([
        {
            id: "mysql_to_node",
            source: "mysql_0",
            target: "node_0"
        }
    ]);
    const [nodes, , onNodesChange] = useNodesState([

    ]);

    return <ReactFlow
        nodes={nodes}
        edges={edges}
        nodeTypes={nodeTypes}
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
}