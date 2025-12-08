#!/system/bin/sh
#此脚本来自ClearBox模块，用于提供一键优化清理（Root管理器端）按钮
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！"
    exit 1
fi
if [ ! -f "/data/adb/wipe_cache/PATH" ]; then
    echo " » Error：PATH读取错误！"
    exit 1
else
    source "/data/adb/wipe_cache/PATH"
fi
######
"$home_dir/BashCore" ClearAll

exit 0
