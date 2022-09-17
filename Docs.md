# Here i will Describe How the Camera Works

## Panasonic's Video/Audio Format

### Packets
The Camera Sends Two types of Packets : Jpeg or Audio.
#### Structure
The first two bytes indicate the length of the packet.
The next 14 seem to do nothing.
In byte 16 is the Packet id (Either 0x61(Audio) or 0x21(Jpeg)) stored.
Everything after byte 56 is the data for the packet id.

### Audio
Audio is encoded in mulaw and always 160 bytes long.

### Video (Jpeg)
Each jpeg frame is 640x360 and gets roughly transmitted every 40ms(25fps).

### Start the Stream
The stream starts if http://<Ip of the Camera>/cam.cgi?mode=startstream&value=(UDP Port) gets send and the camera will try to connect to an udp server on the specified port.

### Sustain a Stream
You have to send an get request to http://<IP of the Camera>/cam.cgi?mode=getstate every 10 seconds to continue getting data on the udp port.

### Special Urls
Zoom In slow : /cam.cgi?mode=camcmd&value=tele-normal
Zoom In fast : /cam.cgi?mode=camcmd&value=tele-fast
Zoom Out slow : /cam.cgi?mode=camcmd&value=wide-normal
Zoom Out fast : /cam.cgi?mode=camcmd&value=wide-fast
Zoomstop : /cam.cgi?mode=camcmd&value=zoomstop
Start Recording : /cam.cgi?mode=camcmd&value=video_recstart
Stop Recording : /cam.cgi?mode=camcmd&value=video_recstop
Capture Image : /cam.cgi?mode=camcmd&value=capture
Picmode : /cam.cgi?mode=camcmd&value=pictmode
Recmode : /cam.cgi?mode=camcmd&value=recmode
Stop stream : /cam.cgi?mode=camcmd&value=stopstream
 
