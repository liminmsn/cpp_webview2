import { memo } from "react";

export type RMAnnotationNode = {
    id: string
    data: any
}

export default memo(({ id, data }: RMAnnotationNode) => {
    return <div>
        注释节点
        {JSON.stringify(id)}
        {JSON.stringify(data)}
    </div>
});