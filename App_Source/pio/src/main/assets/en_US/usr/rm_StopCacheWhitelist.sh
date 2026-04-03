#!/system/bin/sh
#此脚本用于将读取的软件从白名单移除
for C in "$Package"; do
    sed -i /"$C"/d "$work_dir/whitelist.prop"
    if [[ $? = 0 ]]; then
        echo " $C has been successfully removed from the whitelist!"
    else
        echo " $C removal failed or not in the whitelist!"
    fi
done
