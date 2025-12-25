#!/bin/bash
if [ ! -d "$NDK" ]; then
    echo "Failed: \$NDK is space."
    exit 1
fi
if [ "$M_API" = "" ]; then
    echo "Failed: \$M_API is space."
    exit 1
fi
if [ "$M_TARGET" = "" ]; then
    echo "Failed: \$TARGET is space."
    exit 1
elif [ "$M_TARGET" = "aarch64" ]; then
    export TARGET=aarch64-linux-android
elif [ "$M_TARGET" = "armv7a" ]; then
    export TARGET=armv7a-linux-androideabi
elif [ "$M_TARGET" = "x86_64" ]; then
    export TARGET=x86_64-linux-android
elif [ "$M_TARGET" = "i686" ]; then
    export TARGET=i686-linux-android
else
    echo "TARGET Error! "
    exit 1
fi

export NDKTOOL="$NDK/toolchains/llvm/prebuilt/linux-x86_64"
export CC="$NDKTOOL/bin/${TARGET}${M_API}-clang"
export AR="$NDKTOOL/bin/llvm-ar"
export STRIP="$NDKTOOL/bin/llvm-strip"
export CFLAGS="-target ${TARGET}${M_API} -fPIC -D__ANDROID_API__=$M_API"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="-target ${TARGET}${M_API}"

make -j$(nproc)
make module_tar && echo -e " » 打包完成，成品：ClearBox_${M_TARGET}_${M_API}_${M_LANG}.zip！\n » Tar Done, Is: ClearBox_${M_TARGET}_${M_API}_${M_LANG}.zip! "
make clean && echo -e " » 清理完成！\n » Clean Done! "
