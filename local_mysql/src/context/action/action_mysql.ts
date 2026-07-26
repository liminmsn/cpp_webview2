type MysqlAction = { type: "SET_STATE", payload: Partial<MysqlState> } | { type: "RESET" }
interface MysqlState {
    installed: boolean
    running: boolean
    status: | "unknown" | "stopped" | "running" | "installing" | "starting" | "error"
    mysql_path: string
}

const initialState: MysqlState = {
    installed: false,
    running: false,
    status: "unknown",
    mysql_path: "--"
}


function reducer(state: MysqlState, action: MysqlAction): MysqlState {
    switch (action.type) {
        case "SET_STATE":
            return { ...state, ...action.payload }
        case "RESET":
            return initialState
        default:
            return state
    }
}


export type { MysqlState, MysqlAction }
export { reducer, initialState }
