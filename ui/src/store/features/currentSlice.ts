import { createSlice, type PayloadAction } from '@reduxjs/toolkit'

type CurrentStateType = {
  ismysqlRun: boolean;
  mysql: { log: string }
  redis: { log: string }
}

const initialState: CurrentStateType = {
  ismysqlRun: false,
  mysql: { log: "" },
  redis: { log: "" }
}

const currentSlice = createSlice({
  name: 'current',
  initialState: initialState,
  reducers: {
    updateCurrentState: (state, action: PayloadAction<CurrentStateType>) => {
      return { ...state, ...action.payload }
    }
  }
})

export const { updateCurrentState } = currentSlice.actions
export default currentSlice.reducer
