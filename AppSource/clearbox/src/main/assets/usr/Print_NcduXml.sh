#!/system/bin/env sh
source "$work_dir/settings.prop"
if [ "$clearbox_debug_mode" = 1 ]; then
    exec 2>>"$work_dir/LOG.log"
else
    exec 2>>/dev/null
fi
######
DEFAULT_PATH="/storage/emulated/0"
######
DEF_DIR="$DEFAULT_PATH"
if [ -d "$clearbox_storage_scan_dirname" ]; then
    DEF_DIR="$clearbox_storage_scan_dirname"
fi
if [ -d "$1" ]; then
    DEF_DIR="$1"
fi
######

echo '
<?xml version="1.0" encoding="UTF-8" ?>
<root>
'

if [ -z "$(ls -A "$DEF_DIR/")" ]; then
    echo "
    <text>
        <slices>
            <title> </title>
            <slice size=\"30\" color=\"#FF3E00\">$L_VOID_DIR</slice>
        </slices>
    </text>
    "
    exit 0
fi

"$home_dir/$core" --ncdu "$DEF_DIR" | while IFS='|' read -r name dir size unit history mode; do
    if [ "$mode" == "F" ]; then
        echo "
        <action interruptible=\"false\" auto-off=\"false\" warning=\"$L_CLEAN $name? \">
            <title>📄 $name</title>
            <desc>$size $unit  $history</desc>
                <set>
                rm -f \"$dir\"
                </set>
        </action>
        "
    elif [ "$mode" == "D" ]; then
        echo "
        <page
            config-sh=\"$0 '$dir'\"
            title=\"📂 $name\"
            desc=\"$size $unit  $history\" />
        "
    fi
done
echo '
</root>
'
