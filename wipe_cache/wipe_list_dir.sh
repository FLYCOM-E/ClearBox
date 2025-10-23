#!/system/bin/sh
#此脚本来自ClearBox模块，用于自定义规则清理/干掉文件目录
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
######
mkdir -p "$work_dir/清理配置"
######
[ -z "$(ls "$work_dir/清理配置/")" ] && exit 0
for Pro_File in "$work_dir/清理配置"/*; do
    Pro_File_Name=$(echo "$Pro_File" | cut -f6 -d '/')
    
    [ -d "$Pro_File" ] && rm -r "$Pro_File" && continue
    [ ! -f "$Pro_File" ] && echo " » $Pro_File_Name：配置读取错误，请检查！" && continue
    if [ -z "$(cat "$Pro_File")" ]; then
        echo " » $Pro_File_Name：配置内容为空！"
        continue
    else
        echo " » 处理 $Pro_File_Name 配置📍"
    fi
    ######
    count=0
    for i in $(cat "$Pro_File"); do
        count=$((count + 1))
        # 进入指定初始目录
        if echo "$i" | grep ^"@" >/dev/null; then
            dir=$(echo "$i" | grep ^"@" | cut -f2 -d '@')
            if [ -d "$dir" ]; then
                cd "$dir"
            fi
            continue
        fi
        ######
        # 如果该行被注释或以/开头则返回
        if echo "$i" | grep ^"#" >/dev/null; then
            continue
	    fi
	    # 处理
        if [ -f "$df" ]; then
            echo " » 正在删除文件 $df"
            rm -rf "$df"
        elif [ -d "$df" ]; then
            echo " » 正在删除目录 $df"
            rm -rf "$df"
        fi
    done
done
######
echo " » 自定义目录处理完成！"
exit 0
