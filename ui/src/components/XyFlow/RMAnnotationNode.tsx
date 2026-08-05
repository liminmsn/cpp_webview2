import { memo } from "react";

export type RMAnnotationNode = {
    id: string
    data: any
}

export default memo(({ data }: RMAnnotationNode) => {
    return <div className="">
        {data["label"]}
    </div>
});