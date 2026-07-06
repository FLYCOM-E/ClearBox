#!/system/bin/env sh
#此脚本用于将读取的软件从白名单移除
for pk in $Package; do
    sed -i /"$pk"/d "$work_dir/whitelist.prop"
    if [[ $? = 0 ]]; then
        echo " » $pk 已成功从白名单中移除！"
    else
        echo " » $pk 移除失败或不在白名单中！"
    fi
done
