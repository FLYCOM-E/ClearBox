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
if [ -d "$1" ]; then
    DEF_DIR="$1"
fi
######

echo '
<?xml version="1.0" encoding="UTF-8" ?>
<root>
'

"$home_dir/$core" --ncdu "$DEF_DIR" | while IFS='|' read -r name dir size unit mode; do    
    if [ "$mode" == "F" ]; then
        echo "
        <action interruptible=\"false\" auto-off=\"false\" warning=\"$L_CLEAN $name? \">
            <title>📄 $name</title>
            <desc>$size $unit</desc>
                <set>
                rm -f \"$dir\"
                </set>
        </action>
        "
    elif [ "$mode" == "D" ]; then
        if [ -z "$(ls "$dir/")" ]; then
            echo "
            <items>
                <text>
                    <slice>📂 $name</slice>
                </text>
            </items>
            "
        else
            echo "
            <page
                config-sh=\"$0 '$dir'\"
                title=\"📂 $name\"
                desc=\"$size $unit\" />
            "
        fi
    fi
done
echo '
</root>
'
