#!/bin/env sh
set -e

if [ ! -d "$ANDROID_NDK_HOME" ]; then
    echo "  Failed: \$ANDROID_NDK_HOME is space."
    exit 1
fi
if [ "$TARGET_API" = "" ]; then
    echo "  Failed: \$TARGET_API is space."
    exit 1
fi

export module_dir="ModuleFiles"
export app_dir="AppSource"
export tui_dir="TuiSource"
export file_config="FileConfigs"
export app_config="AppConfigs"
export storage_config="StorageConfigs"
export lang_config="LangConfigs"
TARGET_LIST="
aarch64-linux-android:arm64
armv7a-linux-androideabi:arm
x86_64-linux-android:x64
"

if [ "$1" = "-build-apk" ] || [ "$1" = "--build-apk" ]; then
    if [ "$2" = "" ]; then
        echo "  BUILD APK \t\t debug"
    else
        echo "  BUILD APK \t\t $2"
    fi
    
    cd "$app_dir"
    chmod +x ./gradlew
    if [ "$2" = "release" ]; then
        ./gradlew assembleRelease --no-daemon
    else
        ./gradlew assembleDebug --no-daemon
    fi
    cd ..
fi

echo "$TARGET_LIST" | while IFS=':' read -r abi abi_name; do
    [ -z "$abi" ] && continue
    echo "  BUILD ELF \t\t $abi_name"
    
    export NDKTOOL="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64"
    export CC="$NDKTOOL/bin/${abi}${TARGET_API}-clang"
    export AR="$NDKTOOL/bin/llvm-ar"
    export STRIP="$NDKTOOL/bin/llvm-strip"
    export CFLAGS="-target ${abi}${TARGET_API} -fPIC -D__ANDROID_API__=$TARGET_API"
    export CXXFLAGS="$CFLAGS"
    export LDFLAGS="-target ${abi}${TARGET_API}"
    
    make -j$(nproc) # ERROR: top set -e
    
    mkdir -p "$module_dir/bin/$abi_name"
    cp "$module_dir/clearbox" "$module_dir/bin/$abi_name/clearbox"
    
    make clean
done

echo "  ZIP \t ClearBox_$TARGET_API.zip"

find "$app_dir" -name "*.apk" -exec cp {} "$module_dir/ClearBox.apk" \;
cp "$module_dir/system/bin/ClearBox" ./ClearBox.bak
cp "$tui_dir/Main.bash" "$module_dir/system/bin/ClearBox"
cp -r "$lang_config" "$module_dir/LANG"
cp -r "$app_config" "$module_dir/"
cp -r "$storage_config" "$module_dir/"
cp -r "$file_config" "$module_dir/"

cd "$module_dir"
zip -rq ../"ClearBox_$TARGET_API.zip" *
cd -

rm -f "$module_dir/ClearBox.apk"
mv ./ClearBox.bak "$module_dir/system/bin/ClearBox"
rm -r "$module_dir/LANG"
rm -r "$module_dir/$app_config"
rm -r "$module_dir/$file_config"
rm -r "$module_dir/bin"
