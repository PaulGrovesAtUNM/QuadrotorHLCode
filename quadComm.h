#ifndef QUADCOMM_H
#define QUADCOMM_H


void sendByte(char aByte);
void sendNBytes(char *bytes, int num);
void sendText(char *msg);
char bytesAvailable(void);
char getByte(void);
void getBytes(char *buffer, int num);
void skipBytes(int num);
void debugMsg(char *func, char *msg);

#endif
