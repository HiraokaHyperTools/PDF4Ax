# Dev

## CMake

### x86

```
cmake -G "Visual Studio 16 2019" -A Win32
```

- [Visual Studio 16 2019 — CMake 3.14.7 Documentation](https://cmake.org/cmake/help/v3.14/generator/Visual%20Studio%2016%202019.html)

### x64

- [visual studio - How to detect if 64 bit MSVC with cmake? - Stack Overflow](https://stackoverflow.com/questions/39258250/how-to-detect-if-64-bit-msvc-with-cmake)

### BUILD_SHARED_LIBS=false

```
# For a dynamic library, use
#
#   cmake -B build -D BUILD_SHARED_LIBS=true -D CMAKE_BUILD_TYPE=Release
```

## pkg-config

- [How to install pkg config in windows? - Stack Overflow](https://stackoverflow.com/questions/1710922/how-to-install-pkg-config-in-windows/1711338#1711338)

## zlib

```bat
cmake -DCMAKE_INSTALL_PREFIX:PATH=H:\Proj\PDF4Ax\root32 -D BUILD_SHARED_LIBS:BOOL=OFF -G "Visual Studio 17 2022" -A Win32 ..
```

## jpeg

```bat
cmake -DCMAKE_INSTALL_PREFIX:PATH=H:\Proj\PDF4Ax\root32 -D ENABLE_STATIC:BOOL=ON -D ENABLE_SHARED:BOOL=OFF -G "Visual Studio 17 2022" -A Win32 ..
```

## libpng

```bat
cmake -DCMAKE_INSTALL_PREFIX:PATH=H:\Proj\PDF4Ax\root32 -D PNG_STATIC:BOOL=ON -D PNG_SHARED:BOOL=OFF -G "Visual Studio 17 2022" -A Win32 ..
```

## freetype

```bat
cmake -D CMAKE_INSTALL_PREFIX:PATH=H:\Proj\PDF4Ax\root32 -D BUILD_SHARED_LIBS:BOOL=OFF -D FT_REQUIRE_BZIP2:BOOL=OFF -D FT_REQUIRE_BROTLI:BOOL=OFF -G "Visual Studio 17 2022" -A Win32 ..
```

## iconv-wrap

```bat
cmake -D CMAKE_INSTALL_PREFIX:PATH=H:\Proj\PDF4Ax\root32 -G "Visual Studio 17 2022" -A Win32 ..
```

## pixman

- [end to end build for win32](https://www.cairographics.org/end_to_end_build_for_win32/)

```bat
pip3 install meson
meson build --backend vs2022 --prefix H:\Proj\PDF4Ax\root32 --buildtype debug --default-library static -Db_vscrt=static_from_buildtype
cd build
msbuild pixman.sln /p:Configuration=debug /p:Platform=Win32
msbuild RUN_INSTALL.vcxproj /p:Configuration=debug /p:Platform=Win32
```

- [MesonでVisual Studioのソリューションファイルを生成 - Mugichoko’s blog](https://mugichoko.hatenablog.com/entry/2020/02/14/184358)
- `b_vscrt`
  - [Make a fully statically linked .exe · Issue #1104 · mesonbuild/meson](https://github.com/mesonbuild/meson/issues/1104)
  - [Built-in options](https://mesonbuild.com/Builtin-options.html)

## lzo2

```bat
path %path%;H:\Proj\PDF4Ax\pkg-config\bin
set PKG_CONFIG_PATH=H:\Proj\PDF4Ax\root32\lib\pkgconfig
set PKG_CONFIG_LIBDIR=H:\Proj\PDF4Ax\root32\lib

cmake -D CMAKE_INSTALL_PREFIX:PATH=H:\Proj\PDF4Ax\root32 -G "Visual Studio 17 2022" -A Win32 ..
```

- pkg-config がないと .pc を作成しない。

## cairo

```bat
path %path%;H:\Proj\PDF4Ax\pkg-config\bin
set PKG_CONFIG_PATH=H:\Proj\PDF4Ax\root32\lib\pkgconfig
set PKG_CONFIG_LIBDIR=H:\Proj\PDF4Ax\root32\lib

meson build --backend vs2022 --prefix H:\Proj\PDF4Ax\root32 --buildtype debug -Dglib=disabled -Dspectre=disabled -Dfontconfig=disabled -Dpng=disabled --default-library static -Db_vscrt=static_from_buildtype

cd build

msbuild cairo.sln /p:Configuration=debug /p:Platform=Win32
msbuild RUN_INSTALL.vcxproj /p:Configuration=debug /p:Platform=Win32
```

## poppler

```bat
cmake -DCMAKE_INSTALL_PREFIX:PATH=H:\Proj\PDF4Ax\root32 -DCMAKE_BUILD_TYPE=DEBUG -DENABLE_BOOST=OFF -DENABLE_LIBOPENJPEG=none -DENABLE_UTILS:BOOL=OFF -DENABLE_ZLIB_UNCOMPRESS:BOOL=ON -DMSVC:BOOL=ON -DENABLE_UNSTABLE_API_ABI_HEADERS:BOOL=ON -G "Visual Studio 17 2022" -A Win32 ..
```
