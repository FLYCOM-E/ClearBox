CFLAGS = -Os -flto -fPIE -Werror -D_GNU_SOURCE
CFLAGS += -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wformat=2 -Wunused -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wstrict-overflow=3 -Wnull-dereference
CFLAGS += -fstack-protector-strong -D_FORTIFY_SOURCE=3
CFLAGS += -Wno-format-nonliteral -Wno-format-security
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections -Wl,-z,relro,-z,now -Wl,-z,defs -Wl,--as-needed -pie -flto -fuse-ld=lld -llog -s
SQLITE3_CFLAGS = $(CFLAGS) -Wno-error \
                  -Wno-unused-variable \
                  -Wno-cast-qual \
                  -Wno-sign-conversion \
                  -Wno-implicit-float-conversion \
                  -Wno-implicit-int-float-conversion \
                  -Wno-incompatible-pointer-types-discards-qualifiers \
                  -Wno-conversion \
                  -Wno-missing-field-initializers \
                  -Wno-unused-parameter \
                  -Wno-unused-but-set-variable \
                  -Wno-unused-function \
                  -Wno-undefined-internal \
                  -Wno-unused-variable \
                  -Wno-parentheses \
                  -Wno-c11-extensions \
                  -Wno-gnu \
                  -DSQLITE_THREADSAFE=0 \
                  -DSQLITE_OMIT_LOAD_EXTENSION \
                  -DSQLITE_OMIT_DEPRECATED \
                  -DSQLITE_OMIT_SHARED_CACHE

module_dir = ModuleFiles
app_dir = AppSource
tui_dir = TuiSource

UTILS_C = src/INCLUDE/utils.c \
			src/INCLUDE/s_signal.c \
			src/INCLUDE/help.c \
			src/INCLUDE/set_language.c
SQLITE3_C = src/SQlite3/sqlite3.c
BIN_C = src/main.c \
		src/app_cust_rule_clean.c \
		src/app_cache_clean.c \
		src/dexoat.c \
		src/freezer_open.c \
		src/cust_rule_clean.c \
		src/storage_clean.c \
		src/set_install.c \
		src/set_storage.c \
		src/file_manager.c \
		src/bmount.c \
		src/Daemon/stop_cache_daemon.c \
		src/Daemon/disk_gc.c \
		src/Daemon/time_daemon.c \
		src/ncdu.c

UTILS_OBJ = $(UTILS_C:.c=.o)
SQLITE3_OBJ = $(SQLITE3_C:.c=.o)
BIN_OBJ = $(BIN_C:.c=.o)
ALL_OBJ = $(UTILS_OBJ) $(SQLITE3_OBJ) $(BIN_OBJ)

CORE_ELF = $(module_dir)/clearbox

elf: $(CORE_ELF)
.PHONY: elf

$(SQLITE3_OBJ): $(SQLITE3_C)
	@echo "  CC \t\t $@"
	@$(CC) -c $< -o $@ $(SQLITE3_CFLAGS)
%.o: %.c
	@echo "  CC \t\t $@"
	@$(CC) -c $< -o $@ $(CFLAGS)

$(CORE_ELF): $(ALL_OBJ)
	@echo "  LD \t\t $@"
	@$(CC) $^ $(LDFLAGS) -o $(CORE_ELF)

clean: 
	@echo "  CLEAN \t $@"
	@rm $(CORE_ELF)
	@rm -f $(ALL_OBJ)
	@rm -f $(module_dir)/ClearBox.apk
	@mv ./module.prop.bak $(module_dir)/module.prop
	@mv ./ClearBox.bak $(module_dir)/system/bin/ClearBox
	@rm -r $(module_dir)/LANG
	@rm -r $(module_dir)/AppConfigs
	@rm -r $(module_dir)/FileConfigs

module_tar: 
	@echo "  ZIP \t\t $@"
	@find $(app_dir) -name "*.apk" -exec cp {} $(module_dir)/ClearBox.apk \;
	
	@cp $(module_dir)/system/bin/ClearBox ./ClearBox.bak
	@cp $(tui_dir)/Main.bash $(module_dir)/system/bin/ClearBox
	
	@cp $(module_dir)/module.prop ./module.prop.bak
	@echo "updateJson=https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/UpdateJson/@LANG/update_$(TARGET_ABI).json" >> $(module_dir)/module.prop
	
	@cp -r LangConfigs $(module_dir)/LANG
	@cp -r AppConfigs $(module_dir)/AppConfigs
	@cp -r FileConfigs $(module_dir)/FileConfigs
	
	@cd $(module_dir) && zip -rq ../ClearBox_$(TARGET)_$(TARGET_API).zip *
	@echo "  OUTPUT \t ClearBox_$(TARGET)_$(TARGET_API).zip"
