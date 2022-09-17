#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include "Control_defines.h"
extern const char Host[];

extern const char getinfopatt[];
extern const char startstreampatt[];
extern const char sustainstreampatt[];

extern char startstream[];
extern char getinfo[];
extern char sustainstream[];

extern bool quit;
extern bool stream_stop;
// circular audio buffer
extern unsigned char *audio_buffer;
extern unsigned char *audio_end;

extern unsigned char *audio_read;
extern unsigned char *audio_write;

extern int * restart_stream;

char * temppagebuffer = (char*) malloc(MAXPAGESIZE);

void UpdateDisplay(struct button **buttons,int length, SDL_Renderer * renderer){
        struct button * curbutton;
        SDL_SetRenderDrawColor(renderer,255, 255, 255,255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer,0, 0, 0,255);
        for(int i = 0; i < NumofStructs; i++) {
                curbutton = buttons[i];
                if(curbutton->canchange) {
                        if(curbutton->ison) {
                                SDL_RenderCopy(renderer, curbutton->onimg, NULL, curbutton->rect);
                        }else{
                                SDL_RenderCopy(renderer, curbutton->offimg, NULL, curbutton->rect);
                        }
                }else{
                        if(curbutton->ison) { // Just for dumb programmers
                                SDL_RenderCopy(renderer, curbutton->onimg, NULL, curbutton->rect);
                        }else{
                                SDL_RenderCopy(renderer, curbutton->offimg, NULL, curbutton->rect);
                        }
                }
                SDL_RenderDrawRect(renderer,curbutton->rect);
        }

        SDL_RenderPresent(renderer);
}
void sendbutt(struct button * pressedbutton,CURL* curl){
        const char * selurl;
        if(pressedbutton->ison) {
                selurl = pressedbutton->onurl;
        } else if(!pressedbutton->ison) {
                selurl = pressedbutton->url;
        }
        snprintf(temppagebuffer,MAXPAGESIZE,pressedbutton->baseurl,selurl);
        //printf("Get %s\n",temppagebuffer);
        curl_easy_setopt (curl, CURLOPT_URL, temppagebuffer);
        CURLcode res = curl_easy_perform (curl);
        if (res != CURLE_OK)
        {
                printf ("curl_easy_perform() failed: %s\n",
                        curl_easy_strerror (res));
                exit (1);
        }
        if(pressedbutton->needsrestart) {
                curl_easy_setopt (curl, CURLOPT_URL, startstream);
                CURLcode res = curl_easy_perform (curl);
                if (res != CURLE_OK)
                {
                        printf ("curl_easy_perform() failed: %s\n",
                                curl_easy_strerror (res));
                        exit (1);
                }
        }
}
void buttonpresseddisplay(struct button *pressedbutton, SDL_Renderer * renderer,SDL_Event * events,Uint32 windowid,CURL* curl){
        if(pressedbutton->canchange) {
                if(pressedbutton->ison) {
                        SDL_RenderCopy(renderer, pressedbutton->offimg, NULL, pressedbutton->rect);
                        sendbutt(pressedbutton,curl);
                        pressedbutton->ison = false;
                } else if(!pressedbutton->ison) {
                        SDL_RenderCopy(renderer, pressedbutton->offimg, NULL, pressedbutton->rect);
                        sendbutt(pressedbutton,curl);
                        pressedbutton->ison = true;
                }
        }else{
                SDL_RenderCopy(renderer, pressedbutton->pressedimg, NULL, pressedbutton->rect);
                sendbutt(pressedbutton,curl);
                SDL_RenderPresent(renderer);
                while(1) {
                        SDL_WaitEvent(events);
                        if(events->type == SDL_MOUSEBUTTONUP) {
                                break;
                        } else if(events->window.event == SDL_WINDOWEVENT_LEAVE) {
                                break;
                        }
                }
                // SDL_RenderCopy(renderer, pressedbutton->offimg, NULL, pressedbutton->rect);
                // SDL_RenderPresent(renderer);
        }
}
static int control_menu (void * tempwin)
{
        SDL_Window * win = (SDL_Window *)tempwin;
        char baseurlpatt[] = "http://%s/cam.cgi?mode=camcmd&value=%s";
        char baseurl[MAXURLLENGTH];

        struct button zoom_in_slow;
        struct button zoom_in_fast;
        struct button zoom_stop;
        struct button zoom_out_slow;
        struct button zoom_out_fast;
        struct button change_mode;
        struct button record;
        struct button capture_image;
        SDL_Rect zoom_in_sButt;
        SDL_Rect zoom_in_fButt;
        SDL_Rect zoom_stop_Butt;
        SDL_Rect zoom_out_sButt;
        SDL_Rect zoom_out_fButt;
        SDL_Rect rec_Butt;
        SDL_Rect chng_modeButt;
        SDL_Rect cap_image_Butt;
        snprintf(baseurl, MAXURLLENGTH,baseurlpatt, camera,"%s");

        zoom_in_sButt.x = ButtonWidth;
        zoom_in_sButt.y = 0;
        zoom_in_sButt.w = ButtonWidth;
        zoom_in_sButt.h = ButtonHeight;
        zoom_in_fButt.x = 0;
        zoom_in_fButt.y = 0;
        zoom_in_fButt.w = ButtonWidth;
        zoom_in_fButt.h = ButtonHeight;
        zoom_stop_Butt.x = ButtonWidth*2;
        zoom_stop_Butt.y = 0;
        zoom_stop_Butt.w = ButtonWidth;
        zoom_stop_Butt.h = ButtonHeight;
        zoom_out_sButt.x = ButtonWidth*3;
        zoom_out_sButt.y = 0;
        zoom_out_sButt.w = ButtonWidth;
        zoom_out_sButt.h = ButtonHeight;
        zoom_out_fButt.x = ButtonWidth*4;
        zoom_out_fButt.y = 0;
        zoom_out_fButt.w = ButtonWidth;
        zoom_out_fButt.h = ButtonHeight;
        chng_modeButt.x = ButtonWidth;
        chng_modeButt.y = ButtonHeight;
        chng_modeButt.w = ButtonWidth;
        chng_modeButt.h = ButtonHeight;
        rec_Butt.x = ButtonWidth*2;
        rec_Butt.y = ButtonHeight;
        rec_Butt.w = ButtonWidth;
        rec_Butt.h = ButtonHeight;
        cap_image_Butt.x = ButtonWidth*3;
        cap_image_Butt.y = ButtonHeight;
        cap_image_Butt.w = ButtonWidth;
        cap_image_Butt.h = ButtonHeight;

        zoom_in_slow.rect = &zoom_in_sButt;
        zoom_in_slow.url = "tele-normal";
        zoom_in_slow.off_icon = "icons/slow_tele.png";
        zoom_in_slow.pressed_icon = "icons/slow_tele_pressed.png";
        zoom_in_slow.baseurl = baseurl;
        zoom_in_slow.canchange = false;

        zoom_in_fast.rect = &zoom_in_fButt;
        zoom_in_fast.url = "tele-fast";
        zoom_in_fast.off_icon = "icons/fast_tele.png";
        zoom_in_fast.pressed_icon = "icons/fast_tele_pressed.png";
        zoom_in_fast.baseurl = baseurl;
        zoom_in_fast.canchange = false;

        zoom_stop.rect = &zoom_stop_Butt;
        zoom_stop.url = "zoomstop";
        zoom_stop.off_icon = "icons/stop_zoom.png";
        zoom_stop.pressed_icon = "icons/stop_zoom_pressed.png";
        zoom_stop.baseurl = baseurl;
        zoom_stop.canchange = false;

        zoom_out_slow.rect = &zoom_out_sButt;
        zoom_out_slow.url = "wide-normal";
        zoom_out_slow.off_icon = "icons/slow_wide.png";
        zoom_out_slow.pressed_icon = "icons/slow_wide_pressed.png";
        zoom_out_slow.baseurl = baseurl;
        zoom_out_slow.canchange = false;

        zoom_out_fast.rect = &zoom_out_fButt;
        zoom_out_fast.url = "wide-fast";
        zoom_out_fast.off_icon = "icons/fast_wide.png";
        zoom_out_fast.pressed_icon = "icons/fast_wide_pressed.png";
        zoom_out_fast.baseurl = baseurl;
        zoom_out_fast.canchange = false;

        change_mode.rect = &chng_modeButt;
        change_mode.url = "pictmode";
        change_mode.onurl = "recmode";
        change_mode.off_icon = "icons/videorec.png";
        change_mode.on_icon = "icons/photomode.png";
        change_mode.baseurl = baseurl;
        change_mode.canchange = true;
        change_mode.needsrestart = true;

        capture_image.rect = &cap_image_Butt;
        capture_image.url = "capture";
        capture_image.off_icon = "icons/take_pic.png";
        capture_image.pressed_icon = "icons/take_pic_pressed.png";
        capture_image.baseurl = baseurl;
        capture_image.canchange = false;

        record.rect = &rec_Butt;
        record.url = "video_recstart";
        record.onurl = "video_recstop";
        record.off_icon = "icons/record.png";
        record.on_icon = "icons/recording.png";
        record.baseurl = baseurl;
        record.canchange = true;
        struct button * structlist[NumofStructs] = {&zoom_in_slow,&zoom_in_fast,&zoom_stop,&zoom_out_slow,&zoom_out_fast,&change_mode,&record,&capture_image};

        const char sustainstreampatt[] = "http://%s/cam.cgi?mode=getstate";

        char sustainstream[MAXURLLENGTH];

        Uint32 controlwindowID;

        SDL_Event eventcontrol;
        SDL_Texture *img = NULL;
        char * tempcontrol = (char *) malloc (MAXURLLENGTH);

        snprintf(sustainstream, MAXURLLENGTH, sustainstreampatt, camera);

        if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
        {
                printf ("error initializing SDL: %s\n", SDL_GetError ());
        }
        controlwindowID = SDL_GetWindowID(win);
        SDL_Renderer *renderer = SDL_CreateRenderer (win, -1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == NULL)
        {
                printf ("SDL_Renderer failed: %s\n", SDL_GetError ());
                exit (1);
        }
        for(int i = 0; i < NumofStructs; i++) {
                if(structlist[i]->canchange) {
                        structlist[i]->onimg = IMG_LoadTexture(renderer,structlist[i]->on_icon);
                        if(structlist[i]->onimg == NULL) {
                                printf ("SDL_LoadTexture on icon : %s failed: %s\n",structlist[i]->on_icon,SDL_GetError());
                                exit (1);
                        }
                }else{
                        structlist[i]->pressedimg = IMG_LoadTexture(renderer,structlist[i]->pressed_icon);
                        if(structlist[i]->pressedimg == NULL) {
                                printf ("SDL_LoadTexture on icon : %s failed: %s\n",structlist[i]->on_icon,SDL_GetError());
                                exit (1);
                        }
                }
                structlist[i]->offimg = IMG_LoadTexture(renderer,structlist[i]->off_icon);
                if(structlist[i]->offimg == NULL) {
                        printf ("SDL_LoadTexture on icon : %s failed: %s\n",structlist[i]->off_icon,SDL_GetError());
                        exit (1);
                }
        }
        //SDL_RenderDrawRects(renderer,allrects,NumberofButts);
        UpdateDisplay(structlist,NumofStructs,renderer);
        CURL *curl = curl_easy_init ();
        //curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, curl_write);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, CURLTimeout);
        if (curl)
        {
                curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
                curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L);
        }
        struct button *curbutton;
        while(!quit) {
                SDL_PollEvent(&eventcontrol);
                if(eventcontrol.type == SDL_WINDOWEVENT && eventcontrol.window.event == SDL_WINDOWEVENT_CLOSE) {
                        puts("Stopping Control Window");
                        quit = true;
                        break;
                } else if(eventcontrol.type == SDL_MOUSEBUTTONDOWN && eventcontrol.motion.windowID == controlwindowID) {
                        for(int i = 0; i < NumofStructs; i++) {
                                curbutton = structlist[i];
                                if (curbutton->rect->x < eventcontrol.motion.x && eventcontrol.motion.x < curbutton->rect->x+curbutton->rect->w && curbutton->rect->y < eventcontrol.motion.y && eventcontrol.motion.y < curbutton->rect->y+curbutton->rect->h) {
                                        //sendbutt();
                                        buttonpresseddisplay(curbutton,renderer,&eventcontrol,controlwindowID,curl);
                                        UpdateDisplay(structlist,NumofStructs,renderer);
                                        break;
                                }
                        }
                }
        }

        SDL_DestroyRenderer (renderer);
        SDL_DestroyWindow (win);
        SDL_Quit();
        return 0;
}
