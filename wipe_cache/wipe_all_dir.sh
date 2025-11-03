#!/system/bin/sh
#此脚本来自ClearBox模块，用于删除垃圾缓存及空文件夹
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
whitelist="$work_dir/ClearWhitelist.prop"
dir="/storage/emulated/0"
######
ls "$dir/Android/data/" | while read cd1; do
    if grep "$cd1" "$whitelist" >/dev/null; then
        continue
    elif [ ! -d "$dir/Android/data/$cd1/cache" ]; then
        continue
    elif [ "$(du -s -m "$dir/Android/data/$cd1/cache" | cut -f1 -d '	')" -lt "$ClearCacheSize" ]; then
        echo " » 跳过 $UserAppList"
        continue
    fi
    rm -rf "$dir/Android/data/$cd1/cache/"*
done

MediaCache()
{
[ -d "$dir/Pictures/.thumbnails" ] && rm -r "$dir/Pictures/.thumbnails"
[ -d "$dir/Movies/.thumbnails" ] && rm -r "$dir/Movies/.thumbnails"
[ -d "$dir/music/.thumbnails" ] && rm -r "$dir/music/.thumbnails"
[ -d "$dir/DCIM/.thumbnails" ] && rm -r "$dir/DCIM/.thumbnails"
}
MediaCache &

"$bin_dir/busybox" find "$dir/" -type f -name "*.log" -delete &
"$bin_dir/busybox" find "$dir/" -type f -name "*.LOG" -delete &
# 如stopstorage（内部储存固定）已开启则不清理空文件夹
if [ "$stopstorage" = 0 ]; then
    "$bin_dir/busybox" find "$dir/" -type d -empty -delete &
fi
wait

echo " » 内部储存垃圾删除完成！"
######
if ! ls /storage | grep .*- >/dev/null; then
    exit 0
fi
[ "$cleardisk" = 0 ] && exit 0
######
ls /storage | grep .*- | while read diskdir; do
    dir2="/storage/$diskdir"
    ######
    ls "$dir2/Android/data/" | while read cd2; do
        if grep "$cd2" "$whitelist" >/dev/null; then
           continue
        elif [ ! -d "$dir/Android/data/$cd2/cache" ]; then
            continue
        elif [ "$(du -s -m "$dir/Android/data/$cd2/cache" | cut -f1 -d '	')" -lt "$ClearCacheSize" ]; then
            echo " » 跳过 $UserAppList"
            continue
        fi
        rm -rf "$dir/Android/data/$cd2/cache/"*
    done
    
    MediaCache2()
    {
    [ -d "$dir2/Pictures/.thumbnails" ] && rm -r "$dir2/Pictures/.thumbnails"
    [ -d "$dir2/Movies/.thumbnails" ] && rm -r "$dir2/Movies/.thumbnails"
    [ -d "$dir2/music/.thumbnails" ] && rm -r "$dir2/music/.thumbnails"
    [ -d "$dir2/DCIM/.thumbnails" ] && rm -r "$dir2/DCIM/.thumbnails"
    }
    MediaCache2 &

    "$bin_dir/busybox" find "$dir2/" -type f -name "*.log" -delete &
    "$bin_dir/busybox" find "$dir2/" -type f -name "*.LOG" -delete &
    # 处理外部储存因此不检查内部储存固定功能状态
    "$bin_dir/busybox" find "$dir2/" -type d -empty -delete &
    wait
    
    echo " » 外部储存 $diskdir 垃圾删除完成！"
done
######
exit 0
