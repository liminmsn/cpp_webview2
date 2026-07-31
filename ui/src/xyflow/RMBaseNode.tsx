import {
  BaseNode,
  BaseNodeContent,
  BaseNodeFooter,
  BaseNodeHeader,
  // BaseNodeHeaderTitle,
} from "@/components/base-node";
import { NodeStatusIndicator } from "@/components/node-status-indicator";
import { memo, type ReactNode } from "react";

type BaseNodePropDataType = {
  header?: ReactNode;
  content?: ReactNode;
  footer?: ReactNode;
}
type BaseNodePropType = {
  data: BaseNodePropDataType
}

const baseNode = memo(({ data }: BaseNodePropType) => {
  return <NodeStatusIndicator status="initial" variant="overlay">
    <BaseNode className="shadow-sm">
      {
        data.header &&
        <BaseNodeHeader className="border-b">
          {data.header}
        </BaseNodeHeader>
      }
      {
        data.content &&
        <BaseNodeContent>
          {data.content}
        </BaseNodeContent>
      }
      {
        data.footer &&
        <BaseNodeFooter className="border-t">
          {data.footer}
        </BaseNodeFooter>
      }
    </BaseNode>
  </NodeStatusIndicator>
});

baseNode.displayName = "RMBaseNode";
export default baseNode;