#!/system/bin/sh
#此脚本用于将读取的软件从白名单移除
for pk in $Package; do
    sed -i /"$pk"/d "$work_dir/ClearWhitelist.prop"
    if [[ $? = 0 ]]; then
        echo " » $pk 已成功從白名單中移除！"
    else
        echo " » $pk 移除失敗或不在白名單中！"
    fi
done
