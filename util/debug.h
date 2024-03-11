#ifndef __DEBUG_H_
#define __DEBUG_H_

///////////////////////////////////////////////////////////////////////////////
// system includes
///////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <syslog.h>
#include <cassert>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>

#endif

#include <QEvent>
#include <QMetaEnum>

///////////////////////////////////////////////////////////////////////////////
// Debugger
///////////////////////////////////////////////////////////////////////////////
extern short g_nDebug;

#define DEBUG_BUFFER 1024

///////////////////////////////////////////////////////////////////////////////
// Makros
///////////////////////////////////////////////////////////////////////////////

// ERROR[<file>:<line>]:<text> due to: <error>, message to cerr and syslog without exit
/*#define ERROR(fmt...)\
{\
        char buffer1[DEBUG_BUFFER],\
        buffer2[DEBUG_BUFFER],\
        buffer3[DEBUG_BUFFER],\
        buffer[DEBUG_BUFFER*3];\
        sprintf(buffer1, "ERROR[%s:%d]: ", __FILE__, __LINE__);\
        sprintf(buffer2, ##fmt);\
        sprintf(buffer3, " due to: %s\n", strerror(errno));\
        strcpy(buffer, buffer1);\
        strcat(buffer, buffer2);\
        strcat(buffer, buffer3);\
        fprintf(stderr, "%s", buffer);\
        syslog(LOG_ERR, "%s", buffer);\
}

// LOG[<file>:<line>]:<text>, trace message to cerr and syslog
#define LOG(fmt...)\
{\
    fprintf(stdout, "LOG: ");\
    fprintf(stdout, ##fmt);\
    fprintf(stdout, "\n");\
        syslog(LOG_INFO, ##fmt);\
}
*/

class DebugHelpers
{
public:
    // Gives human-readable event type information.
    static QString QEventToString(const QEvent* event)
    {
        QString name;
        static int eventEnumIndex = QEvent::staticMetaObject.indexOfEnumerator("Type");
        if (event)
            name = QEvent::staticMetaObject.enumerator(eventEnumIndex).valueToKey(event->type());
        return name;
    }
};

#ifdef _DEBUG

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// TRACE[<level>, <file>:<line>]:<text>, trace message to cerr
#define TRACE(lvl, fmt...)\
{\
        if (g_nDebug >= lvl)\
        {\
                struct timeval tv;\
                struct timezone tz;\
                struct tm *tm;\
                gettimeofday(&tv, &tz);\
                tm=localtime(&tv.tv_sec);\
                pid_t tid = (pid_t) syscall (SYS_gettid);\
                fprintf(stdout, "[%02d:%02d:%02d.%03lu][tid=%d] TRACE[%s:%d]: ", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec/1000, tid, __FILENAME__, __LINE__);\
                fprintf(stdout, ##fmt);\
                fprintf(stdout,"\n");\
                fflush(stdout);\
        }\
}

// ASSERT
#define ASSERT(logicalExpression, fmt...)\
{\
        if (!(logicalExpression))\
        {\
                char buffer1[DEBUG_BUFFER],\
                buffer2[DEBUG_BUFFER],\
                buffer[DEBUG_BUFFER*3];\
                sprintf(buffer1, "ASSERT[%s:%d]: ", __FILENAME__, __LINE__);\
                sprintf(buffer2, ##fmt);\
                strcpy(buffer, buffer1);\
                strcat(buffer, buffer2);\
                fprintf(stderr, "%s\n", buffer);\
                exit(1);\
        }\
}
#else
#define ASSERT(logicalExpression, fmt...) // empty definition
#define TRACE(lvl, fmt...) // empty definition
#endif

#endif // __DEBUG_H_
