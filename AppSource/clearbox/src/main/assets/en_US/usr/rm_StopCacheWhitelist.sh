#!/system/bin/env sh
#此脚本用于将读取的软件从白名单移除
for pk in $Package; do
    sed -i /"$pk"/d "$work_dir/whitelist.prop"
    if [[ $? = 0 ]]; then
        echo " » $pk has been successfully removed from the whitelist!"
    else
        echo " » $pk removal failed or not in the whitelist!"
    fi
done
