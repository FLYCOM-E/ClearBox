#!/system/bin/sh
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
source "$work_dir/settings.prop"
if [ "$DebugPro" = 1 ]; then
    exec 2>>"$work_dir/运行日志.log"
else
    exec 2>>/dev/null
fi
AppDir="/data/data"
ClearDone=0
######
if [ ! -d "$work_dir/清理规则" ]; then
    rm -rf "$work_dir/清理规则"
    mkdir -p "$work_dir/清理规则"
fi
######
if [ -z "$(ls "$work_dir/清理规则/")" ]; then
    echo " » 无App清理配置！"
    exit 0
fi
######
function service()
{
for File in $(ls "$work_dir/清理规则/"); do
    Pro_File="$work_dir/清理规则/$File"
    if [ -d "$Pro_File" ]; then
        rm -r "$Pro_File"
        continue
    fi
    if [ -z "$(cat "$Pro_File")" ]; then
        echo " » $File：配置内容为空！自动跳过"
        continue
    fi
    if grep "$AppName" "$Pro_File" >> /dev/null; then
        count=0
        for i in $(cat "$Pro_File"); do
            count=$((count + 1))
            # 进入指定初始App目录
            if echo "$i" | grep ^"@" >/dev/null; then
                if ! echo "$i" | grep "/" >/dev/null; then
                    echo " » $File 配置第 $count 行初始错误！"
                    break
                fi
                AppPackage=$(echo "$i" | cut -f2 -d '@' | cut -f1 -d '/')
                Name=$(echo "$i" | cut -f2 -d '/')
                if [ -z "$AppPackage" ]; then
                    echo " » $File 配置未指定App包名！"
                    break
                fi
                if [ -z "$Name" ]; then
                    echo " » $File 配置未指定App名称！"
                    break
                fi
                ######
                if [ -d "$AppDir/$AppPackage/" ]; then
                    cd "$AppDir/$AppPackage/"
                    echo " » 清理 $Name &"
                    local pro=1
                    continue
                else
                    echo " » $File：配置指定软件未找到！"
                    break
                fi
                ######
            else
                if [ "$pro" = "1" ]; then
                    # 如果该行被注释则返回
                    if echo "$i" | grep ^"#" >/dev/null; then
                        continue
                    fi
                    if echo "$i" | grep ^"/" >/dev/null; then
                        echo " » $File：配置第 $count 行存在危险错误！"
                        break
                    fi
                    # 处理
                    if [ -f "$i" ]; then
                        rm "$i"
                    elif [ -d "$i" ]; then
                        rm -r "$i"
                    fi
                else
                    echo " » $File 配置初始错误！"
                fi
            fi
        done
        ClearDone=1
    else
        if [ "$ClearDone" = "1" ]; then
            break
        fi
    fi
done
######
if [ "$ClearDone" = "1" ]; then
    echo " » 软件规则清理成功！"
else
    echo " » 未找到指定软件！"
fi
}
######
if [ -z "$1" ]; then
    echo " » ERROR：需要一个参数，未传入软件名称！"
    exit 1
else
    AppName="$1"
    service &
    wait
fi
######
exit 0
