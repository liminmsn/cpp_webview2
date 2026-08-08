import { useNodesState, Position, useEdgesState } from '@xyflow/react';
import '@xyflow/react/dist/style.css';
import { BaseNodeHeaderTitle } from '@/components/base-node';
import redis from "@/assets/redis.png";
import { LabeledHandle } from '@/components/labeled-handle';
import { useDispatch, useSelector } from 'react-redux';
import XyFlow from '@/components/XyFlow';
import type { RootState } from '@/store/store';
import { BaseHandle } from '@/components/base-handle';
import { Textarea } from '@/components/ui/textarea';
import { updateCurrentState } from '@/store/features/currentSlice';
import type { BaseNodePropChildrenDataType } from '@/components/XyFlow/RMBaseNode';
import { Edit, FileSliders, Info, Rocket } from 'lucide-react';
import { useEffect, useState } from 'react';
import { Button } from '@/components/ui/button';
import { DataSourceManager_OpenWithExplorer, DataSourceManager_REDIS_GetConfigFile, DataSourceManager_REDIS_IsRun, DataSourceManager_REDIS_RUN, DataSourceManager_REDIS_WriteConfigFile } from '@/event/DataSourceManager';
import { toast } from 'sonner';
import { Label } from '@/components/ui/label';

export default function () {
    const dispatch = useDispatch();
    const current = useSelector((state: RootState) => state.current)
    const services = useSelector((state: RootState) => state.services)



    useEffect(() => {
        DataSourceManager_REDIS_IsRun(bol => {
            dispatch(updateCurrentState({ ...current, isredisRun: bol }))
        })
        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, [current.isredisRun, dispatch])

    const [edges, , onEdgesChange] = useEdgesState([
        {
            id: crypto.randomUUID(),
            source: "redis_0",
            target: "redis_out_log"
        },
        {
            id: crypto.randomUUID(),
            source: "redis_0",
            target: "redis_config"
        },
        {
            id: crypto.randomUUID(),
            source: "redis_config",
            target: "redis_contoller"
        },
        {
            id: crypto.randomUUID(),
            source: "redis_contoller",
            target: "redis_contoller_run"
        },
    ]);

    const [nodes, , onNodesChange] = useNodesState([
        {
            type: "RMBaseNode",
            id: "redis_0",
            position: { x: 60, y: 0 },
            data: {
                header: <>
                    <img src={redis} className="w-6" />
                    <BaseNodeHeaderTitle>Mysql</BaseNodeHeaderTitle>
                </>,
                ContentComponent() {
                    const current = useSelector((state: RootState) => state.current)
                    return <div className='w-30 text-center'>
                        <span>服务状态</span>
                        <div className={`p-2 rounded-sm inline-block mt-1 ${current.isredisRun ? "bg-chart-3" : "bg-chart-1"}`}>
                            <Label>{current.isredisRun ? "运行中" : "未启动"}</Label>
                        </div>
                    </div>
                },
                footer: <LabeledHandle
                    id="mysql_0_out"
                    type="source"
                    title="配置"
                    position={Position.Bottom}
                />
            },
        },
        {
            type: "RMBaseNode",
            id: "redis_out_log",
            position: { x: -180, y: 200 },
            data: {
                targetData: {
                    log: localStorage.getItem("redis_out_log") || ""
                },
                HeaderComponent({ targetData }: BaseNodePropChildrenDataType) {
                    const [data, setData] = targetData;
                    return <>
                        <BaseHandle type="target" position={Position.Top} />
                        <Info className="size-5" />
                        <BaseNodeHeaderTitle>
                            日志
                        </BaseNodeHeaderTitle>
                        <Button className="nodrag nopan nowheel cursor-pointer" variant="destructive" onClick={() => {
                            setData({ ...data, log: "" })
                            localStorage.setItem("redis_out_log", "")
                        }}>清空日志</Button>
                    </>
                },
                ContentComponent({ targetData }: BaseNodePropChildrenDataType) {
                    const [data, setData] = targetData;

                    useEffect(() => {
                        const onMessage = ({ data: d }: WebView2Event) => {
                            if (d && d.data.type == "RedisLog") {
                                const msg = d.data.msg;
                                const log = data.log += `${msg}\n`;
                                setData({ ...data, log: log });
                                localStorage.setItem("redis_out_log", log);
                            }
                        }

                        window.chrome.webview.addEventListener("message", onMessage);
                        return function () {
                            window.chrome.webview.removeEventListener("message", onMessage);
                        }
                    }, [data, setData])
                    return <Textarea
                        className='nodrag nopan nowheel w-90 min-h-60 max-h-60'
                        placeholder="服务日志..."
                        value={data.log}
                        onChange={() => { }}
                    />
                }
            }
        },
        {
            id: "redis_config",
            position: { x: 220, y: 0 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Left} />
                    <BaseHandle type="source" position={Position.Bottom} />
                    <Edit className="size-5" />
                    <BaseNodeHeaderTitle>
                        redis.conf
                    </BaseNodeHeaderTitle>
                    <Button onClick={() => {
                        DataSourceManager_OpenWithExplorer(services.redis.outDir + "\\redis.conf", (state) => {
                            console.log(state);
                        })
                    }}>外部程序编辑</Button>
                </>,
                ContentComponent() {
                    const dispatch = useDispatch();
                    const current = useSelector((state: RootState) => state.current)

                    useEffect(() => {
                        DataSourceManager_REDIS_GetConfigFile(({ data }) => {
                            dispatch(updateCurrentState({ ...current, redisConfigLabel: data }))
                        })

                        // eslint-disable-next-line react-hooks/exhaustive-deps
                    }, [current.redisConfigLabel, dispatch])


                    return <Textarea
                        className='nodrag nopan nowheel w-110 h-70'
                        placeholder="redis.conf配置"
                        value={current.redisConfigLabel}
                        onChange={({ target }) => {
                            const newValue = target.value;
                            dispatch(updateCurrentState({ ...current, redisConfigLabel: newValue }));
                        }}
                    />
                }
            },
            type: "RMBaseNode",
        },
        {
            type: "RMBaseNode",
            id: "redis_contoller",
            position: { x: 250, y: 400 },
            data: {
                header: <>
                    <BaseHandle type="target" position={Position.Left} />
                    <BaseHandle type="source" position={Position.Right} />
                    <FileSliders className="size-5" />
                    <BaseNodeHeaderTitle>
                        配置
                    </BaseNodeHeaderTitle>
                </>,
                ContentComponent() {
                    const current = useSelector((state: RootState) => state.current);
                    useEffect(() => { }, [current])
                    return <div className='text-center'>
                        <Button onClick={() => {
                            DataSourceManager_REDIS_WriteConfigFile(current.redisConfigLabel, bol => {
                                toast(`写入${bol ? '成功!' : '错误！'}`)
                            })
                        }}>修改配置</Button>
                    </div>
                },
            },
        },
        {
            type: "RMBaseNode",
            id: "redis_contoller_run",
            position: { x: 390, y: 400 },
            data: {
                HeaderComponent() {
                    return <>
                        <BaseHandle type="target" position={Position.Left} />
                        <Rocket className="size-5" />
                        <BaseNodeHeaderTitle>
                            运行
                        </BaseNodeHeaderTitle>
                    </>
                },
                ContentComponent() {
                    const current = useSelector((state: RootState) => state.current)
                    useEffect(() => { }, [current.isredisRun])

                    const [disabled, setDisabled] = useState(false);
                    return <div className='text-center'>
                        <Button
                            disabled={disabled}
                            variant={`${current.isredisRun ? "destructive" : "default"}`}
                            onClick={() => {
                                setDisabled(true)
                                DataSourceManager_REDIS_RUN(current.isredisRun ? "STOP" : "RUN", () => {
                                    setTimeout(() => {
                                        setDisabled(false)
                                        DataSourceManager_REDIS_IsRun(bol => {
                                            dispatch(updateCurrentState({ ...current, isredisRun: bol }))
                                        });
                                    }, 1000);
                                })
                            }}
                        >{current.isredisRun ? "停止" : "启动"}服务</Button>
                    </div>
                },
            },
        }
    ]);
    return <XyFlow
        nodes={nodes}
        edges={edges}
        onEdgesChange={onEdgesChange}
        onNodesChange={onNodesChange}
    />
}