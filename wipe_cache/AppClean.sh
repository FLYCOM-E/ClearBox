#!/system/bin/sh
# 暂无调用，此脚本只是半成品
#此脚本来自ClearBox模块，用于自定义规则软件内部清理
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
AppDir="/data/data"
######
if [ -f "$work_dir/清理规则" ]; then
    rm -rf "$work_dir/清理规则"
fi
mkdir -p "$work_dir/清理规则"
######
if [ "$(ls "$work_dir/清理规则/")" = "" ]; then
    echo " » App清理配置为空！"
    exit 0
fi
ls "$work_dir/清理规则/" | while read File; do
    Pro_File="$work_dir/清理规则/$File"
    if [ -d "$Pro_File" ]; then
        rm -r "$Pro_File"
        continue
    elif [ ! -f "$Pro_File" ]; then
        " » $File：配置文件错误，请检查！"
    fi
    if [ "$(cat "$Pro_File")" = "" ]; then
        echo " » $File：配置内容为空！自动跳过"
        continue
    fi
    ######
    for i in $(cat "$Pro_File"); do
        ######
        # 进入指定初始App目录
        if echo "$i" | grep ^"@" >/dev/null; then
            AppPackage=$(echo "$i" | cut -f2 -d '@')
            if [ "$AppPackage" = "" ]; then
                echo " » $File 配置未指定App，请检查！"
                break
            fi
            cd "$AppDir/$AppPackage/"
            echo " » 清理$AppPackage &"
            continue
        fi
        ######
        if echo "$i" | grep ^"/" >/dev/null; then
            echo " » $Pro_File：配置存在错误，请检查！！"
            exit 1
        fi
        # 如果该行被注释则返回
        if echo "$i" | grep ^"#" >/dev/null; then
            continue
        fi
        # 处理
        if [ -f "$i" ]; then
            rm "$i"
        elif [ -d "$i" ]; then
            rm -r "$i"
        fi
    done
done
######
echo " » 软件规则处理完成！"

exit 0
