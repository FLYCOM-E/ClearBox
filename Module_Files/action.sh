#!/system/bin/sh
#此脚本来自ClearBox模块，用于提供一键优化清理（Root管理器端）按钮
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！"
    exit 1
if
######
home_dir=${0%/*}
"$home_dir/BashCore" ClearAll

exit 0
