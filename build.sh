#!/bin/bash
if [ ! -d "$NDK" ]; then
    echo "Failed: \$NDK is space."
    exit 1
fi
if [ "$API" = "" ]; then
    echo "Failed: \$API is space."
    exit 1
fi

if [ "$1" = "aarch64" ]; then
    export TARGET=aarch64-linux-android
elif [ "$1" = "armv7a" ]; then
    export TARGET=armv7a-linux-androideabi
elif [ "$1" = "x86_64" ]; then
    export TARGET=x86_64-linux-android
elif [ "$1" = "i686" ]; then
    export TARGET=i686-linux-android
else
    echo "Mode Error! "
    exit 1
fi

export NDKTOOL="$NDK/toolchains/llvm/prebuilt/linux-x86_64"
export CC="$NDKTOOL/bin/${TARGET}${API}-clang"
export AR="$NDKTOOL/bin/llvm-ar"
export STRIP="$NDKTOOL/bin/llvm-strip"
export CFLAGS="-target ${TARGET}${API} -fPIC -D__ANDROID_API__=$API"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="-target ${TARGET}${API}"

make -j$(nproc)
make module_tar && echo " » 打包完成，成品：ClearBox_$M_LANG.zip！\n » Tar Done, Is: ClearBox_$M_LANG.zip! "
make clean && echo " » 清理完成！\n » Clean Done! "
