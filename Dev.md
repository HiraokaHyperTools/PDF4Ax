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
meson build --backend vs2022 --prefix H:\Proj\PDF4Ax\root32 --buildtype debugoptimized
msbuild pixman.sln /p:Configuration=debugoptimized /p:Platform=Win32
msbuild RUN_INSTALL.vcxproj /p:Configuration=debugoptimized /p:Platform=Win32
```

- [MesonでVisual Studioのソリューションファイルを生成 - Mugichoko’s blog](https://mugichoko.hatenablog.com/entry/2020/02/14/184358)

## lzo2

```bat
path %path%;C:\msys32\mingw32\bin

cmake -D CMAKE_INSTALL_PREFIX:PATH=H:\Proj\PDF4Ax\root32 -D CMAKE_DEBUG_POSTFIX=d -G "Visual Studio 17 2022" -A Win32 ..
```

- pkg-config がないと .pc を作成しない。

## cairo

```bat
path %path%;C:\msys32\mingw32\bin

meson build --backend vs2022 --prefix H:\Proj\PDF4Ax\root32 --buildtype debugoptimized -Dglib=disabled -Dspectre=disabled -Dfontconfig=disabled -Dfreetype=disabled -Dpng=disabled

msbuild cairo.sln /p:Configuration=debugoptimized /p:Platform=Win32
msbuild RUN_INSTALL.vcxproj /p:Configuration=debugoptimized /p:Platform=Win32
```

## poppler

```bat
H:\Proj\PDF4Ax\poppler\build>cmake -DCMAKE_INSTALL_PREFIX:PATH=H:\Proj\PDF4Ax\root32 -DCMAKE_BUILD_TYPE=DEBUG -DENABLE_BOOST=OFF -DENABLE_LIBOPENJPEG=none -DENABLE_UTILS:BOOL=OFF -DENABLE_ZLIB_UNCOMPRESS:BOOL=ON -G "Visual Studio 17 2022" -A Win32 ..
```
