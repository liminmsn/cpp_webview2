// store.ts
import { configureStore } from '@reduxjs/toolkit'
import counterReducer from './features/counterSlice'
import serviceReducer from './features/serviceSlice'

export type RootState = ReturnType<typeof store.getState>
export type AppDispatch = typeof store.dispatch

export const store = configureStore({
  reducer: {
    counter: counterReducer,
    services: serviceReducer
  }
})