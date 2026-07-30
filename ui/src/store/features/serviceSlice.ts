import { createSlice, type PayloadAction } from '@reduxjs/toolkit'
export interface ServiceStateType {
  mysql: ServiceStateItemType;
  redis: ServiceStateItemType;
}
interface ServiceStateItemType {
  InitialState: boolean;
}


const initialState: ServiceStateType = {
  mysql: { InitialState: false },
  redis: { InitialState: false }
}


const serviceSlice = createSlice({
  name: 'services',
  initialState,
  reducers: {
    updateState: (state, action: PayloadAction<ServiceStateType>) => {
      return { ...state, ...action.payload }
    }
  }
})

export const { updateState } = serviceSlice.actions
export default serviceSlice.reducer
