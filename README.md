# awtk-firmware-flasher(not workable yet)

## 1.介绍

* [AWTK](https://github.com/zlgopen/awtk)  全称 Toolkit AnyWhere，是 ZLG 开发的开源 GUI 引擎，旨在为嵌入式系统、WEB、各种小程序、手机和 PC 打造的通用 GUI 引擎，为用户提供一个功能强大、高效可靠、简单易用、可轻松做出炫酷效果的 GUI 引擎。

* [AWTK-firmware-flasher](https://github.com/zlgopen/awtk-firmware-flasher) 固件下载工具。

## 2.准备

1. 获取 awtk 并编译

```
git clone https://github.com/zlgopen/awtk.git
cd awtk; scons; cd -
```

2. 获取 awtk-firmware-flasher 并编译
```
git clone https://github.com/zlgopen/awtk-firmware-flasher.git
cd awtk-firmware-flasher; scons
```

## 3.运行

```
./bin/awtk_flasher
```

> 本文以 Linux/MacOS 为例，Windows 可能会微妙差异，请酌情处理。
