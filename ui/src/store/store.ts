// store.ts
import { configureStore } from '@reduxjs/toolkit'
import currentReducer from './features/currentSlice'
import serviceReducer from './features/serviceSlice'

export type RootState = ReturnType<typeof store.getState>
export type AppDispatch = typeof store.dispatch

export const store = configureStore({
  reducer: {
    current: currentReducer,
    services: serviceReducer
  }
})