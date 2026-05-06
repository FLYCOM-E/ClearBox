CFLAGS = -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wformat=2 -Wunused -Wcast-qual -Wstrict-prototypes -Werror -Wno-format-nonliteral -Wno-format-security -D_GNU_SOURCE -D_FORTIFY_SOURCE=2
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

UTILS_C = src/INCLUDE/Utils.c
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
		src/Daemon/StopCached.c \
		src/Daemon/Timed.c

CORE_ELF = $(home_dir)/BashCore

all: $(CORE_ELF)
.PHONY: all

$(CORE_ELF): $(BIN_C)
	$(CC) $(CFLAGS) $(BIN_C) $(UTILS_C) $(LDFLAGS) -o $(CORE_ELF)

clean: 
	@rm $(CORE_ELF)
	@rm -f $(home_dir)/ClearBox.apk
	@rm -r $(home_dir)/LANG
	@rm -r $(home_dir)/AppConfigs
	@rm -r $(home_dir)/FileConfigs

module_tar: 
	@find App_Source -name "*.apk" -exec cp {} $(home_dir)/ClearBox.apk \;
	@mkdir -p $(home_dir)/LANG && cp LANG_Configs/$(M_LANG).conf $(home_dir)/LANG/Local.conf
	@cp -r AppConfigs $(home_dir)/AppConfigs
	@cp -r FileConfigs $(home_dir)/FileConfigs
	@cp $(home_dir)/module.prop ./module.prop.bak
	@echo "updateJson=https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/UpdateJson/$(M_LANG)/update_$(M_TARGET).json" >> $(home_dir)/module.prop
	@cd $(home_dir) && zip -r ../ClearBox_$(M_TARGET)_$(M_API)_$(M_LANG).zip *
	@mv ./module.prop.bak $(home_dir)/module.prop
