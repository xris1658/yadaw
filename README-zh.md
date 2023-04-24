| [English](./README.md) | 简体中文 |
| ---------------------- | -------- |

# YADAW

YADAW 意为 **Y**et **A**nother **D**igital **A**udio **W**orkstation. 此项目是本人[初次尝试](https://github.com/xris1658/musec-legacy)开发的 DAW 软件的改良版本。

主要改动包括：
- 项目名
- 不再使用 Qt 5.15.2，改用 Qt 6.5
- 换用其他的音频后端；移除 ASIO 支持（[为什么？](https://github.com/xris1658/musec-legacy/issues/7)）
- 引入 Vestifal：数个逆向工程的 VST 插件 API 的结合
- ...