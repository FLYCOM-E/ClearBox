#!/system/bin/sh
#此脚本来自ClearBox模块，用于清空系统缓存
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！"
    exit 1
elif ! ClearBox -v >/dev/null; then
    echo " » 模块加载异常，请排查反馈！"
    exit 1
fi
######
bin_dir=$(ClearBox -b)
home_dir=$(ClearBox -h)
work_dir=$(ClearBox -w)
source "$work_dir/settings.prop"
if [ "$DebugPro" = 1 ]; then
    exec 2>>"$work_dir/运行日志.log"
else
    exec 2>>/dev/null
fi
data_dir1="/data/user"
######
# 遍历清空系统组件cache文件夹
for userid_dir in "$data_dir1"/*; do
    for SystemAppList in $(pm list package -s | cut -f2 -d ':'); do
        if grep ^"$SystemAppList" "$whitelist" >/dev/null; then
            continue
        fi
        if [ ! -d "$userid_dir/$SystemAppList" ]; then
            continue
        elif [ "$(du -s -m "$userid_dir/$SystemAppList/cache" | cut -f1 -d '	')" -lt "$ClearCacheSize" ]; then
            echo " » 跳过 $SystemAppList"
            continue
        fi
        rm -r "$userid_dir/$SystemAppList/cache/"* &
        rm -r "$userid_dir/$SystemAppList/code_cache/"* &
        wait
        echo " $SystemAppList 缓存已清除"
    done
done
######
# 清除“MTP主机”组件数据可解决MTP连接文件显示不全的问题
pm clear "com.android.mtp" >>/dev/null
######
# 清空系统缓存
rm -r /cache/* &
rm -r /data/resource-cache/* &
rm -r /data/system/package_cache/* &
rm -r /data/dalvik-cache/* &
wait
######
echo " » 系统缓存已清空！建议重启系统！"
exit 0
