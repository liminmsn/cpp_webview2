import type { RootState } from "@/store/store";
import { useDispatch, useSelector } from "react-redux";
import Empty from "./Empty";
import ViewMysql from "./view/ViewMysql";
import { DataSourceManager_GetState } from "@/event/DataSourceManager";
import { updateState } from "@/store/features/serviceSlice";
import { useEffect } from "react";

export default function () {
    const dispatch = useDispatch()
    const services = useSelector((state: RootState) => state.services)

    useEffect(() => {
        DataSourceManager_GetState((state) => dispatch(updateState(state)))
    }, [dispatch]);


    if (services.mysql.InitialState)
        return <ViewMysql />
    return <Empty InitKey="InitMysql" />
}