SRC=src/*.cpp
STDLIB_VER=-std=c++11

# -Wno-invalid-offsetof prevents STEAM_CALLBACK from giving out warnings
CPP_FLAGS=-Wno-invalid-offsetof -Wall

THIRD_PARTY=./third-party

OSX_OUT=luasteam.so
OSX_IPATHS=-I$(THIRD_PARTY)/include/
OSX_FLAGS=$(OSX_IPATHS) $(STDLIB_VER)

GNU_OUT=luasteam.so
GNU_IPATHS=-I/usr/include/luajit-2.1
GNU_FLAGS=$(GNU_IPATHS) $(STDLIB_VER) -lluajit-5.1

.PHONY: all osx linux32 linux64 windows32 windows64

all:
	@echo "choose platform: linux64 | linux32 | windows32 | windows64 | osx"

STEAM_LIB=sdk/redistributable_bin

osx:
	$(CXX) $(SRC) $(CPP_FLAGS) ${STEAM_LIB}/osx/libsteam_api.dylib ${THIRD_PARTY}/lib/libluajit-5.1.a -o $(OSX_OUT) -shared -fPIC $(OSX_FLAGS)

linux32:
	$(CXX) $(SRC) $(CPP_FLAGS) ${STEAM_LIB}/linux32/libsteam_api.so -m32 -o $(GNU_OUT) -shared -fPIC $(GNU_FLAGS)

linux64:
	$(CXX) $(SRC) $(CPP_FLAGS) ${STEAM_LIB}/linux64/libsteam_api.so -o $(GNU_OUT) -shared -fPIC $(GNU_FLAGS)

ifeq ($(OS),Windows_NT)
# Windows stuff
SHELL=cmd
WINDOWS_IPATHS=-I./cache/include
WINDOWS_OPT=-LD -EHsc -Feluasteam
WINDOWS_OPT_SERVER=-LD -EHsc -Feluasteamserver
VARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"

luajit.zip:
	curl -sL -o luajit.zip http://luajit.org/download/LuaJIT-2.0.5.zip

cache: luajit.zip
	@echo "Downloading LuaJIT source"
	unzip -qo luajit.zip
	mkdir cache\include
	cp LuaJIT-2.0.5/src/*.h cache/include

cache/win32_lua51.lib:
	@echo "Compiling LuaJIT 32 bits"
	if not exist cache $(MAKE) cache
	cd LuaJIT-2.0.5/src && call $(VARSALL) x86 && call msvcbuild.bat
	cp LuaJIT-2.0.5/src/lua51.lib cache/win32_lua51.lib

cache/win64_lua51.lib:
	@echo "Compiling LuaJIT 64 bits"
	if not exist cache $(MAKE) cache
	cd LuaJIT-2.0.5/src && call $(VARSALL) x64 && call msvcbuild.bat
	cp LuaJIT-2.0.5/src/lua51.lib cache/win64_lua51.lib

windows32:
	if not exist cache\win32_lua51.lib $(MAKE) cache/win32_lua51.lib
	call $(VARSALL) x86 && cl $(SRC) cache/win32_lua51.lib ${STEAM_LIB}/steam_api.lib $(WINDOWS_OPT) $(WINDOWS_IPATHS)

windows64:
	if not exist cache\win64_lua51.lib $(MAKE) cache/win64_lua51.lib
	call $(VARSALL) x64 && cl $(SRC) cache/win64_lua51.lib ${STEAM_LIB}/win64/steam_api64.lib $(WINDOWS_OPT) $(WINDOWS_IPATHS)
endif
