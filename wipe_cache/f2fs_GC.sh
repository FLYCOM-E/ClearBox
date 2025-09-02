#!/system/bin/sh
# 此脚本来自ClearBox模块，用于运行紧急GC优化，原理来自Coolapk@Amktiao，感谢大佬
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
get_f2fs_sysfs="/sys/fs/f2fs/$(getprop dev.mnt.dev.data)"
######
function f2fs_gc()
    {
    # 检测是否为f2fs文件系统
    if [ ! -d "$get_f2fs_sysfs" ]; then
        echo " » 您的设备不是 F2FS 文件系统"
        echo " » 维护仅支持 F2FS 环境"
        exit 0
    # 检测是否支持当前gc方案
    elif [ ! -f "$get_f2fs_sysfs/gc_urgent" ]; then
        echo " » 您的设备不支持当前GC功能"
        exit 0
    fi
    ######
    function check_dirty()
    {
    Dirty=$(cat "$get_f2fs_sysfs/dirty_segments")
    Free=$(cat "$get_f2fs_sysfs/free_segments")
    echo " » 目前脏段: $Dirty"
    echo " » 目前空闲段: $Free"
    echo "         "
    }
    ######
    # 开始运行gc并等待节点归零（完成gc）
    check_dirty
    if $(echo 1 > "$get_f2fs_sysfs/gc_urgent"); then
        echo " » GC已开始, 请您耐心等待，建议挂后台！"
    else
        echo " » GC启动失败！“$get_f2fs_sysfs/gc_urgent”节点写入失败！"
        exit 1
    fi
    time=0
    time2=0
    while true; do
        if [ "$time" = 60 ]; then
            time=0
            if [ "$time2" -lt 1 ]; then
                time2=1
            else
                time2=$((time2 + 1))
            fi
            timeM=" $time2 分"
        fi
        echo -ne " » 已运行$timeM $time 秒...\r"
        if [ "$time2" = 10 ]; then
            echo " » GC等待超时，已结束等待！"
            echo "         "
            break
        fi
        time=$((time + 5))
        if [ $(cat "$get_f2fs_sysfs/gc_urgent") = 0 ]; then
            echo " » GC运行完成，已结束运行！"
            break
        fi
        sleep 5
    done
    ######
    OldDirty="$Dirty"
    OldFree="$Free"
    check_dirty
    NewDirty=$((OldDirty - Dirty))
    if [ "$OldDirty" -ge "$Dirty" ]; then
        echo " » 磁盘脏块减少 $NewDirty"
    else
        echo " » 磁盘脏块增加 $NewDirty"
        echo " » GC可能仍在优化或并不适合您的设备！"
    fi
    ######
    echo "         "
    echo " [ $(date) ] GC已完成！"
    }
######
function idle-maint()
{
echo " » 开始快速磁盘优化，请您耐心等待，可以离开前台！"
if [ "$(sm idle-maint run >/dev/null)" = "" ]; then
    echo " » 优化完成，可以试试更激进的GC优化哦 (・∀・)"
    i="成功"
else
    i="失败"
fi
echo "[ $(date) ]：快速磁盘优化 <$i>" >> "$work_dir/运行日志.log"
}
######
case $1 in
    F2FS_GC)
        f2fs_gc
        ;;
    FAST_GC)
        idle-maint
        ;;
    *)
        if [ "$1" = "" ]; then
            echo " » ERROR：需要一个参数，未传入选项名称！"
            exit 1
        fi
        ;;
esac
######
exit 0
