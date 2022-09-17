#define NumofStructs NumberofButts

struct button {
  const char * off_icon;
  const char * on_icon;
  const char * pressed_icon;
  const char * url;
  const char * onurl;
  char * baseurl;
  SDL_Texture * pressedimg;
  SDL_Texture * offimg;
  SDL_Texture * onimg;
  SDL_Rect * rect;
  bool canchange = false;
  bool ison = false;
  bool needsrestart = false;
};
