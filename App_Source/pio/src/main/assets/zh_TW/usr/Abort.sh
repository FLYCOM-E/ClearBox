#!/system/bin/sh
source "$work_dir/settings.prop"
if [ "$clearbox_debug_mode" = 1 ]; then
    exec 2>>"$work_dir/LOG.log"
else
    exec 2>>/dev/null
fi
if [ "$clearbox_debug_mode" = 1 ]; then
    DebugTitle="調試模式已打開"
    DebugMode=1
fi
######
echo '
<?xml version="1.0" encoding="UTF-8" ?>
'
echo "
<items>
    <text>
        <slice bold=\"true\" align=\"center\" break=\"true\" size=\"20\">      </slice>
        <slice bold=\"true\" align=\"center\" break=\"true\" size=\"20\">      </slice>
        <slice bold=\"true\" align=\"center\" break=\"true\" color=\"#00D2C4\" size=\"62\">ClearBox</slice>
        <slice bold=\"true\" align=\"center\" break=\"true\" size=\"20\">$Version</slice>
        <slice bold=\"true\" align=\"center\" break=\"true\" size=\"20\">      </slice>
    </text>
</items>
"
echo "
<items>
    <text>
        <slice bold=\"true\" align=\"center\" break=\"true\" color=\"#6A71F6\" size=\"29\">$Str_1</slice>
        <slice bold=\"true\" align=\"center\" break=\"true\" color=\"#6A71F6\" size=\"29\">    $Str_2</slice>
        <slice bold=\"true\" align=\"center\" break=\"true\" size=\"20\">      </slice>
    </text>
</items>
"
echo '
<group>
    <text>
        <slice break="break" link="https://github.com/FLYCOM-E/ClearBox" size="20">GitHub</slice>
    </text>
</group>
'
echo '
<group>
    <text>
        <slice break="break" link="https://yhfx.jwznb.com/share?key=yigOTedUjh62&amp;ts=1747355950" size="20">模組內測 - YHChat</slice>
    </text>
</group>
'
echo '
<group>
    <text>
        <slice break="break" link="https://t.me/clearboxmodule" size="20">模組內測 - Telegram</slice>
    </text>
</group>
'
if [ "$DebugMode" = 1 ]; then
    echo "
    <items>
        <text>
            <slice align=\"center\" break=\"true\" color=\"#FFF9592F\" size=\"18\">$DebugTitle</slice>
        </text>
    </items>
    "
fi
