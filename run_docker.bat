docker build -t staubhold/luasteam:latest .
docker rm luasteam
docker run --name luasteam staubhold/luasteam:latest
docker container cp luasteam:/build/luasteam.so build/