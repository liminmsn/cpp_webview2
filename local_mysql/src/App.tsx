import { Tabs } from '@heroui/react'
import { Database, FileCog, TableProperties } from 'lucide-react'
import LMysql from './page/LMysql'

const tabs = [
  {
    label: '配置管理',
    icon: FileCog,
    page: LMysql
  },
  {
    label: '数据库',
    icon: Database,
    page: () => <div>数据库</div>
  },
  {
    label: '数据表',
    icon: TableProperties,
    page: () => <div></div>
  }
]

export default function () {
  return <div className='h-full p-2'>
    <Tabs className="h-full" orientation="vertical">
      <Tabs.ListContainer className='shadow-sm'>
        <Tabs.List aria-label="垂直标签页" className='min-w-36'>
          {
            tabs.map((item, idx) => {
              return <Tabs.Tab id={item.label} key={idx} className='justify-start pl-3 py-5 data-selected:text-white'>
                <Tabs.Indicator className='bg-accent' />
                <item.icon size={18} className='mr-1' />
                {item.label}
              </Tabs.Tab>
            })
          }
        </Tabs.List>
      </Tabs.ListContainer>
      {
        tabs.map((item, idx) => {
          return <Tabs.Panel id={item.label} key={idx} className='p-0'>
            <item.page />
          </Tabs.Panel>
        })
      }
    </Tabs>
  </div>
}
