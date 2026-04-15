#pragma once
#include "compiler.h"

#define FILEATTR_READONLY   0x01
#define FILEATTR_DIRECTORY  0x10
#define FILEATTR_VOLUME     0x08

typedef struct {
    UINT    attr;
    OEMCHAR path[MAX_PATH];
} FLINFO;

FILEH file_open(const OEMCHAR *path);
FILEH file_open_rb(const OEMCHAR *path);
FILEH file_create(const OEMCHAR *path);
void  file_close(FILEH handle);
UINT  file_read(FILEH handle, void *buf, UINT size);
UINT  file_write(FILEH handle, const void *buf, UINT size);
long  file_seek(FILEH handle, long offset, int origin);
UINT32 file_getsize(FILEH handle);
void  file_getdatetime(FILEH handle, DOSDATE *date, DOSTIME *time);

FLISTH file_findopen(const OEMCHAR *path);
BRESULT file_findnext(FLISTH handle, FLINFO *info);
void  file_findclose(FLISTH handle);
void  file_catname(OEMCHAR *dst, const OEMCHAR *src, int maxlen);
void  file_cpyname(OEMCHAR *dst, const OEMCHAR *src, int maxlen);
void  file_cutname(OEMCHAR *path);
void  file_cutext(OEMCHAR *path);
void  file_setext(OEMCHAR *path, const OEMCHAR *ext, int maxlen);
BRESULT file_getfattr(const OEMCHAR *path);
BOOL  file_cmpname(const OEMCHAR *a, const OEMCHAR *b);
OEMCHAR *file_getext(const OEMCHAR *path);
OEMCHAR *file_getname(const OEMCHAR *path);
OEMCHAR *file_getcd(const OEMCHAR *filename);
void  file_setcd(const OEMCHAR *path);
void  file_setseparator(OEMCHAR *path, int maxlen);

UINT file_attr(const OEMCHAR *path);
BRESULT file_delete(const OEMCHAR *path);
BRESULT file_rename(const OEMCHAR *oldpath, const OEMCHAR *newpath);
BRESULT file_dircreate(const OEMCHAR *path);
BRESULT file_dirdelete(const OEMCHAR *path);
