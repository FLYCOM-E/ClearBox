#!/system/bin/sh
#此脚本来自ClearBox模块，用于外部储存指定格式文件清理
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
    function ClearService()
    {
    echo " » 开始清理外部储存 $FileName！"
    if [ -f "$work_dir/文件格式配置/$FileName.xml" ]; then
        Name=$(echo "$NFile" | cut -f1 -d ".")
        count_num=0
        for Fn in $(cat "$work_dir/文件格式配置/$FileName.xml"); do
            for File in $("$bin_dir/busybox" find "$dir"/ -type f -name "*.$Fn"); do
                if [ -f "$File" ]; then
                    rm "$File"
                    count_num=$((count_num + 1))
                fi
            done
        done
        if [ "$count_num" -ge 1 ]; then
            echo " » 已清理 $count_num 个 $FileName"
        fi
    fi
    }
    ######
    function ClearServiceAll()
    {
    echo " » 开始深度清理外部储存文件！"
    for File in $(ls "$work_dir/文件格式配置/"); do
        if [ -f "$work_dir/文件格式配置/$File" ]; then
            Name=$(echo "$File" | cut -f1 -d ".")
            count_num=0
            for Fn in $(cat "$work_dir/文件格式配置/$File"); do
                for FileD in $("$bin_dir/busybox" find "$dir"/ -type f -name "*.$Fn"); do
                    if [ -f "$FileD" ]; then
                        rm "$FileD"
                        count_num=$((count_num + 1))
                    fi
                done
            done
            if [ "$count_num" -ge 1 ]; then
                echo " » 已清理 $count_num 个 $Name"
            fi
        fi
    done
    }
done
######
case $1 in
    *)
      FileName="$1"
      if [ "$FileName" = "" ]; then
          echo " » ERROR：需要一个参数，未传入清理项名称！"
          exit 1
      fi
      ;;
esac
if [ "$FileName" = "ALL" ]; then
    ClearServiceAll &
    wait
else
    ClearService &
    wait
fi
exit 0
