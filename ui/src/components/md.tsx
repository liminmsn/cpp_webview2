import ReactMarkdown from 'react-markdown';

export default function () {
    const markdown = `
无需复杂配置，即可快速创建独立的本地数据库运行环境。支持数据库初始化、服务安装、启动停止、状态监控、配置管理等功能，让开发者能够更加高效地管理本地开发数据库。

采用现代化桌面 UI 设计，结合原生 C++ 高性能管理能力，为开发者提供轻量、稳定、安全的本地数据库解决方案。


## 主要功能

- 🚀 一键初始化 MySQL 数据环境
- 🗄️ MySQL 服务安装、启动、停止、重启管理
- ⚡ Redis 本地实例快速部署与运行
- 📊 实时监控数据库运行状态
- ⚙️ 可视化管理数据库配置文件
- 🔧 自动化处理数据库目录、端口、服务配置
- 🖥️ 原生 Windows 桌面体验
- 🔒 数据完全存储于本机，保护开发数据安全


## 适用场景

- 后端开发者
- 全栈开发者
- 软件工程师
- 本地测试环境搭建
- 项目开发数据库管理


## 项目特点

LocalData Manager 致力于为开发者提供更加简单、高效、可靠的本地数据库管理体验。

通过自动化流程替代繁琐的手动配置，让开发者能够快速搭建 MySQL 与 Redis 开发环境，将更多精力投入到项目开发本身。
  `

    return <div>
        <ReactMarkdown>{markdown}</ReactMarkdown>
        <p className='text-primary mt-5 mb-3'>请开发者喝冰可乐</p>
        <img className='inline-block w-50' src="/wx_0.png" alt="支持我" />
        <p className='text-primary my-3'>技术支持</p>
        <img className='inline-block w-50' src="/wx_1.png" alt="微信技术支持" />
    </div>;
}