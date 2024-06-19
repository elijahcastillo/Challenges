#ifndef EDITOR_UTILS_H
#define EDITOR_UTILS_H



#define LOG(fmt, ...) logMessage("%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__)

char* readFileToString(const char *filename);
void logMessage(const char *format, ...);



#endif // EDITOR_UTILS_H
