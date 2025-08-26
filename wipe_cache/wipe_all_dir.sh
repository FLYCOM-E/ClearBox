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
#exec 2>>/dev/null
exec 2>>"$work_dir/运行日志.log"
whitelist="$work_dir/ClearWhitelist.prop"
dir="/storage/emulated/0"
dir2="/storage/$(ls /storage | grep .*-)"
######
for c in $(ls "$dir"/Android/data); do
    if grep "$c" "$whitelist" >/dev/null; then
        continue
    fi
    rm -r "$dir"/Android/data/"$c"/cache/*
    echo " $c 缓存已清除"
done
rm -r "$dir"/Pictures/.thumbnails
rm -r "$dir"/Movies/.thumbnails
rm -r "$dir"/music/.thumbnails
rm -r "$dir"/DCIM/.thumbnails
"$bin_dir/busybox" find "$dir" -type d -empty -delete
"$bin_dir/busybox" find "$dir"/ -type f -name "*.log" -delete &
"$bin_dir/busybox" find "$dir"/ -type f -name "*.LOG" -delete &
wait
echo " » 内部储存垃圾删除完成！"
######
if ! ls /storage | grep .*- >/dev/null; then
    exit 0
fi
if grep "cleardisk=0" "$work_dir/settings.prop" >/dev/null; then
    exit 0
fi
######
ls /storage | grep .*- | while read diskdir; do
    dir="/storage/$diskdir"
    
    for v in $(ls "$dir2"/Android/data); do
        if grep "$v" "$whitelist" >/dev/null; then
           continue
        fi
        rm -r "$F"/Android/data/"$v"/cache/*
        echo " $v 缓存已清除"
    done
    rm -r "$dir2"/Pictures/.thumbnails
    rm -r "$dir2"/Movies/.thumbnails
    rm -r "$dir2"/music/.thumbnails
    rm -r "$dir2"/DCIM/.thumbnails
    "$bin_dir/busybox" find "$dir2" -type d -empty -delete
    "$bin_dir/busybox" find "$dir2"/ -type f -name "*.log" -delete &
    "$bin_dir/busybox" find "$dir2"/ -type f -name "*.LOG" -delete &
    wait
    echo " » 外部储存 $diskdir 垃圾删除完成！"
done
######
exit 0
