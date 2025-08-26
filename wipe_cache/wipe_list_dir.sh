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
#exec 2>>/dev/null
exec 2>>"$work_dir/运行日志.log"
######
if [ -f "$work_dir/清理配置" ]; then
    rm -rf "$work_dir/清理配置"
fi
mkdir -p "$work_dir/清理配置"
######
if [ "$(ls "$work_dir/清理配置/")" = "" ]; then
    exit 0
fi
ls "$work_dir/清理配置/" | while read File; do
    Pro_File="$work_dir/清理配置/$File"
    if [ -d "$Pro_File" ]; then
        continue
    elif [ ! -f "$Pro_File" ]; then
        " » $File：配置读取错误，请检查！"
    fi
    if [ "$(cat "$Pro_File")" = "" ]; then
        echo " » $File：配置内容为空！"
        continue
    else
        echo " » 处理 $File 配置📍"
    fi
    ######
    count=0
    for i in $(cat "$Pro_File"); do
        count=$((count + 1))
        df=$(echo "$i" | cut -f2 -d '=')
        ######
        # 进入指定初始目录
        if echo "$i" | grep ^"@" >/dev/null; then
            dir=$(echo "$i" | grep ^"@" | cut -f2 -d '@')
            if [ -d "$dir" ]; then
                cd "$dir"
            else
                echo " » $Pro_File：配置指定初始目录错误！"
            fi
            continue
        fi
        ######
        if echo "$i" | grep ^"/" >/dev/null; then
            echo " » $Pro_File：配置第 $count 行存在错误！"
            continue
        fi
        # 如果该行被注释则返回
        if echo "$i" | grep ^"#" >/dev/null; then
            continue
        fi
        # 设置clear参数则删除该文件夹
        if echo "$i" | grep ^"CLEAR" >/dev/null; then
            if [ -f "$df" ]; then
                echo " » 正在删除 $df"
                rm -r "$df"
            elif [ -d "$df" ]; then
                echo " » 正在删除 $df"
                rm -r "$df"
            fi
        # 设置kill参数则将对应文件夹替换为文件
        elif echo "$i" | grep ^"KILL" >/dev/null; then
            if [ -d "$df" ]; then
                echo " » 正在强制干掉 $df"
                rm -r "$df"
                touch "$df"
            fi
        else
            echo " » $Pro_File：配置第 $count 行未指定参数！"
        fi
    done
done
######
echo " » 自定义目录处理完成！"

exit 0
