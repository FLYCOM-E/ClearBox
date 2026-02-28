CFLAGS = -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wformat=2 -Wunused -Wcast-qual -Wstrict-prototypes -Werror -D_GNU_SOURCE -D_FORTIFY_SOURCE=2
LDFLAGS = -llog -s

ifeq ($(strip $(CC)),)
    $(error 编译器$CC变量设置为空！The compiler $CC variable is set to empty!)
endif
ifeq ($(strip $(M_LANG)),)
    $(error 语言$M_LANG变量设置为空！The language $M_LANG variable is set to empty!)
else
    ifeq ($(M_LANG), zh_CN)
        CFLAGS += -Dzh_CN
    else ifeq ($(M_LANG), en_US)
        CFLAGS += -Den_US
    else ifeq ($(M_LANG), zh_TW)
        CFLAGS += -Dzh_TW
    endif
endif

home_dir = Module_Files
daemon_bin_dir = Module_Files/Daemon

FUNCTIONS_C = src/INCLUDE/functions.c
BIN_C = src/BashCore.c \
		src/AppClean.c \
		src/CacheClean.c \
		src/Dexoat.c \
		src/FreeZer.c \
		src/StorageRuleClean.c \
		src/StorageClean.c \
		src/F2fs_GC.c \
		src/SetInstall.c \
		src/SetStorage.c \
		src/FileManager.c \

CORE_ELF = $(home_dir)/BashCore
DAEMON_ELF = $(daemon_bin_dir)/StopCached \
				$(daemon_bin_dir)/Timed

all: $(CORE_ELF) $(DAEMON_ELF)
.PHONY: all

$(CORE_ELF): $(BIN_C)
	$(CC) $(CFLAGS) $(BIN_C) $(FUNCTIONS_C) $(LDFLAGS) -o $(CORE_ELF)

$(daemon_bin_dir)/StopCached: src/Daemon/StopCached.c
	@mkdir -p $(daemon_bin_dir)
	$(CC) $(CFLAGS) src/Daemon/StopCached.c $(FUNCTIONS_C) $(LDFLAGS) -o $(daemon_bin_dir)/StopCached

$(daemon_bin_dir)/Timed: src/Daemon/Timed.c
	@mkdir -p $(daemon_bin_dir)
	$(CC) $(CFLAGS) src/Daemon/Timed.c $(FUNCTIONS_C) $(LDFLAGS) -o $(daemon_bin_dir)/Timed

clean: 
	@rm $(CORE_ELF)
	@rm $(DAEMON_ELF)
	@rm $(home_dir)/ClearBox.apk
	@rm -r $(daemon_bin_dir)
	@rm -r $(home_dir)/语言包

module_tar: 
	@cp APKS/ClearBox_$(M_LANG).apk $(home_dir)/ClearBox.apk
	@mkdir -p $(home_dir)/语言包 && cp LANG_Configs/$(M_LANG).conf $(home_dir)/语言包/Local.conf
	@cp $(home_dir)/module.prop ./module.prop.bak
	@echo "updateJson=https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/UpdateJson/$(M_LANG)/update_$(M_TARGET).json" >> $(home_dir)/module.prop
	@cd $(home_dir) && zip -r ../ClearBox_$(M_TARGET)_$(M_API)_$(M_LANG).zip *
	@mv ./module.prop.bak $(home_dir)/module.prop
