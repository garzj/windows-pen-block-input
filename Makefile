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

cpp_files = $(call rwildcard,src/,*.cpp)

gcc_args = -lgdi32 -static-libgcc -static-libstdc++ \
	-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
ifneq ($(MAKECMDGOALS),dev)
	gcc_args += -mwindows
endif

all: build

build: build_dir $(cpp_files)
	g++ \
		-o $(binary_name) $(cpp_files) \
		$(gcc_args)

run:
	$(call winpath,$(binary))

dev: all run

watch:
	nodemon --watch ./src/** --ext cpp,hpp,c,h --exec make dev

build_dir:
	$(call mkdir,$(build_dir))

dll-dump:
	objdump -p $(binary) | findstr "DLL Name:"

clean:
	rmdir /s /q $(build_dir)
