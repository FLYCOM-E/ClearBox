#!/system/bin/env sh
set -a

SD_PATH=$({SDCARD_PATH})
EXECUTOR_PATH=$({EXECUTOR_PATH})
TMPDIR=$({TEMP_DIR})
HOME=$({START_DIR})
APP_USER_ID=$({APP_USER_ID})
SDK=$({ANDROID_SDK})
Package_name=$({PACKAGE_NAME})
Version_Name=$({PACKAGE_VERSION_NAME})
Version_code=$({PACKAGE_VERSION_CODE})
PREFIX=$({TOOLKIT})
Have_ROOT=$({ROOT_PERMISSION})
ANDROID_UID=$({ANDROID_UID})
DATA_DIR=${HOME%/${Package_name}*}
PATH0="$PATH"
ShellScript=$PREFIX
PeiZhi_File=~/Configuration_File
PATH="${ELF1_Path}:${ELF2_Path}:${ELF3_Path}:${PATH0}:${ELF4_Path}"
TMP=/data/local/tmp
######
lang_dir="LANG"
app_config_dir="AppCleanRules"
file_config_dir="FileConfigs"
timed_config_dir="TimedConfigs"
core="clearbox"

if [ ! -f "/data/adb/wipe_cache/PATH" ]; then
    echo "${1} » PATH Read Error！"
    exit 1
else
    source "/data/adb/wipe_cache/PATH"
fi
######
local_lang="$($home_dir/$core --get-lang)"
if ! source "$home_dir/$lang_dir/$local_lang.conf"; then
    source "$home_dir/$lang_dir/en_US.conf"
fi
Version=$(grep "version=" "$home_dir/module.prop" | cut -f2 -d "=")
Str_1=$(grep "str=" "$home_dir/module.prop" | cut -f2 -d '=' | cut -f1 -d "|")
Str_2=$(grep "str=" "$home_dir/module.prop" | cut -f2 -d "|")

set +a
######
[[ -f $Core ]] && . $Core
if [[ -f "$1" ]]; then
    cd "$ShellScript"
    . "$@" &
else
    echo "${1} Script missing!"
fi
wait
