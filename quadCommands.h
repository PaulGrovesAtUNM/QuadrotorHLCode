// quadCommands.h

#ifndef QUADCOMMANDS_H
#define QUADCOMMANDS_H

#define SOF 234 //Start of Frame
#define EOFm 213 //End of Frame
#define DMC 201 //Direct Motor Command
#define VERSION 202 //HL Code Version Number
#define DEBUGMODE 203 //Debug Mode = data[0]
#define GPIO 204 //GPIO SSSS (0 = off, !0 = on)
#define GPIOTOGGLE 205
#define ECHO 206

#define IMUFRAME 0
#define STATUSFRAME 1
#define ECHOFRAME 2
#define VERSIONFRAME 3

#define	QUAD_VERSION  "0.3.9"

#endif
