| English | [简体中文](./README-zh.md) |
| ------- | -------------------------- |

# YADAW

YADAW stands for **Y**et **A**nother **D**igital **A**udio **W**orkstation. This is intended to be an improved version of my [first attempt](https://github.com/xris1658/musec-legacy) developing a DAW software.

Major changes include:
- The name
- Use Qt 6 instead of Qt 5.15.2 (which will reach EoL in a few months)
  - Qt 6.5 will be used once the [final release](https://wiki.qt.io/Qt_6.5_Release) arrives on March 30th
- Use other audio backends; remove ASIO support ([why?](https://github.com/xris1658/musec-legacy/issues/7))
- ...