name = stylus-block-input

rwildcard = $(wildcard $(addsuffix $2, $1)) $(foreach d,$(wildcard $(addsuffix *, $1)),$(call rwildcard,$d/,$2))
winpath = $(subst /,\,$1)
ifeq ($(OS),Windows_NT)
	mkdir = if not exist $1 mkdir $1
else
	mkdir = mkdir -p $1
endif

build_dir = build

binary_name = $(build_dir)/$(name)
binary = $(binary_name).exe
dll = $(binary_name).dll

gcc_args = -lgdi32 -static-libgcc -static-libstdc++ \
	-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive \
	-Isrc/shared/
ifneq ($(MAKECMDGOALS),dev)
	gcc_args += -mwindows
endif

src_files = $(call rwildcard,src/$1/,*.cpp)
hook_files := $(call src_files,hook)
main_files := $(call src_files,main)

all: hook main

build_dir:
	$(call mkdir,$(build_dir))

hook: build_dir $(hook_files)
	g++ \
		-shared -Wl,--subsystem,windows \
		-o $(dll) $(hook_files) \
		$(gcc_args)

main: build_dir $(main_files)
	g++ \
		-o $(binary_name) $(main_files) \
		$(gcc_args)

run:
	./$(binary) || $(call winpath,$(binary))

dll-dump:
	objdump -p $(binary) | (grep "DLL Name:" || findstr "DLL Name:")

dev: all run

watch:
	nodemon --watch ./src/** --ext cpp,hpp,c,h --exec make dev

clean:
	rm -f $(build_dir) || rmdir /s /q $(build_dir)
