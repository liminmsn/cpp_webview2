import type { DefaultEdgeOptions, NodeTypes } from "@xyflow/react";
import RMBaseNode from "./RMBaseNode";

const nodeTypes: NodeTypes = {
    baseNodeFull: RMBaseNode
};

const defaultEdgeOptions: DefaultEdgeOptions = {
    type: "smoothstep",
    style: {
        strokeWidth: 1.2,
        stroke: "var(--color-primary)",
        // filter: "drop-shadow(0 0 3px var(--color-primary))"
    }
    // animated: true,
}

export { nodeTypes, defaultEdgeOptions };