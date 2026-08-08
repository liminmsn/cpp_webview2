import { createSlice, type PayloadAction } from '@reduxjs/toolkit'

type CurrentStateType = {
  ismysqlRun: boolean;
  redisConfigLabel: string;
  mysql: any
  redis: any
}

const initialState: CurrentStateType = {
  ismysqlRun: false,
  mysql: {},
  redis: {},
  redisConfigLabel: ""
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
