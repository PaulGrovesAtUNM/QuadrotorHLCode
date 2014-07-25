#ifndef QUADCOMM_H
#define QUADCOMM_H


void sendByte(char aByte);
void sendNBytes(char *bytes, int num);
void sendText(char *msg);
char bytesAvailable(void);
char getByte(void);
void getBytes(char *buffer, int num);
char peekByte(void);
int findInBuffer(int startIndex, int distance, char aByte);
int skipBytes(int num);
void pushBack(char byte);
void debugMsg(char *func, char *msg);
void showRBuffer(void);

#endif
