| English | [简体中文](./README-zh.md) |
| ------- | -------------------------- |

# YADAW

YADAW stands for **Y**et **A**nother **D**igital **A**udio **W**orkstation. This is intended to be an improved version of my [first attempt](https://github.com/xris1658/musec-legacy) developing a DAW software.

Major changes:
- [x] The name
- [x] Use Qt 6.5 instead of Qt 5.15.2
- [x] Use other audio backends; remove ASIO support ([why?](https://github.com/xris1658/musec-legacy/issues/7))
- [x] Introduce Vestifal: a combination of several reverse-engineered works of VST plugin API
- [ ] Beyond stereo: add multiple speaker arrangement support
- [ ] Advanced mixer structure: Insert groups, send support, sidechain support, PDC supoort, etc.
- [ ] Stock plugins (maybe)
- ...