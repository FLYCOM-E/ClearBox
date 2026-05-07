#!/system/bin/sh
#此脚本来自ClearBox模块，用于完全卸载模块并清理残留
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！Please grant root privileges!"
    exit 1
fi
######
if [ ! -f "/data/adb/wipe_cache/PATH" ]; then
    echo " » Error：PATH读取错误！PATH read error!"
    exit 1
else
    source "/data/adb/wipe_cache/PATH"
fi
######
bin_dir=""
if [ -d "/data/adb/magisk" ]; then
    bin_dir="/data/adb/magisk"
elif [ -d "/data/adb/ap/bin" ]; then
    bin_dir="/data/adb/ap/bin"
elif [ -d "/data/adb/ksu/bin" ]; then
    bin_dir="/data/adb/ksu/bin"
else
    # 这是一个回退方案
    for busybox in $(find /data/adb -name "busybox"); do
        ln -s "$busybox" "$home_dir/busybox"
    done
    bin_dir="$home_dir"
fi
######
exec 2>>/dev/null
source "$work_dir/settings.prop"
lang_dir="LANG"
if getprop persist.sys.locale | grep -E "zh-CN|Hans" 2>/dev/null; then
    source "$home_dir/$lang_dir/zh_CN.conf"
elif getprop persist.sys.locale | grep -E "zh-TW|Hant" 2>/dev/null; then
    source "$home_dir/$lang_dir/zh_TW.conf"
else
    source "$home_dir/$lang_dir/en_US.conf"
fi
######
# 还原模块设置并执行卸载
uninstall_md()
{
    touch "$home_dir/disable"
    touch "$home_dir/remove"
    rm -r "/sdcard/Android/ClearBox"
    
    dirList="/data/app /data/media/0 /mnt/expand /mnt/media_rw"
    # 解锁
    for dir in $dirList; do
        "$bin_dir/busybox" chattr -R -i "$dir"
    done
    
    kill $(pgrep StopCached)
    for path in "/data/user"/*; do
        "$bin_dir/busybox" chattr -R -i "$path"/* 2>/dev/null
    done
}
######
uninstall_md
echo -e " » $UNINSTALL_TITLE😋"
sleep 3
rm -r "$work_dir"
cmd package uninstall "wipe.cache.module"
exit 0
