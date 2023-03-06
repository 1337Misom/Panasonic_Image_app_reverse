#define byte unsigned char
#define AUDIO_BUFFER_SIZE 4096 * 10

#define UDPPORT 49700
#define MAXBUFLEN 1024*40
#define MAXURLLENGTH 125
#define AUDIO_ID 0x61
#define JPEG_ID 0x21
#define HEADER_OFFSET 56
#define WIDTH 640
#define HEIGHT 360
#define APPROXIMATE_START 30
#define JPEG_HEADER_LENGTH 2
#define ButtonWidth 48
#define ButtonHeight 32
#define WIDTHControl ButtonWidth*5
#define HEIGHTControl ButtonHeight*2
#define NumberofButts 8
#define NumberofIcons 10
#define MAXPAGESIZE 1024000*4 // 4mb
#define MAXPATHSIZE 100
#define CURLTimeout 5

const char Host[] = "0.0.0.0";

const char getinfopatt[] = "http://%s/cam.cgi?mode=getinfo&type=capability";
const char startstreampatt[] = "http://%s/cam.cgi?mode=startstream&value=%d";
const char sustainstreampatt[] = "http://%s/cam.cgi?mode=getstate";
const char setrecmodepatt[] = "http://%s/cam.cgi?mode=camcmd&value=recmode";

const char windowicon[] = "icons/logo.png";

const uint8_t jpeg_header[JPEG_HEADER_LENGTH] = {0xff,0xd8};

char *camera;

char startstream[MAXURLLENGTH];
char getinfo[MAXURLLENGTH];
char sustainstream[MAXURLLENGTH];
char setrecmode[MAXURLLENGTH];
bool quit = false;

// circular audio buffer
unsigned char *audio_buffer;
unsigned char *audio_end;

unsigned char *audio_read;
unsigned char *audio_write;
