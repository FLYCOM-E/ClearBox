#!/system/bin/sh
#此脚本用于将读取的软件从白名单移除
for C in "$Package"; do
    sed -i /"$C"/d "$work_dir/ClearWhitelist.prop"
    if [[ $? = 0 ]]; then
        echo " $C 已成功从白名单中移除！"
    else
        echo " $C 移除失败或不在白名单中！"
    fi
done
