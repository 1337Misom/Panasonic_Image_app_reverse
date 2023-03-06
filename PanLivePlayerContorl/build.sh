GREEN='\033[0;32m'
LIGHTGREEN='\033[1;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

echo "${LIGHTGREEN}Building${NC} PanLivePlayerControl ${BLUE}(g++ PanLivePlayerControl.c -lSDL2 -lSDL2_image -lcurl -pthread -lturbojpeg -lao -Iincludes/ -o PanLivePlayerControl)${NC}"
if g++ PanLivePlayerControl.c -lSDL2 -lSDL2_image -lcurl -pthread -lturbojpeg -lao -Iincludes/ -o PanLivePlayerControl ; then
  echo "${GREEN}Built PanLivePlayerControl Successfully${NC}"
else
  echo "${RED}Failed To Build ${GREEN}PanLivePlayerControl ${BLUE}Tipp: Check that you have c++/g++ SDL2 SDL2_image libcurl and libturbojpeg installed.${NC}"
fi
