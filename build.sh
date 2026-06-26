#!/bin/bash
set -e

if [ ! -d "$ANDROID_NDK_HOME" ]; then
    echo "Failed: \$ANDROID_NDK_HOME is space."
    exit 1
fi
if [ "$TARGET_API" = "" ]; then
    echo "Failed: \$TARGET_API is space."
    exit 1
fi
if [ "$TARGET" = "" ]; then
    echo "Failed: \$TARGET is space."
    exit 1
elif [ "$TARGET" = "aarch64" ]; then
    export TARGET=aarch64-linux-android
elif [ "$TARGET" = "armv7a" ]; then
    export TARGET=armv7a-linux-androideabi
elif [ "$TARGET" = "x86_64" ]; then
    export TARGET=x86_64-linux-android
elif [ "$TARGET" = "i686" ]; then
    export TARGET=i686-linux-android
elif [ "$TARGET" = "riscv64" ]; then
    export TARGET=riscv64-linux-android
else
    echo "TARGET Error! "
    exit 1
fi

export NDKTOOL="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64"
export CC="$NDKTOOL/bin/${TARGET}${TARGET_API}-clang"
export AR="$NDKTOOL/bin/llvm-ar"
export STRIP="$NDKTOOL/bin/llvm-strip"
export CFLAGS="-target ${TARGET}${TARGET_API} -fPIC -D__ANDROID_API__=$TARGET_API"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="-target ${TARGET}${TARGET_API}"

make -j$(nproc)
if [ "$1" = "-build-apk" ] || [ "$1" = "--build-apk" ]; then
    cd AppSource/
    chmod +x ./gradlew
    if [ "$2" = "release" ]; then
        ./gradlew assembleRelease --no-daemon
    else
        ./gradlew assembleDebug --no-daemon
    fi
    cd ..
fi

make module_tar && echo -e " » 打包完成，成品：ClearBox_${TARGET}_${TARGET_API}.zip！\n » Tar Done, Is: ClearBox_${TARGET}_${TARGET_API}.zip! "
make clean && echo -e " » 清理完成！\n » Clean Done! "
