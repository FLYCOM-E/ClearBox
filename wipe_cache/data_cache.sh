#!/system/bin/sh
#此脚本来自ClearBox模块，用于清空内部储存软件缓存
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
data_dir="/data/user"
micro_dir="/mnt/expand/$(ls /mnt/expand)/user"
whitelist="$work_dir/ClearWhitelist.prop"
######
WipeCache()
{
# 遍历清空内部储存软件cache文件夹
for userid_dir in "$dir"/*; do
    for UserAppList in $(pm list package -3 | cut -f2 -d ':'); do
        if grep ^"$UserAppList" "$whitelist" >/dev/null; then
	        continue
	    fi
        if [ ! -d "$userid_dir/$UserAppList" ]; then
            continue
        elif [ "$(du -s -m "$userid_dir/$UserAppList/cache" | cut -f1 -d '	')" -lt "$ClearCacheSize" ]; then
            echo " » 跳过 $UserAppList"
            continue
        fi
        rm -rf "$userid_dir/$UserAppList/cache/"* &
        rm -rf "$userid_dir/$UserAppList/code_cache/"* &
        wait
        echo " » $UserAppList 缓存已清除"
    done
done
}
######
dir="$data_dir"
WipeCache && echo " » 内部储存软件缓存删除完成"
[ "$cleardisk" = 0 ] && exit 0
if [ -d "$micro_dir" ]; then
    dir="$micro_dir"
    WipeCache && echo " » 外部储存软件缓存删除完成"
fi
exit 0
