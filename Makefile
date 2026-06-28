CFLAGS = -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wformat=2 -Wunused -Wcast-qual -Wstrict-prototypes -Werror -Wno-format-nonliteral -Wno-format-security -fstack-protector-strong -D_GNU_SOURCE -D_FORTIFY_SOURCE=2
LDFLAGS = -llog -s

ifeq ($(strip $(CC)),)
    $(error 编译器$CC变量设置为空！The compiler $CC variable is set to empty!)
endif

module_dir = ModuleFiles
app_dir = AppSource
tui_dir = TuiSource

UTILS_C = src/INCLUDE/Utils.c \
			src/INCLUDE/Signal.c \
			src/INCLUDE/Help.c \
			src/INCLUDE/Lang.c
BIN_C = src/main.c \
		src/AppClean.c \
		src/CacheClean.c \
		src/Dexoat.c \
		src/FreeZer.c \
		src/StorageRuleClean.c \
		src/StorageClean.c \
		src/SetInstall.c \
		src/SetStorage.c \
		src/FileManager.c \
		src/Bmount.c \
		src/Daemon/StopCached.c \
		src/Daemon/F2fsGcd.c \
		src/Daemon/Timed.c

CORE_ELF = $(module_dir)/clearbox

all: $(CORE_ELF)
.PHONY: all

$(CORE_ELF): $(BIN_C)
	$(CC) $(CFLAGS) $(BIN_C) $(UTILS_C) $(LDFLAGS) -o $(CORE_ELF)

clean: 
	@rm $(CORE_ELF)
	@rm -f $(module_dir)/ClearBox.apk
	@rm -r $(module_dir)/LANG
	@rm -r $(module_dir)/AppConfigs
	@rm -r $(module_dir)/FileConfigs

module_tar: 
	@find $(app_dir) -name "*.apk" -exec cp {} $(module_dir)/ClearBox.apk \;
	@cp $(tui_dir)/Main.bash $(module_dir)/system/bin/ClearBox
	@cp -r LangConfigs $(module_dir)/LANG
	@cp -r AppConfigs $(module_dir)/AppConfigs
	@cp -r FileConfigs $(module_dir)/FileConfigs
	@cp $(module_dir)/module.prop ./module.prop.bak
	@echo "updateJson=https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/UpdateJson/@LANG/update_$(TARGET).json" >> $(module_dir)/module.prop
	@cd $(module_dir) && zip -r ../ClearBox_$(TARGET)_$(TARGET_API).zip *
	@mv ./module.prop.bak $(module_dir)/module.prop
