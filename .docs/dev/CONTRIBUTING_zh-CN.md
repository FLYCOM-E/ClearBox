# 为 ClearBox 做贡献

感谢您愿意提供帮助！ClearBox 是一个主力单人维护的项目（至少目前 :-) ），因此任何帮助 —— 可能是一行配置文件 —— 开发者都会感谢您。
请您根据所长选择下方的项目参与。**前两项无需编写代码。**



## 贡献方式

| 方向 | 工作内容 | 最终去向 |
|---|---|---|---|
| 配置 | 编写并分享您的清理/整理/自动化规则 | 内置配置或社区规则集 |
| 翻译 | 改进多语言翻译字符串 | `.docs/README_*.md` 或应用/TUI 内文本 |
| 测试 | 在您的设备/ROM 上运行构建，报告 Bug | Issues 或 Telegram 群组 |
| 代码 | 参与 C 核心、Kr-Script 应用、构建脚本开发 | Pull Requests & Push |



### 配置（配置编写与调试）

以下全部是纯文本——只要您会编辑 `.txt`，就能贡献：

预设配置：
- `AppSource/clearbox/src/main/assets/usr/Timed_xml/` — 自动化任务
- `FileConfigs/` — 文件清理 / 归类规则
- `AppConfigs/` — 应用私有储存规则

自定义配置：
- `/data/adb/wipe_cache/CleanConfigs` — 杂乱目录清理规则
- `/data/adb/wipe_cache/BindConfigs` — 存储重定向规则

语法参考：请查看 README 中各部分的说明。需遵守的规则：

- 一个 PR（或一个文件）对应一个规则文件，以所清理的内容命名
- 部分规则使用相对路径，**请不要** 编码以根 `/xxx` 开始的路径
- `#` 注释尽量独占一行（行尾的 `#ext` 内联注释是唯一的例外）
- 请在您自己的设备上测试规则，PR 并附测试设备 / ROM / Android 版本。
- 预设配置请提交至以上预设目录并 **发起 PR**
- 自定义配置推荐您分享至社区或在 Telegram 群组中



### 翻译（完善字符串）

发现拼写错误、表述生硬或未翻译的字符串？欢迎 PR。
请尽量每个 PR **只修改一种语言**，以便快速审阅。
项目目前包含以下语言，您可以选择您擅长的：
- **`en-US / zh-CN / zh-TW / ru-RU / ko-KR / ja-JP`**
（怎么，您发现没有 :-)？那只好烦劳您一下了）



### 测试（功能改善与报告）

报告问题时，请包含：

- 设备与 Android 版本：
- ROM（MIUI / HyperOS / AOSP...）
- Root 方案：Magisk / APatch / KernelSU + 版本号
- ClearBox 版本（Version Code）
- 文件系统（如果知道）：f2fs / ext4 / sdcardfs
- 实际发生了什么 vs 预期行为
- 终端输出或日志（如有）



### 代码（功能添加 / Bug 修复）

- 请配置 **NDK** 并设置环境变量 **ANDROID_NDK_HOME** 为 **NDK** 根目录路径
- 如果深度参与 **App** 代码开发与调试请配置 **Android SDK** 环境
- 以下是一些基础环境配置: 

基础包工具

``` shell
# Debian、Ubuntu 系
sudo apt install zip make build-essential

# Arch 系
sudo pacman -S zip make base-devel

# Alpine 系
sudo apk add zip make build-base

# Fedora 系
sudo dnf install zip make
sudo dnf groupinstall "Development Tools"
```

拉取项目（仅供参考，如果 PR 请自行 Fork）

``` shell
git clone https://github.com/FLYCOM-E/ClearBox
cd ClearBox/ && chmod +x *

export TARGET_API=23         # 可以自行根据需要调整目标api版本
export TARGET_ABI=aarch64   # 目前可选: aarch64、armv7a、x86_64、riscv64
```

编译核心

``` shell
./build.sh       # 如需编译 APK: --build-apk
```

提交规范：保持现有风格: `Fix: ...` / `Update: ...` / `Add: ...`，尽量每个提交只做一项相关更改，如果有多项更改请添加换行。
PR 要求：一个 PR = 一个功能或一个修复。请描述您更改了什么以及 **为什么**（如有必要。您遇到的实际失败案例是最好的“为什么”）。



## 🚫 请不要

- 请不要在没有上述设备信息的情况下提交 “它不工作” 的 issue，没有这些信息维护者可能根本无法复现
- 功能创意：在编写大量代码之前，请先创建一个 Issue 进行讨论，以免浪费您的时间
- 不要将格式调整与逻辑更改混在同一个提交中
- 不要提交未经测试的正则猜测式规则



## ✍️ 讨论渠道

- Issues：用于 **Bug** 和功能追踪
- Telegram（最活跃）：[**Group**](https://t.me/clearbox_group)



## ❤️ 致谢

本项目的许可证为 [GPL-3.0](https://github.com/FLYCOM-E/ClearBox/blob/main/LICENSE) ~ 一旦您贡献，即表示您同意您的成果以相同许可证分发。
贡献者将在此列出。您的名字可能成为这个列表中的靠前一个。



### 感谢以下贡献者：

[**Amktiao**](https://www.coolapk.com/u/925348): 间接提供 F2FS-GC 原理
[**Drsexo**](https://github.com/Drsexo): 添加 Github Action 配置
[**Creeper005bili**](https://github.com/Creeper005bili): 在模块早期帮助测试
