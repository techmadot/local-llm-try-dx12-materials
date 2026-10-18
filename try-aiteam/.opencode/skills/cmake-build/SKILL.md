---
name: cmake-build
description: Build the project using CMake.
usage: cmake --build <build-directory>
---

# CMake Build Skill

CMakeを使用してプロジェクトをビルドするためのスキルです。
プロジェクトのルートディレクトリをカレントディレクトリにして、CMakeを実行します。
`cl.exe` や `msbuild.exe` は直接呼び出しません。

## 前提

- CMake が利用可能であること
- Visual Studio 2022 の CMake ジェネレーターを使用すること
- シェーダーのオフラインコンパイルに DXC を使用すること
- DXC の既定のパスは `C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\dxc.exe` とすること
- DXC のパスは環境変数 `DXC_PATH` で変更できること

`DXC_PATH` が設定されていない場合は、既定のパスにある `dxc.exe` を使用します。
DXC が見つからない場合は、ビルドを開始せずエラーとして扱います。

## 手順

1. **プロジェクトのルートディレクトリ** をカレントディレクトリにする
2. DXC のパスを確認する。必要な場合は `DXC_PATH` を設定する
3. ビルド用ディレクトリを作成し、CMake を呼び出して Visual Studio のビルドファイル群を生成する
4. `Debug` または `Release` の構成を指定して、CMake 経由でビルドを実行する
5. ビルド時に HLSL を DXC でオフラインコンパイルし、`bin/shaders/*.cso` を生成する

上記の手順を実行することで、プロジェクトをビルドすることができます。

### Visual Studio 用プロジェクトの生成
```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
```

DXC の場所を変更する場合は、CMake の構成前に環境変数を設定します。

```powershell
$env:DXC_PATH = "C:\path\to\dxc.exe"
cmake -S . -B build -G "Visual Studio 17 2022"
```

### ビルドの実行

```powershell
cmake --build build --config Debug
```

Release ビルドを実行する場合は、構成だけを変更します。

```powershell
cmake --build build --config Release
```

