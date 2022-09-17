#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <turbojpeg.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <ao/ao.h>
#include "defines.h"
#include "Audio.h"
#include "Control.c"

pthread_t thread_id;

// Just for removing the print of libcurl
size_t
write_data (void *buffer, size_t size, size_t nmemb, void *userp)
{
        return size * nmemb;
}


void
usage (char *progname)
{
        printf ("Usage : %s <IP>\n", progname);
        puts ("Required:");
        puts ("  IP           : The IP of the camera");
        exit (1);
}


int
startdisplay (CURL * curl,char * sustainstream,char * getinfo,char * startstream,pthread_t thread_id)
{
        tjhandle jpegdec = tjInitDecompress ();

        size_t jpegsize = 0;

        uint8_t *jpegbuf = (uint8_t *) malloc (WIDTH * HEIGHT * 2);

        SDL_Event event;
        SDL_Rect srcrect;

        srcrect.x = 0;
        srcrect.y = 0;
        srcrect.w = WIDTH;
        srcrect.h = HEIGHT;

        int UDPSocket;
        int returnbytes;
        int counter = 0;

        int16_t pcm[512];

        int8_t mulaw[512];

        uint8_t packet_id;

        unsigned int addr_len;
        unsigned short length;

        unsigned char *audio_buf;
        unsigned char *video_buf;

        unsigned char rgbbuf[WIDTH * HEIGHT * 3];
        unsigned char buf[MAXBUFLEN];

        struct sockaddr_in server;
        struct hostent *host;

        if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
        {
                printf ("error initializing SDL: %s\n", SDL_GetError ());
                exit(1);
        }

        SDL_Window *win = SDL_CreateWindow ("Video Player",
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);

        SDL_Renderer *renderer = SDL_CreateRenderer (win, -1,
                                                     SDL_RENDERER_ACCELERATED |
                                                     SDL_RENDERER_PRESENTVSYNC);

        SDL_Surface * iconimg = IMG_Load(windowicon);
        if(iconimg == NULL){
          printf ("error loading app icon: %s\n", SDL_GetError ());
          exit(1);
        }
        SDL_SetWindowIcon(win,iconimg);
        if (renderer == NULL)
        {
                printf ("SDL_Renderer failed: %s\n", SDL_GetError ());
                exit (1);
        }
        SDL_RenderSetLogicalSize (renderer, WIDTH, HEIGHT);
        SDL_Texture *texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGB24,
                                                  SDL_TEXTUREACCESS_TARGET, WIDTH,
                                                  HEIGHT);
        SDL_Window *controlwin = SDL_CreateWindow ("Control Menu",
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            WIDTHControl, HEIGHTControl, 0);
        SDL_Thread *control_thread;
        control_thread = SDL_CreateThread(control_menu,"Control Menu",controlwin);

        ao_device *device;
        ao_sample_format format;
        int default_driver;
        audio_buffer = (unsigned char *) malloc (AUDIO_BUFFER_SIZE);
        audio_end = audio_buffer + AUDIO_BUFFER_SIZE;
        audio_read = audio_buffer;
        audio_write = audio_buffer;

        format.bits = 16;
        format.channels = 1;
        format.rate = 8000;
        format.byte_format = AO_FMT_LITTLE;

        curl_easy_setopt (curl, CURLOPT_URL, getinfo);
        curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L);
        CURLcode res = curl_easy_perform (curl);
        if (res != CURLE_OK)
        {
                printf ("curl_easy_perform() failed: %s\n", curl_easy_strerror (res));
                exit (1);
        }
        // Set Recmode
        curl_easy_setopt (curl, CURLOPT_URL, setrecmode);
        res = curl_easy_perform (curl);
        if (res != CURLE_OK)
        {
                printf ("curl_easy_perform() failed: %s\n", curl_easy_strerror (res));
                exit (1);
        }
        sleep (2);
        if ((host = gethostbyname (Host)) == NULL)
        {   /* get the host info */
                herror ("gethostbyname");
                exit (1);
        }
        if ((UDPSocket = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
        {
                perror ("socket");
                exit (1);
        }

        server.sin_family = AF_INET;
        server.sin_port = htons (UDPPORT);
        //server.sin_addr = *((struct in_addr *)host->h_addr);
        server.sin_addr.s_addr = inet_addr (Host);

        bzero (&(server.sin_zero), 8);

        addr_len = sizeof (server);

        curl_easy_setopt (curl, CURLOPT_URL, startstream);
        res = curl_easy_perform (curl);

        if (res != CURLE_OK)
        {
                printf ("curl_easy_perform() failed: %s\n", curl_easy_strerror (res));
                exit (1);
        }
        if (bind (UDPSocket, (struct sockaddr *) &server, addr_len) == -1)
        {
                perror ("Bind");
                exit (1);
        }
        //puts("");
        bool firstrun = true;
        while (!quit)
        {
                if ((returnbytes =
                             recvfrom (UDPSocket, buf, MAXBUFLEN, MSG_WAITALL,
                                       (struct sockaddr *) &server, &addr_len)) == -1)
                {
                        perror ("recvfrom");
                        exit (1);
                }
                //puts("Received Data");
                if (counter == 200)
                {
                        curl_easy_setopt (curl, CURLOPT_URL, sustainstream);
                        res = curl_easy_perform (curl);
                        counter = 0;
                        if (res != CURLE_OK)
                        {
                                printf ("curl_easy_perform() failed: %s\n",
                                        curl_easy_strerror (res));
                                exit (1);
                        }
                }
                length = buf[0] << 8 | buf[1];
                packet_id = buf[16];
                if (packet_id == AUDIO_ID)
                {
                        audio_buf = buf + HEADER_OFFSET;
                        decodemulaw ((signed char *) audio_buf, pcm,
                                     returnbytes - HEADER_OFFSET);
                        write_to_audio_buffer ((unsigned char *) pcm,
                                               (returnbytes - HEADER_OFFSET) * 2);
                        if (firstrun)
                        {
                                pthread_create (&thread_id, NULL, play_samples_in_thread, NULL);
                                firstrun = false;
                        }
                }
                else if (packet_id == JPEG_ID)
                {
                        video_buf = buf + HEADER_OFFSET;
                        if (tjDecompress2
                                    (jpegdec, video_buf, returnbytes - HEADER_OFFSET, rgbbuf,
                                    WIDTH, WIDTH * 3, HEIGHT, TJPF_RGB, TJFLAG_FASTDCT) == -1)
                        {
                                printf
                                        ("Jpeg Decompression failed because : %s\nContinuing Anyways\n",
                                        tjGetErrorStr ());
                        }
                        switch (event.type)
                        {
                        case SDL_QUIT:
                                quit = true;
                                break;
                        }
                        SDL_UpdateTexture (texture, &srcrect, rgbbuf, WIDTH * 3);
                        SDL_RenderCopy (renderer, texture, NULL, NULL);
                        SDL_RenderPresent (renderer);
                }
                else
                {
                        printf ("Unrecognized Packet ID");
                        exit (1);
                }
                SDL_PollEvent (&event);
                switch (event.type)
                {
                case SDL_QUIT:
                        quit = true;
                        break;
                }
                counter++;
        }
        puts("Stopping Main Window");
        SDL_DestroyTexture (texture);
        SDL_DestroyRenderer (renderer);
        SDL_DestroyWindow (win);
        tjDestroy (jpegdec);
        close (UDPSocket);
        return 0;
}
int
main (int argc, char *argv[])
{
        if (argc < 2)
        {
                usage (argv[0]);
        }


        camera = argv[1];




        snprintf (sustainstream, MAXURLLENGTH, sustainstreampatt, camera);
        snprintf (getinfo, MAXURLLENGTH, getinfopatt, camera);
        snprintf (setrecmode, MAXURLLENGTH, setrecmodepatt, camera);
        snprintf (startstream, MAXURLLENGTH, startstreampatt, camera, UDPPORT);
        CURL *curl = curl_easy_init ();
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt (curl, CURLOPT_TIMEOUT, CURLTimeout);
        if (!curl)
        {
                puts ("Curl Init failed");
                exit (1);

        }

        startdisplay(curl,sustainstream,getinfo,startstream,thread_id);
        pthread_kill (thread_id, SIGKILL);
        SDL_Quit();
        return 0;
}
