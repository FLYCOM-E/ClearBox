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
data_dir1="/data/user"
data_dir2="/data/user_de"
micro_dir1="/mnt/expand/$(ls /mnt/expand)/user"
micro_dir2="/mnt/expand/$(ls /mnt/expand)/user_de"
whitelist="$work_dir/ClearWhitelist.prop"
######
function WipeCache1()
{
# 遍历清空内部储存软件cache文件夹
ls "$data_dir1/" | while read userid_dir; do
    for UserAppList in $(pm list package -3 | cut -f2 -d ':'); do
        if grep ^"$UserAppList" "$whitelist" >/dev/null; then
            continue
        fi
        if [ ! -d "/data/user/$userid_dir/$UserAppList" ]; then
            continue
        fi
        rm -rf "$data_dir1/$userid_dir/$UserAppList/cache/"* &
        rm -rf "$data_dir1/$userid_dir/$UserAppList/code_cache/"* &
        rm -rf "$data_dir2/$userid_dir/$UserAppList/cache/"* &
        rm -rf "$data_dir2/$userid_dir/$UserAppList/code_cache/"* &
        wait
        echo " $UserAppList 缓存已清除"
    done
done
echo " » 内部储存软件缓存删除完成"
}
######
function WipeCache2()
{
if [ ! -d "$micro_dir1" ]; then
    exit 0
fi
if [ "$cleardisk" = 0 ]; then
    exit 0
fi
######
# 遍历清空软件cache文件夹
ls "$micro_dir1/" | while read userid_dir; do
    for CardAppList in $(ls "$micro_dir1/$userid_dir"); do
        if grep ^"$CardAppList" "$whitelist" >/dev/null; then
            continue
        fi
        rm -rf "$micro_dir1/$userid_dir/$CardAppList/cache/"* &
        rm -rf "$micro_dir1/$userid_dir/$CardAppList/code_cache/"* &
        rm -rf "$micro_dir2/$userid_dir/$CardAppList/cache/"* &
        rm -rf "$micro_dir2/$userid_dir/$CardAppList/code_cache/"* &
        wait
        echo " $CardAppList 缓存已清除"
    done
done
echo " » 外部储存软件缓存删除完成"
}
######
WipeCache1 &
WipeCache2 &
wait

exit 0
