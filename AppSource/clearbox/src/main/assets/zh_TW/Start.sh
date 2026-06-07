export SD_PATH=$({SDCARD_PATH})
export EXECUTOR_PATH=$({EXECUTOR_PATH})
export TMPDIR=$({TEMP_DIR})
export HOME=$({START_DIR})
export APP_USER_ID=$({APP_USER_ID})
export SDK=$({ANDROID_SDK})
export Package_name=$({PACKAGE_NAME})
export Version_Name=$({PACKAGE_VERSION_NAME})
export Version_code=$({PACKAGE_VERSION_CODE})
export PREFIX=$({TOOLKIT})
export Have_ROOT=$({ROOT_PERMISSION})
export ANDROID_UID=$({ANDROID_UID})
export DATA_DIR=${HOME%/${Package_name}*}
export PATH0="$PATH"
export ShellScript=$PREFIX
export PeiZhi_File=~/Configuration_File
export PATH="${ELF1_Path}:${ELF2_Path}:${ELF3_Path}:${PATH0}:${ELF4_Path}"
export TMP=/data/local/tmp
######
if [ ! -f "/data/adb/wipe_cache/PATH" ]; then
    echo "${1} » PATH Read Error！"
    exit 1
else
    source "/data/adb/wipe_cache/PATH"
fi
export home_dir="$home_dir"
export work_dir="$work_dir"
export bin_dir="$bin_dir"
export Version=$(grep "version=" "$home_dir/module.prop" | cut -f2 -d "=")
export Str_1=$(grep "str=" "$home_dir/module.prop" | cut -f2 -d '=' | cut -f1 -d "|")
export Str_2=$(grep "str=" "$home_dir/module.prop" | cut -f2 -d "|")
export app_config_dir="AppCleanRules"
export file_config_dir="FileConfigs"
export timed_config_dir="TimedConfig"
export core="BashCore"
######
[[ -f $Core ]] && . $Core
if [[ -f "$1" ]]; then
    cd "$ShellScript"
    . "$@" &
else
    echo "${1} 腳本缺失！"
fi
wait
