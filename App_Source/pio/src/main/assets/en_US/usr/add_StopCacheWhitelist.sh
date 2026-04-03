#!/system/bin/sh
#此脚本用于将读取的包名添加进白名单
for i in "$Package"; do
    echo "$i" >> "$work_dir/whitelist.prop"
    if [[ $? = 0 ]]; then
        echo " $i has been successfully added to the whitelist!"
    fi
done
