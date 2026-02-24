#!/system/bin/sh
#此脚本来自ClearBox模块，升级过渡 / 配置纠错脚本
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
exec 2>>/dev/null
######
[ -n "$cleardisk" ] && sed -i 's|cleardisk=|clearbox_clear_disk=|g' "$work_dir/settings.prop"
[ -n "$stopcache" ] && sed -i 's|stopcache=|clearbox_stop_cache=|g' "$work_dir/settings.prop"
[ -n "$stopinstall" ] && sed -i 's|stopinstall=|clearbox_stop_install=|g' "$work_dir/settings.prop"
[ -n "$stopstorage" ] && sed -i 's|stopstorage=|clearbox_stop_storage=|g' "$work_dir/settings.prop"
[ -n "$fileall" ] && sed -i 's|fileall=|clearbox_auto_file_all=|g' "$work_dir/settings.prop"
[ -n "$Fileall_Disk" ] && sed -i 's|Fileall_Disk=|clearbox_file_all_disk=|g' "$work_dir/settings.prop"
[ -n "$FileClear_Disk" ] && sed -i 's|FileClear_Disk=|clearbox_file_clear_disk=|g' "$work_dir/settings.prop"
[ -n "$DebugPro" ] && sed -i 's|DebugPro=|clearbox_debug_mode=|g' "$work_dir/settings.prop"
[ -n "$ClearCacheSize" ] && sed -i 's|ClearCacheSize=|clearbox_clear_cache_size=|g' "$work_dir/settings.prop"

[ "$clearbox_clear_cache_size" -gt 100 ] && sed -i 's|clearbox_clear_cache_size=[0-9]*|clearbox_clear_cache_size=5|g' "$work_dir/settings.prop"
sed -i '/^$/d' "$work_dir/settings.prop"

true
