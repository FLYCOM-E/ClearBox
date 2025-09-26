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
for cd1 in "$dir/Android/data"/*; do
    if grep "$cd1" "$whitelist" >/dev/null; then
        continue
    fi
    rm -rf "$dir/Android/data/$cd1/cache/"*
    echo " $cd1 缓存已清除"
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
"$bin_dir/busybox" find "$dir/" -type d -empty -delete &
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
    for cd2 in "$dir2/Android/data"/*; do
        if grep "$cd2" "$whitelist" >/dev/null; then
           continue
        fi
        rm -rf "$dir/Android/data/$cd2/cache/"*
        echo " $cd2 缓存已清除"
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
    "$bin_dir/busybox" find "$dir2/" -type d -empty -delete &
    wait
    
    echo " » 外部储存 $diskdir 垃圾删除完成！"
done
######
exit 0
