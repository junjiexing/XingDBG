# XingDBG  
[![GitHub license](https://img.shields.io/github/license/junjiexing/XingDBG)](https://github.com/junjiexing/XingDBG/blob/master/LICENSE) [![GitHub Actions Build Status](https://github.com/junjiexing/XingDBG/actions/workflows/macOS.x64.yml/badge.svg?branch=master&event=push)](https://github.com/junjiexing/XingDBG/actions/workflows/macOS.x64.yml?query=event%3Apush+branch%3Amaster) [![GitHub Actions Build Status](https://github.com/junjiexing/XingDBG/actions/workflows/Windows.x64.yml/badge.svg?branch=master&event=push)](https://github.com/junjiexing/XingDBG/actions/workflows/Windows.x64.yml?query=event%3Apush+branch%3Amaster) [![GitHub Actions Build Status](https://github.com/junjiexing/XingDBG/actions/workflows/Windows.arm64.yml/badge.svg?branch=master&event=push)](https://github.com/junjiexing/XingDBG/actions/workflows/Windows.arm64.yml?query=event%3Apush+branch%3Amaster) [![Total Downloads](https://img.shields.io/github/downloads/junjiexing/XingDBG/total)](https://github.com/junjiexing/XingDBG/releases)  

![Logo](https://raw.githubusercontent.com/junjiexing/images/main/XingDBG.svg)  
感谢 [LinGo](https://github.com/saixmh) 帮忙设计的图标

基于 LLDB 和 Qt 开发的调试器  

- 已完成大部分基础调试功能
- 调试器本身支持大多数 LLDB 和 Qt 支持的平台，开发及测试是 Windows 和 macOS 上，Linux 理论上支持，但是没时间编译和测试
- 支持远程调试 Windows、macOS、Linux、Android 和 iOS

下载可以直接到 [Actions](https://github.com/junjiexing/XingDBG/actions) 页面下载对应系统编译好的 artifact。目前会自动编译 Windows x64、Windows arm64 和 macOS x64 版本的可执行程序，因为 Github Actions 目前还不支持 Apple Silicon 的系统，所以CI上目前没有 macOS arm64 的 binary，等支持后会补上。  

## 屏幕截图  
![Screenshot 1](https://raw.githubusercontent.com/junjiexing/images/main/XingDBG-mac-arm64-1.png)  
![Screenshot 2](https://raw.githubusercontent.com/junjiexing/images/main/XingDBG-mac-arm64-2.png)  

