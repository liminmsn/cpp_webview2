import { Button, Input, Label, Switch } from "@heroui/react";
import { GetState, InstallService, OpenDir, OpenExe, RemoveService, Start, Stop } from "../util/mysql";
import { useEffect, useState } from "react";
import { useMysql } from "../context/MysqlContext";

export default function () {
    const { state, setState } = useMysql();
    const { mysql_path, installed, running } = state;
    const [disabled, setDisabled] = useState(false);

    function eventfun(funArr: ((state: any) => void)[], bol: boolean) {
        setDisabled(true)
        const call = funArr[(() => {
            return bol ? 1 : 0;
        })()]
        call(() => {
            setDisabled(false);
            GetState(setState as any);
        });
    }

    useEffect(() => {
        GetState(setState as any);
    }, [])

    return <div className="h-full p-3">
        <div className="flex gap-1">
            <Input value={mysql_path} className="text-[10pt] w-full" dir="rtl" onChange={() => { }} />
            <Button size="sm" variant="tertiary" onClick={() => OpenDir(mysql_path)}>目录</Button>
            <Button size="sm" variant="tertiary" onClick={() => OpenExe("Powershell")}>终端</Button>
        </div>
        <div className="flex gap-1 mt-1">
            <Input value={mysql_path.concat("\\my.ini")} className="text-[10pt] w-full" dir="rtl" onChange={() => { }} />
            <Button size="sm" variant="tertiary" onClick={() => OpenExe("EditInI")}>编辑配置</Button>
        </div>
        <div className="flex flex-col gap-y-1.5 mt-1">
            <Label>服务安装状态:
                {installed && <sup className="text-success">ON</sup>}
                {!installed && <sup className="text-danger">OFF</sup>}
            </Label>
            <Button size="sm" variant={installed ? "danger" : "primary"} onPress={() => eventfun([InstallService, RemoveService], installed)}>
                {!installed && "安装服务"}
                {installed && "删除服务"}
            </Button>
            <Label>服务运行状态:
                {running && <sup className="text-success">ON</sup>}
                {!running && <sup className="text-danger">OFF</sup>}
            </Label>
            <div className={`
                ${running ? 'bg-warning' : 'bg-accent'}
                select-none
                rounded-[30vh]
                w-30
                h-30
                flex flex-col
                gap-y-1
                justify-center
                items-center
                mx-auto
                shadow-md`}>
                <Switch isDisabled={!installed || disabled} isSelected={running} onChange={(isSelectd) => eventfun([Stop, Start], isSelectd)}>
                    <Switch.Content>
                        <Switch.Control>
                            <Switch.Thumb />
                        </Switch.Control>
                    </Switch.Content>
                </Switch>
            </div>
        </div>
    </div>
}