#include "g711.c"



void
decodemulaw (int8_t * mulaw, int16_t * pcm, uint16_t size)
{
  for (int i = 0; i <= size; i++)
    {
      pcm[i] = ulaw2linear (mulaw[i]);
    }
}

void
read_from_audio_buffer (byte * buf, int length)
{

  int org_length = length;

  if (length + audio_read > audio_end)
    {
      memcpy (buf, audio_read, audio_end - audio_read);
      length -= audio_end - audio_read;
      audio_read = audio_buffer;
    }
  memcpy (buf, audio_read, length);
  audio_read += length;
}

void
write_to_audio_buffer (byte * buf, int length)
{

  if (length + audio_write > audio_end)
    {
      memcpy (audio_write, buf, audio_end - audio_write);
      length -= audio_end - audio_write;
      audio_write = audio_buffer;
    }
  memcpy (audio_write, buf, length);
  audio_write += length;
}

void *
play_samples_in_thread (void *vargp)
{
  ao_device *device;
  ao_sample_format format;
  int default_driver;
  int byteread = 320;
  unsigned char *buffer = (unsigned char *) malloc (byteread);
  ao_initialize ();
  default_driver = ao_default_driver_id ();
  memset (&format, 0, sizeof (format));
  format.bits = 16;
  format.channels = 1;
  format.rate = 8000;
  format.byte_format = AO_FMT_LITTLE;
  device = ao_open_live (default_driver, &format, NULL /* no options */ );
  if (device == NULL)
    {
      fprintf (stderr, "Error opening device.\n");
      exit (1);
    }
  while (!quit)
    {
      read_from_audio_buffer (buffer, byteread);
      ao_play (device, (char *) buffer, byteread);
    }
  ao_close (device);
  ao_shutdown ();
  return NULL;
}
