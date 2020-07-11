
#ifndef MISC_H
#define MISC_H

#include "windef.h"

extern const char **current_sha1;
extern const RECT *current_bounds;
extern HCRYPTPROV crypt_prov;

HPALETTE create_default_palette( int bpp );

static inline DWORD get_stride(const BITMAPINFO *bmi)
{
    return ((bmi->bmiHeader.biBitCount * bmi->bmiHeader.biWidth + 31) >> 3) & ~3;
}

static inline DWORD get_dib_size(const BITMAPINFO *bmi)
{
    return get_stride(bmi) * abs(bmi->bmiHeader.biHeight);
}

void reset_bits( HDC hdc, const BITMAPINFO *bmi, BYTE *bits );
char *hash_dib(HDC hdc, const BITMAPINFO *bmi, const void *bits);
void reset_bounds( HDC hdc );
void compare_bounds( HDC hdc, const char *info );
void compare_hash_broken_todo(HDC hdc, const BITMAPINFO *bmi, BYTE *bits, const char *info, int num_broken, BOOL todo);
void compare_hash(HDC hdc, const BITMAPINFO *bmi, BYTE *bits, const char *info);

BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPSTR lpszFileName);
#endif
