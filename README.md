# MapRanger

MapRanger 是一个运行在 Windows 上的轻量级，针对 PUBG 的小地图测距工具。程序会创建一个透明覆盖层，并根据配置抓取指定区域的小地图，识别 4 类标记并显示距离信息。
> 本程序不会修改游戏文件、注入游戏进程或读写游戏内存。根据当前实现方式，理论上不会触发反作弊检测，但无法保证未来反作弊策略调整后的情况。

## 功能

- 透明窗口叠加层，覆盖在桌面或游戏画面之上
- 全局热键切换小地图叠加层显示状态
- 按屏幕分辨率自动加载预设的小地图区域和比例参数
- 基于颜色匹配检测 4 类标记，并换算为游戏中米数显示
- 支持不同色盲模式的标记颜色配置
- 可选调试输出，便于排查检测结果

## 环境要求

- 运行时需要保证运行目录存在正确的 `config.ini`

## 构建方式

项目提供了 `build.bat`，默认使用 `cl` 编译器进行构建。

```bat
build.bat
```

构建成功后，生成文件位于：

```text
build\MapRanger.exe
```

## 配置说明

程序启动时会读取根目录下的 `config.ini`。

### `config.ini`

```ini
[CONFIG]
minimapOverlayHotkey=Shift+F1
debug=false
colorBlind=none
tolerance=10
```

### 配置项

- `minimapOverlayHotkey`
  - 全局热键，用于切换小地图叠加层的显示/隐藏
  - 目前支持常见组合键，例如 `Shift+F1`、`Ctrl+Alt+M`
- `debug`
  - `true` / `false`
  - 启用后会输出更多运行日志和性能信息
- `colorBlind`
  - 支持：`none`、`deuteranopia`、`protanopia`、`tritanopia`
  - 用于切换不同的颜色检测预设
- `tolerance`
  - 颜色检测容差，数值越大越宽松

## 分辨率支持

当前内置了以下分辨率配置：

- `1920x1080`
- `2560x1440`

如果检测到其他分辨率，程序会回退到 `1920x1080` 的小地图区域和比例配置。

## 使用方法

1. 按照你的显示器分辨率和偏好修改 `config.ini`
2. 运行生成的 `MapRanger.exe`
3. 使用配置好的全局热键切换小地图叠加层
4. 当小地图可见时，程序会自动检测标记并刷新距离显示

## 项目结构

```text
main.cpp                程序入口
runtime/                运行时配置、分辨率和颜色预设
utils/                  输入、截图、数学、性能计时等工具
graphics/               Win32 + DirectX11 + ImGui 画面上下文
ui/                     小地图叠加层绘制
includes/               第三方依赖（ImGui, mINI）
config.ini              运行配置
build.bat               Windows 批量构建脚本
```

## 其他

- 采用 Win32 + DirectX11 + Dear ImGui 构建覆盖层
- 使用 GDI 进行屏幕区域抓取
- 通过颜色阈值和几何换算计算距离
- 以增量刷新方式渲染，避免不必要的重绘

## 许可证

本项目采用 MIT License。详情见 [LICENSE](LICENSE)。

