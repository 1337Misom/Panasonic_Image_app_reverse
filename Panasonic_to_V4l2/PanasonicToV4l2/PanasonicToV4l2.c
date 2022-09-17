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
#include <libv4l2cpp/V4l2Capture.h>
#include <libv4l2cpp/V4l2MmapDevice.h>
#include <libv4l2cpp/V4l2ReadWriteDevice.h>
#include <libv4l2cpp/V4l2Device.h>
#include <libv4l2cpp/V4l2Output.h>

#define UDPPORT 49700
#define MAXBUFLEN 1024*32
#define MAXURLLENGTH 125
#define AUDIO_ID 0x61
#define JPEG_ID 0x21
#define HEADER_OFFSET 54

char Host[] = "0.0.0.0";

const char getinfopatt[] = "http://%s/cam.cgi?mode=getinfo&type=capability";
const char startstreampatt[] = "http://%s/cam.cgi?mode=startstream&value=%d";
const char sustainstreampatt[] = "http://%s/cam.cgi?mode=getstate";

// Just for removing the print of libcurl
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp){
   return size * nmemb;
}

void usage(char* progname){
  printf("Usage : %s <IP> <Video Device>\n",progname);
  puts("Required:");
  puts("  IP           : The IP of the camera");
  puts("  Video Device : The Virtual Video Device (e.g '/dev/video1')");
  exit(1);
}

int main(int argc, char *argv[]){
  if(argc < 3){
    usage(argv[0]);
  }
  int UDPSocket;
  int returnbytes;
  int counter = 0;

  uint8_t packet_id;

  unsigned int addr_len;
  unsigned short length;

  int width = 640;
  int height = 360;
  int fps = 25;

  unsigned char *audio_buf;
  unsigned char *video_buf;

  unsigned char buf[MAXBUFLEN];

  char startstream[MAXURLLENGTH];
  char getinfo[MAXURLLENGTH];
  char sustainstream[MAXURLLENGTH];

  char *videodevice = argv[2];
  char *camera = argv[1];

  struct sockaddr_in server;
  struct hostent *host;

  V4L2DeviceParameters param(videodevice, V4L2_PIX_FMT_MJPEG, width, height, fps, IOTYPE_MMAP, true);
  V4l2Output* videoOutput = V4l2Output::create(param);

  snprintf(sustainstream,MAXURLLENGTH,sustainstreampatt,camera);
  snprintf(getinfo, MAXURLLENGTH,getinfopatt,camera);
  snprintf(startstream, MAXURLLENGTH,startstreampatt,camera,UDPPORT);

  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, getinfo);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L);
    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK){
      printf("curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
      exit(1);
    }
    sleep(2);
    if ((host=gethostbyname(Host)) == NULL) {  /* get the host info */
      herror("gethostbyname");
      exit(1);
    }

    if ((UDPSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      perror("socket");
      exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(UDPPORT);
    //server.sin_addr = *((struct in_addr *)host->h_addr);
    server.sin_addr.s_addr = inet_addr(Host);

    bzero(&(server.sin_zero), 8);

    addr_len = sizeof(server);

    curl_easy_setopt(curl, CURLOPT_URL, startstream);
    res = curl_easy_perform(curl);

    if(res != CURLE_OK){
      printf("curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
      exit(1);
    }
    if (bind(UDPSocket, (struct sockaddr *)&server,addr_len) == -1){
      perror("Bind");
      exit(1);
    }
    puts("");
    while(1){
      if ((returnbytes = recvfrom(UDPSocket, buf, MAXBUFLEN, MSG_WAITALL, (struct sockaddr *)&server, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
      }
      if (counter == 500){
        curl_easy_setopt(curl, CURLOPT_URL, sustainstream);
        res = curl_easy_perform(curl);
        counter = 0;
        if(res != CURLE_OK){
          printf("curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
          exit(1);
        }
      }
      length = buf[0] << 8 | buf[1];
      packet_id = buf[16];
      if(packet_id == AUDIO_ID){
        // DO Nothing since I haven't found a suitable Way to create a Microphone
      } else if(packet_id == JPEG_ID){
        video_buf = buf + HEADER_OFFSET;
        videoOutput->write((char*)video_buf, returnbytes-HEADER_OFFSET);
      } else {
        printf("Unrecognized Packet ID");
        exit(1);
      }
      counter ++;
    }
    close(UDPSocket);
  } else {
    puts("Curl Init failed");
    exit(1);
  }

  return 0;
}
