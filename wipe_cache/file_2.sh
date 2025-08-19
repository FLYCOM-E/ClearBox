#!/system/bin/sh
#此脚本来自ClearBox模块，用于外部储存文件收集归类
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
######
if ! ls /storage | grep .*- >/dev/null; then
    exit 0
fi
if grep "Fileall_Disk=0" "$work_dir/settings.prop" >/dev/null; then
    exit 0
fi
######
ls /storage | grep .*- | while read diskdir; do
    dir="/storage/$diskdir"
    ######
    function FileAll()
    {
    ls "$work_dir/文件格式配置/" | while read NFile; do
        if [ -d "$NFile" ]; then
            rm -r "$NFile"
            continue
        else
            Name=$(echo "$NFile" | cut -f1 -d ".")
        fi
        mkdir -p "$dir/Documents/$Name"
        File_dir="$dir/Documents/$Name"
        count_num=0
        for Fn in $(cat "$work_dir/文件格式配置/$NFile"); do
            for File in $("$bin_dir/busybox" find "$dir"/ -type f -name "*.$Fn"); do
                if [ -f "$File" ]; then
                    mv "$File" "$File_dir"
                    count_num=$((count_num + 1))
                fi
            done
        done
        if [ "$count_num" -ge 1 ]; then
            echo " » 已归类 $count_num 个 $Name"
        fi
    done
    }
    ######
    FileAll &
    wait
    echo " » 外部储存文件整理完成！"
    ######
done
######
exit 0
