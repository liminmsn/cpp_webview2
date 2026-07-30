import type { RootState } from "@/store/store";
import { useSelector } from "react-redux";
import Empty from "./Empty";
import ViewRedis from "./view/ViewRedis";

export default function () {
    const services = useSelector((state: RootState) => state.services)

    if (services.redis.InitialState)
        return <ViewRedis />
    return <Empty InitKey="InitRedis" />
}