| [English](./README.md) | 简体中文 |

# YADAW

YADAW 意为 **Y**et **A**nother **D**igital **A**udio **W**orkstation. 此项目是本人[初次尝试](github.com/xris1658/musec-legacy)开发的 DAW 软件的改良版本。

主要改动包括：
- 项目名
- 不再使用 Qt 5.15.2（数月后生命周期结束），改用 Qt 6
  - 当 Qt 6.5 [最终版](https://wiki.qt.io/Qt_6.5_Release) 3 月 30 日发行后使用此版本
- 换用其他的音频后端；移除 ASIO 支持（[为社么？](https://github.com/xris1658/musec-legacy/issues/7)）
- ...