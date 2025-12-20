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
source "$work_dir/settings.prop"
source "$home_dir/语言包/Local.conf"
if [ "$DebugPro" = 1 ]; then
    exec 2>>"$work_dir/运行日志.log"
else
    exec 2>>/dev/null
fi
######
# 还原模块设置并执行卸载
uninstall_md()
{
    touch "$home_dir/disable"
    touch "$home_dir/remove"
    rm -r "/sdcard/Android/ClearBox"
    "$bin_dir/busybox" chattr -R -i /data/app
    "$bin_dir/busybox" chattr -R -i /data/user
    "$bin_dir/busybox" chattr -R -i /mnt/expand
}
######
uninstall_md
echo -e " » $UNINSTALL_TITLE😋"
sleep 3
rm -r "$work_dir"
pm uninstall "wipe.cache.module"
exit 0
