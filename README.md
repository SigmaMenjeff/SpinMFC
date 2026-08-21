# SpinMFC — 工业相机图像采集上位机

基于 **MFC + Spinnaker SDK** 开发的工业相机（Teledyne FLIR / Point Grey）桌面图像采集上位机，支持**实时图像显示、图像保存、视频录制**以及**曝光、增益等核心参数控制**。

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![Platform: Windows](https://img.shields.io/badge/Platform-Windows-brightgreen.svg)
![Build: VS2022](https://img.shields.io/badge/Build-VS2022-blue.svg)

---

## 功能特性

- **相机连接与信息显示**：自动检测并连接相机，枚举显示设备序列号、型号等设备信息
- **实时图像预览**：工作线程持续采集图像，通过自定义消息异步刷新到界面，不阻塞主线程
- **参数实时调节**：曝光时间、增益支持滑动条拖动 + 数值输入双通道控制，自动关闭自动曝光/自动增益
- **像素格式切换**：支持 `Mono8` / `RGB8` 格式选择
- **图像尺寸切换**：内置 7 种分辨率档位（2448x2048 / 2048x2048 / 1920x1080 / 1120x1120 / 1024x1224 / 1024x1024 / 1280x720），自动计算居中偏移
- **图像保存**：支持 **JPG / BMP / PNG** 三种格式，文件名自动附加时间戳
- **视频录制**：基于 Spinnaker `SpinVideo`，支持 **未压缩 AVI / MJPG AVI / H.264 AVI / H.264 MP4** 四种封装
- **图像缓冲优化**：采用 `NewestOnly` 缓冲模式，保证低延迟实时显示

---

## 截图

> 截图待补充。图片放置说明：
> - 将截图放入 `docs/screenshots/` 目录
> - 按下列文件名命名，README 会自动显示，无需修改代码
> - 图片格式支持 png / jpg / gif

### 主界面

![主界面](docs/screenshots/main.png)

<!-- 更多截图示例（后续可按需取消注释使用）：
![采集与保存界面](docs/screenshots/capture.png)
![参数控制界面](docs/screenshots/parameters.png)
-->

---

## 技术栈

| 类别 | 技术 |
|---|---|
| 开发环境 | Visual Studio 2022 |
| 界面框架 | MFC（动态链接，Unicode） |
| 语言标准 | C++ / C++17 |
| 相机 SDK | Spinnaker SDK 4.2.0.83 |
| 采集接口 | GigE / USB3 Vision 工业相机 |

---

## 环境依赖

### 硬件

- Teledyne FLIR（原 Point Grey）系列工业相机，通过 GigE 或 USB3 接口连接

### 软件

| 依赖 | 说明 |
|---|---|
| Windows 10/11 x64 | 仅支持 x64 构建 |
| Visual Studio 2022 | 需安装 **"适用于最新 v143 生成工具的 C++ MFC (x86 和 x64)"** 组件 |
| Spinnaker SDK 4.2 | 免费下载，用于相机驱动与图像采集 |

> **Spinnaker SDK 下载**（需注册/登录后下载）：
>
> - 中文站：https://www.teledynevisionsolutions.com/zh-cn/products/spinnaker-sdk/?model=Spinnaker%20SDK&vertical=machine%20vision&segment=iis
> - English：https://www.teledynevisionsolutions.com/products/spinnaker-sdk/?model=Spinnaker%20SDK&vertical=machine%20vision&segment=iis

---

## 构建与运行

### 方式一：使用属性表（推荐，只需改一处）

1. 克隆 / 下载本项目到本地
2. 安装 **Spinnaker SDK**，记住安装根目录路径（例如 `E:\SpinnakerSDK\Spinnaker4.2.0.83`）
3. 用 VS2022 打开 `SpinMFC.sln`
4. 用文本编辑器打开项目根目录下的 **`Spinnaker.props`**，将文件中部（约第 20 行）的
   `<SpinnakerSDKRoot>` 值改为你本机的 SDK 根目录：

   ```xml
   <SpinnakerSDKRoot>E:\SpinnakerSDK\Spinnaker4.2.0.83</SpinnakerSDKRoot>
   ```

5. 选择 **Debug + x64** 配置，点击生成（F7）即可

### 方式二：在 VS 属性页手动配置

若不使用属性表，也可在 **项目 → 属性** 中手动配置包含目录、库目录和附加依赖项。
完整的手动配置步骤见：[docs/移植指南.md](docs/移植指南.md)

---

## 使用说明

1. **连接相机**：点击"连接相机"，程序自动枚举并连接第一台相机，同时在右侧显示设备信息
2. **开始采集**：点击"开启相机"，实时图像开始在左侧窗口显示
3. **调节参数**：拖动曝光/增益滑动条或在输入框输入数值，图像即时响应
4. **保存图片**：点击"选择路径"设置保存目录 → 点击"保存图片"，按所选格式（JPG/BMP/PNG）保存当前帧
5. **录制视频**：点击"选择路径"设置保存目录 → 选择视频格式 → 点击"录制视频"开始 / 停止录制
6. **退出**：关闭窗口时自动释放相机资源

---

## 目录结构

```
SpinMFC/
├── SpinMFC.sln              # 解决方案文件
├── Spinnaker.props          # Spinnaker SDK 路径属性表（★ 关键配置）
├── README.md                # 项目说明
├── LICENSE                  # MIT 许可证
├── .gitignore               # 忽略编译输出与缓存
├── docs/
│   └── 移植指南.md           # 完整的手动移植 / 配置指南
└── SpinMFC/
    ├── SpinMFC.cpp/.h       # 应用入口
    ├── SpinMFCDlg.cpp/.h    # 主对话框（UI 逻辑、采集线程）
    ├── SpinCamera.cpp/.h    # 相机控制与图像处理封装
    ├── framework.h / pch.h  # 预编译头
    ├── resource.h / .rc     # 资源文件
    └── res/                 # 图标等资源
```

---

## 主要代码模块

| 文件 | 职责 |
|---|---|
| `SpinMFCDlg.cpp` | 界面交互、图像采集工作线程、图像消息派发 |
| `SpinCamera.cpp` | Spinnaker SDK 封装：连接/断开、曝光增益、图像尺寸、保存图片、录制视频 |
| `SpinMFC.cpp` | 应用程序类与入口 |

---

## 作者

- **SigmaMenjeff** — [GitHub](https://github.com/SigmaMenjeff)

---

## 许可证

本项目基于 [MIT License](LICENSE) 开源。

> 注意：本项目依赖的 **Spinnaker SDK** 为 Teledyne FLIR 提供的免费商业 SDK，使用时请遵守其自身的许可协议。
