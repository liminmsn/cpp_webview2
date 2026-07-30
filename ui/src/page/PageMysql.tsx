import type { RootState } from "@/store/store";
import { useSelector } from "react-redux";
import Empty from "./Empty";
import ViewMysql from "./view/ViewMysql";

export default function () {
    const services = useSelector((state: RootState) => state.services)

    if (services.mysql.InitialState)
        return <ViewMysql />
    return <Empty InitKey="InitMysql" />
}