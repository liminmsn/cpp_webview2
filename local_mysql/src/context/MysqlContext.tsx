import { createContext, useReducer, type ReactNode, useContext, useEffect } from "react"
import { reducer, type MysqlState, initialState } from "./action/action_mysql"
import GlobalWebViewEvent from "../event/GlobalWebViewEbent"

interface MysqlContextType {
    state: MysqlState
    setState: (value: Partial<MysqlState>) => void
    reset: () => void
}


const MysqlContext = createContext<MysqlContextType | null>(null)

export function MysqlProvider({ children }: { children: ReactNode }) {
    const [state, dispatch] = useReducer(reducer, initialState)

    function setState(value: Partial<MysqlState>) {
        dispatch({ type: "SET_STATE", payload: value })
    }

    function reset() {
        dispatch({ type: "RESET" })
    }

    useEffect(() => {
        GlobalWebViewEvent.start();
    }, [])

    return <MysqlContext.Provider value={{ state, setState, reset }}>
        {children}
    </MysqlContext.Provider>
}



export function useMysql() {
    const context = useContext(MysqlContext)
    if (!context) {
        throw new Error("useMysql 必须在 MysqlProvider 内使用")
    }

    return context
}