GREEN='\033[0;32m'
LIGHTGREEN='\033[1;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'


echo "${LIGHTGREEN}Deleting${NC} Files in bin"
rm -rf bin/*

echo "${LIGHTGREEN}Building${NC} PanasonicToV4l2 ${BLUE}(g++ ./PanasonicToV4l2/PanasonicToV4l2.c -lSDL2 -lcurl -lturbojpeg  -Iincludes/ -o bin/PanLivePlayerSDL)${NC}"
if g++ ./PanasonicToV4l2/PanasonicToV4l2.c -lcurl -lv4l2wrapper  -Iincludes/ -o bin/PanasonicToV4l2 ; then
  echo "${GREEN}Built PanasonicToV4l2 Successfully${NC}"
else
  echo "${RED}Failed To Build ${GREEN}PanasonicToV4l2 ${BLUE}Tipp: Check that you have c++ SDL2 libcurl and libv4l2cpp[https://github.com/mpromonet/libv4l2cpp] installed.${NC}"
fi
