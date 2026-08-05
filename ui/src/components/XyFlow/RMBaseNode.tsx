import {
  BaseNode,
  BaseNodeContent,
  BaseNodeFooter,
  BaseNodeHeader,
  // BaseNodeHeaderTitle,
} from "@/components/base-node";
import { NodeStatusIndicator } from "@/components/node-status-indicator";
import { useReactFlow } from "@xyflow/react";
import { memo, useState, type ReactNode } from "react";

export type BaseNodePropChildrenDataType = {
  targetData: [data: any | undefined, (data: any) => void | undefined];
  sourceData: BaseNodePropChildrenDataType["targetData"];
};

type BaseNodePropDataType = {
  header?: ReactNode;
  content?: ReactNode;
  footer?: ReactNode;
  HeaderComponent?: (prop: BaseNodePropChildrenDataType) => ReactNode;
  ContentComponent?: (prop: BaseNodePropChildrenDataType) => ReactNode;
  FooterComponent?: (prop: BaseNodePropChildrenDataType) => ReactNode;
}
type BaseNodePropType = {
  id: string
  data: BaseNodePropDataType & BaseNodePropChildrenDataType
}

const baseNode = memo(({ id, data }: BaseNodePropType) => {
  const flow = useReactFlow();

  const targetDataParent = flow.getEdges().filter(item => item.target == id).map(edges => {
    return flow.getNode(edges.source)?.data?.targetData;
  })[0] as BaseNodePropChildrenDataType["targetData"];

  data.targetData = useState(data.targetData);

  return <NodeStatusIndicator status="initial" variant="overlay">
    <BaseNode className="shadow-sm">
      {
        data.header &&
        <BaseNodeHeader className="border-b">
          {data.header}
        </BaseNodeHeader>
        || data.HeaderComponent &&
        <BaseNodeHeader className="border-b">
          <data.HeaderComponent
            targetData={data.targetData}
            sourceData={targetDataParent}
          />
        </BaseNodeHeader>
      }
      {
        <BaseNodeContent>
          {data.content && data.content}
          {data.ContentComponent &&
            <data.ContentComponent
              targetData={data.targetData}
              sourceData={targetDataParent}
            />
          }
        </BaseNodeContent>
      }
      {
        (data.footer || data.FooterComponent) &&
        <BaseNodeFooter className="border-t">
          {data.footer && data.footer}
          {data.FooterComponent &&
            <data.FooterComponent
              targetData={data.targetData}
              sourceData={targetDataParent}
            />
          }
        </BaseNodeFooter>
      }
    </BaseNode>
  </NodeStatusIndicator>
});

baseNode.displayName = "RMBaseNode";
export default baseNode;