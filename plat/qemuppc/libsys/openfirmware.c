#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "libsys.h"

static void* stdout_handle;

static void* of_finddevice(const char* name)
{
    struct
    {
        const char* method;
        int ins, outs;
        const char* name;
        void* phandle;
    } args;

    args.method = "finddevice";
    args.ins = 1;
    args.outs = 1;
    args.name = name;
    openfirmware_call(&args);
    return args.phandle;
}

static int of_getprop(void* phandle, const char* name, void* dest, int destlen)
{
    struct
    {
        const char* method;
        int ins, outs;
        void* phandle;
        const char* name;
        void* dest;
        int destlen;
        int flag;
    } args;

    args.method = "getprop";
    args.ins = 4;
    args.outs = 1;
    args.phandle = phandle;
    args.name = name;
    args.dest = dest;
    args.destlen = destlen;
    openfirmware_call(&args);
    return args.flag;
}

void openfirmware_init(void)
{
    void* chosen = of_finddevice("/chosen");
    of_getprop(chosen, "stdout", &stdout_handle, sizeof(stdout_handle));
}

unsigned char _sys_rawread(void)
{
    return 0;
}

void _sys_rawwrite(unsigned char c)
{
    struct
    {
        const char* method;
        int ins, outs;
        void* ihandle;
        void* address;
        int len;
        int actual;
    } args;

    args.method = "write";
    args.ins = 3;
    args.outs = 1;
    args.ihandle = stdout_handle;
    args.address = &c;
    args.len = 1;
    openfirmware_call(&args);
}
