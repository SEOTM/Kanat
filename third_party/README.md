# 第三方依赖

## Kanat.Deps.Native 1.0.0

`Kanat.Deps.Native.1.0.0.nupkg` 是构建期依赖归档。CMake 在配置时把它解包到
`${CMAKE_BINARY_DIR}/third_party/kanat-deps`，然后使用其中的头文件与静态库。构建过程不调用 NuGet，
也不访问网络。

归档内容：

| 组件 | 版本 | 形式 | 用途 |
| :--- | :--- | :--- | :--- |
| Dear ImGui | 1.92.8 | 预编译静态库（x64-windows-static，`/MT`）+ 头文件 | 控件绘制、Win32 / DX11 后端 |
| RapidJSON | 1.1.0 | 头文件 | 布局文档 JSON 解析 |

包含目录：

```text
build/native/include
```

静态库：

```text
build/native/lib/debug/imguid.lib
build/native/lib/release/imgui.lib
```

归档内另有 `THIRD-PARTY-NOTICES.txt`，随包携带同一份许可声明。

## 许可

Dear ImGui 与 RapidJSON 均以 MIT License 分发。

```text
MIT License

Copyright (c) 2014-2025 Omar Cornut
Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 更新依赖

1. 准备新的归档，保持 `build/native/include` 与 `build/native/lib/{debug,release}` 结构；
2. 替换 `third_party/` 下的归档并同步修改 `CMakeLists.txt` 中的文件名；
3. 删除 `out/` 后重新配置与构建；
4. 更新本文件的版本表与许可声明。
