#!/system/bin/env sh
#此脚本用于将读取的软件从白名单移除
for pk in $Package; do
    sed -i /"$pk"/d "$work_dir/ClearWhitelist.prop"
    if [[ $? = 0 ]]; then
        echo " » $pk успешно удалён из белого списка!"
    else
        echo " » Не удалось удалить $pk или его нет в белом списке!"
    fi
done
