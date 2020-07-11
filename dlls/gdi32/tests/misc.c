

#include <stdarg.h>
#include <stdio.h>
#include <math.h>

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "wincrypt.h"
#include "mmsystem.h" /* DIBINDEX */

#include "wine/test.h"
#include "misc.h"

const char **current_sha1;
const RECT *current_bounds;
const char *dst_format;
HCRYPTPROV crypt_prov;

static const RECT graphics_bounds[] =
{
    { 0, 0, 0, 0 },
    { 10, 3, 219, 101 },
    { 100, 100, 301, 301 },
    { 0, 0, 201, 201 },
    { 10, 10, 110, 320 },
    { 10, 99, 300, 200 },
    { 99, 12, 201, 200 },
    { 90, 110, 300, 200 },
    { 90, 90, 210, 200 },
    { 10, 99, 300, 200 },
    { 10, 99, 300, 200 },
    { 99, 12, 201, 200 },
    { 99, 11, 201, 200 },
    { 90, 110, 300, 200 },
    { 90, 110, 300, 200 },
    { 10, 10, 365, 405 },
    { 10, 10, 365, 405 },
    { 10, 10, 365, 405 },
    { 10, 10, 365, 405 },
    { 10, 10, 365, 405 },
    { 10, 10, 365, 405 },
    { 10, 10, 365, 405 },
    { 10, 10, 365, 405 },
    { 10, 10, 350, 251 },
    { 10, 10, 300, 200 },
    { 300, 10, 9, 260 },
    { 10, 10, 435, 405 },
    { 10, 10, 120, 120 },
    { 10, 10, 110, 110 },
    { 10, 10, 120, 110 },
    { 10, 10, 110, 120 },
    { 10, 10, 120, 120 },
    { 10, 10, 110, 110 },
    { 10, 10, 120, 110 },
    { 10, 10, 110, 120 },
    { 0, 4, 356, 356 },
    { 100, 100, 356, 356 },
    { 50, 50, 306, 306 },
    { 100, 100, 356, 356 },
    { 100, 100, 356, 356 },
    { 100, 100, 356, 356 },
    { 100, 100, 356, 356 },
    { 100, 100, 356, 356 },
    { 100, 100, 356, 356 },
    { 100, 100, 356, 356 },
    { 100, 100, 356, 356 },
    { 100, 100, 356, 356 },
    { 10, 10, 356, 356 },
    { 100, 100, 356, 356 },
    { 0, 0, 260, 39 },
    { 0, 0, 16, 16 },
    { 10, 10, 416, 26 },
    { 10, 8, 60, 104 },
    { 0, 0, 512, 512 },
    { 0, 10, 511, 306 },
    { 0, 0, 512, 336 },
    { 1, 1, 300, 512 },
    { 0, 0, 500, 512 },
    { 5, 5, 206, 206 },
    { 45, 45, 256, 256 },
    { 86, 86, 215, 215 },
    { 45, 45, 256, 256 },
    { 8, 0, 392, 231 },
    { 8, 0, 392, 231 },
    { 0, 0, 60, 20 },
    { 0, 0, 512, 512 },
    { -1, -1, -1, -1 }  /* the end */
};

static const RGBQUAD default_palette_1[2] =
{
    { 0x00, 0x00, 0x00 }, { 0xff, 0xff, 0xff }
};

static const RGBQUAD default_palette_4[16] =
{
    { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x80 }, { 0x00, 0x80, 0x00 }, { 0x00, 0x80, 0x80 },
    { 0x80, 0x00, 0x00 }, { 0x80, 0x00, 0x80 }, { 0x80, 0x80, 0x00 }, { 0x80, 0x80, 0x80 },
    { 0xc0, 0xc0, 0xc0 }, { 0x00, 0x00, 0xff }, { 0x00, 0xff, 0x00 }, { 0x00, 0xff, 0xff },
    { 0xff, 0x00, 0x00 }, { 0xff, 0x00, 0xff }, { 0xff, 0xff, 0x00 }, { 0xff, 0xff, 0xff },
};

static const RGBQUAD default_palette_8[256] =
{
    { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x80 }, { 0x00, 0x80, 0x00 }, { 0x00, 0x80, 0x80 },
    { 0x80, 0x00, 0x00 }, { 0x80, 0x00, 0x80 }, { 0x80, 0x80, 0x00 }, { 0xc0, 0xc0, 0xc0 },
    { 0xc0, 0xdc, 0xc0 }, { 0xf0, 0xca, 0xa6 }, { 0x00, 0x20, 0x40 }, { 0x00, 0x20, 0x60 },
    { 0x00, 0x20, 0x80 }, { 0x00, 0x20, 0xa0 }, { 0x00, 0x20, 0xc0 }, { 0x00, 0x20, 0xe0 },
    { 0x00, 0x40, 0x00 }, { 0x00, 0x40, 0x20 }, { 0x00, 0x40, 0x40 }, { 0x00, 0x40, 0x60 },
    { 0x00, 0x40, 0x80 }, { 0x00, 0x40, 0xa0 }, { 0x00, 0x40, 0xc0 }, { 0x00, 0x40, 0xe0 },
    { 0x00, 0x60, 0x00 }, { 0x00, 0x60, 0x20 }, { 0x00, 0x60, 0x40 }, { 0x00, 0x60, 0x60 },
    { 0x00, 0x60, 0x80 }, { 0x00, 0x60, 0xa0 }, { 0x00, 0x60, 0xc0 }, { 0x00, 0x60, 0xe0 },
    { 0x00, 0x80, 0x00 }, { 0x00, 0x80, 0x20 }, { 0x00, 0x80, 0x40 }, { 0x00, 0x80, 0x60 },
    { 0x00, 0x80, 0x80 }, { 0x00, 0x80, 0xa0 }, { 0x00, 0x80, 0xc0 }, { 0x00, 0x80, 0xe0 },
    { 0x00, 0xa0, 0x00 }, { 0x00, 0xa0, 0x20 }, { 0x00, 0xa0, 0x40 }, { 0x00, 0xa0, 0x60 },
    { 0x00, 0xa0, 0x80 }, { 0x00, 0xa0, 0xa0 }, { 0x00, 0xa0, 0xc0 }, { 0x00, 0xa0, 0xe0 },
    { 0x00, 0xc0, 0x00 }, { 0x00, 0xc0, 0x20 }, { 0x00, 0xc0, 0x40 }, { 0x00, 0xc0, 0x60 },
    { 0x00, 0xc0, 0x80 }, { 0x00, 0xc0, 0xa0 }, { 0x00, 0xc0, 0xc0 }, { 0x00, 0xc0, 0xe0 },
    { 0x00, 0xe0, 0x00 }, { 0x00, 0xe0, 0x20 }, { 0x00, 0xe0, 0x40 }, { 0x00, 0xe0, 0x60 },
    { 0x00, 0xe0, 0x80 }, { 0x00, 0xe0, 0xa0 }, { 0x00, 0xe0, 0xc0 }, { 0x00, 0xe0, 0xe0 },
    { 0x40, 0x00, 0x00 }, { 0x40, 0x00, 0x20 }, { 0x40, 0x00, 0x40 }, { 0x40, 0x00, 0x60 },
    { 0x40, 0x00, 0x80 }, { 0x40, 0x00, 0xa0 }, { 0x40, 0x00, 0xc0 }, { 0x40, 0x00, 0xe0 },
    { 0x40, 0x20, 0x00 }, { 0x40, 0x20, 0x20 }, { 0x40, 0x20, 0x40 }, { 0x40, 0x20, 0x60 },
    { 0x40, 0x20, 0x80 }, { 0x40, 0x20, 0xa0 }, { 0x40, 0x20, 0xc0 }, { 0x40, 0x20, 0xe0 },
    { 0x40, 0x40, 0x00 }, { 0x40, 0x40, 0x20 }, { 0x40, 0x40, 0x40 }, { 0x40, 0x40, 0x60 },
    { 0x40, 0x40, 0x80 }, { 0x40, 0x40, 0xa0 }, { 0x40, 0x40, 0xc0 }, { 0x40, 0x40, 0xe0 },
    { 0x40, 0x60, 0x00 }, { 0x40, 0x60, 0x20 }, { 0x40, 0x60, 0x40 }, { 0x40, 0x60, 0x60 },
    { 0x40, 0x60, 0x80 }, { 0x40, 0x60, 0xa0 }, { 0x40, 0x60, 0xc0 }, { 0x40, 0x60, 0xe0 },
    { 0x40, 0x80, 0x00 }, { 0x40, 0x80, 0x20 }, { 0x40, 0x80, 0x40 }, { 0x40, 0x80, 0x60 },
    { 0x40, 0x80, 0x80 }, { 0x40, 0x80, 0xa0 }, { 0x40, 0x80, 0xc0 }, { 0x40, 0x80, 0xe0 },
    { 0x40, 0xa0, 0x00 }, { 0x40, 0xa0, 0x20 }, { 0x40, 0xa0, 0x40 }, { 0x40, 0xa0, 0x60 },
    { 0x40, 0xa0, 0x80 }, { 0x40, 0xa0, 0xa0 }, { 0x40, 0xa0, 0xc0 }, { 0x40, 0xa0, 0xe0 },
    { 0x40, 0xc0, 0x00 }, { 0x40, 0xc0, 0x20 }, { 0x40, 0xc0, 0x40 }, { 0x40, 0xc0, 0x60 },
    { 0x40, 0xc0, 0x80 }, { 0x40, 0xc0, 0xa0 }, { 0x40, 0xc0, 0xc0 }, { 0x40, 0xc0, 0xe0 },
    { 0x40, 0xe0, 0x00 }, { 0x40, 0xe0, 0x20 }, { 0x40, 0xe0, 0x40 }, { 0x40, 0xe0, 0x60 },
    { 0x40, 0xe0, 0x80 }, { 0x40, 0xe0, 0xa0 }, { 0x40, 0xe0, 0xc0 }, { 0x40, 0xe0, 0xe0 },
    { 0x80, 0x00, 0x00 }, { 0x80, 0x00, 0x20 }, { 0x80, 0x00, 0x40 }, { 0x80, 0x00, 0x60 },
    { 0x80, 0x00, 0x80 }, { 0x80, 0x00, 0xa0 }, { 0x80, 0x00, 0xc0 }, { 0x80, 0x00, 0xe0 },
    { 0x80, 0x20, 0x00 }, { 0x80, 0x20, 0x20 }, { 0x80, 0x20, 0x40 }, { 0x80, 0x20, 0x60 },
    { 0x80, 0x20, 0x80 }, { 0x80, 0x20, 0xa0 }, { 0x80, 0x20, 0xc0 }, { 0x80, 0x20, 0xe0 },
    { 0x80, 0x40, 0x00 }, { 0x80, 0x40, 0x20 }, { 0x80, 0x40, 0x40 }, { 0x80, 0x40, 0x60 },
    { 0x80, 0x40, 0x80 }, { 0x80, 0x40, 0xa0 }, { 0x80, 0x40, 0xc0 }, { 0x80, 0x40, 0xe0 },
    { 0x80, 0x60, 0x00 }, { 0x80, 0x60, 0x20 }, { 0x80, 0x60, 0x40 }, { 0x80, 0x60, 0x60 },
    { 0x80, 0x60, 0x80 }, { 0x80, 0x60, 0xa0 }, { 0x80, 0x60, 0xc0 }, { 0x80, 0x60, 0xe0 },
    { 0x80, 0x80, 0x00 }, { 0x80, 0x80, 0x20 }, { 0x80, 0x80, 0x40 }, { 0x80, 0x80, 0x60 },
    { 0x80, 0x80, 0x80 }, { 0x80, 0x80, 0xa0 }, { 0x80, 0x80, 0xc0 }, { 0x80, 0x80, 0xe0 },
    { 0x80, 0xa0, 0x00 }, { 0x80, 0xa0, 0x20 }, { 0x80, 0xa0, 0x40 }, { 0x80, 0xa0, 0x60 },
    { 0x80, 0xa0, 0x80 }, { 0x80, 0xa0, 0xa0 }, { 0x80, 0xa0, 0xc0 }, { 0x80, 0xa0, 0xe0 },
    { 0x80, 0xc0, 0x00 }, { 0x80, 0xc0, 0x20 }, { 0x80, 0xc0, 0x40 }, { 0x80, 0xc0, 0x60 },
    { 0x80, 0xc0, 0x80 }, { 0x80, 0xc0, 0xa0 }, { 0x80, 0xc0, 0xc0 }, { 0x80, 0xc0, 0xe0 },
    { 0x80, 0xe0, 0x00 }, { 0x80, 0xe0, 0x20 }, { 0x80, 0xe0, 0x40 }, { 0x80, 0xe0, 0x60 },
    { 0x80, 0xe0, 0x80 }, { 0x80, 0xe0, 0xa0 }, { 0x80, 0xe0, 0xc0 }, { 0x80, 0xe0, 0xe0 },
    { 0xc0, 0x00, 0x00 }, { 0xc0, 0x00, 0x20 }, { 0xc0, 0x00, 0x40 }, { 0xc0, 0x00, 0x60 },
    { 0xc0, 0x00, 0x80 }, { 0xc0, 0x00, 0xa0 }, { 0xc0, 0x00, 0xc0 }, { 0xc0, 0x00, 0xe0 },
    { 0xc0, 0x20, 0x00 }, { 0xc0, 0x20, 0x20 }, { 0xc0, 0x20, 0x40 }, { 0xc0, 0x20, 0x60 },
    { 0xc0, 0x20, 0x80 }, { 0xc0, 0x20, 0xa0 }, { 0xc0, 0x20, 0xc0 }, { 0xc0, 0x20, 0xe0 },
    { 0xc0, 0x40, 0x00 }, { 0xc0, 0x40, 0x20 }, { 0xc0, 0x40, 0x40 }, { 0xc0, 0x40, 0x60 },
    { 0xc0, 0x40, 0x80 }, { 0xc0, 0x40, 0xa0 }, { 0xc0, 0x40, 0xc0 }, { 0xc0, 0x40, 0xe0 },
    { 0xc0, 0x60, 0x00 }, { 0xc0, 0x60, 0x20 }, { 0xc0, 0x60, 0x40 }, { 0xc0, 0x60, 0x60 },
    { 0xc0, 0x60, 0x80 }, { 0xc0, 0x60, 0xa0 }, { 0xc0, 0x60, 0xc0 }, { 0xc0, 0x60, 0xe0 },
    { 0xc0, 0x80, 0x00 }, { 0xc0, 0x80, 0x20 }, { 0xc0, 0x80, 0x40 }, { 0xc0, 0x80, 0x60 },
    { 0xc0, 0x80, 0x80 }, { 0xc0, 0x80, 0xa0 }, { 0xc0, 0x80, 0xc0 }, { 0xc0, 0x80, 0xe0 },
    { 0xc0, 0xa0, 0x00 }, { 0xc0, 0xa0, 0x20 }, { 0xc0, 0xa0, 0x40 }, { 0xc0, 0xa0, 0x60 },
    { 0xc0, 0xa0, 0x80 }, { 0xc0, 0xa0, 0xa0 }, { 0xc0, 0xa0, 0xc0 }, { 0xc0, 0xa0, 0xe0 },
    { 0xc0, 0xc0, 0x00 }, { 0xc0, 0xc0, 0x20 }, { 0xc0, 0xc0, 0x40 }, { 0xc0, 0xc0, 0x60 },
    { 0xc0, 0xc0, 0x80 }, { 0xc0, 0xc0, 0xa0 }, { 0xf0, 0xfb, 0xff }, { 0xa4, 0xa0, 0xa0 },
    { 0x80, 0x80, 0x80 }, { 0x00, 0x00, 0xff }, { 0x00, 0xff, 0x00 }, { 0x00, 0xff, 0xff },
    { 0xff, 0x00, 0x00 }, { 0xff, 0x00, 0xff }, { 0xff, 0xff, 0x00 }, { 0xff, 0xff, 0xff }
};

HPALETTE create_default_palette( int bpp )
{
    char pal_buffer[sizeof(LOGPALETTE) + 255 * sizeof(PALETTEENTRY)];
    LOGPALETTE *pal = (LOGPALETTE *)pal_buffer;
    PALETTEENTRY *entries = pal->palPalEntry;
    int i;

    pal->palVersion = 0x300;
    pal->palNumEntries = 1 << bpp;
    switch (bpp)
    {
    case 1:
        for (i = 0; i < 2; i++)
        {
            entries[i].peRed   = default_palette_1[i].rgbRed;
            entries[i].peGreen = default_palette_1[i].rgbGreen;
            entries[i].peBlue  = default_palette_1[i].rgbBlue;
            entries[i].peFlags = 0;
        }
        break;
    case 4:
        for (i = 0; i < 16; i++)
        {
            entries[i].peRed   = default_palette_4[i].rgbRed;
            entries[i].peGreen = default_palette_4[i].rgbGreen;
            entries[i].peBlue  = default_palette_4[i].rgbBlue;
            entries[i].peFlags = 0;
        }
        break;
    case 8:
        for (i = 0; i < 256; i++)
        {
            entries[i].peRed   = default_palette_8[i].rgbRed;
            entries[i].peGreen = default_palette_8[i].rgbGreen;
            entries[i].peBlue  = default_palette_8[i].rgbBlue;
            entries[i].peFlags = 0;
        }
        break;
    }
    return CreatePalette( pal );
}

void reset_bits( HDC hdc, const BITMAPINFO *bmi, BYTE *bits )
{
    DWORD size = get_dib_size( bmi );
    if (bits) memset( bits, 0xcc, size );
    else
    {
        void *ddb_bits = HeapAlloc( GetProcessHeap(), 0, size );
        memset( ddb_bits, 0xcc, size );
        SetBitmapBits( GetCurrentObject(hdc, OBJ_BITMAP), size, ddb_bits );
        HeapFree( GetProcessHeap(), 0, ddb_bits );
    }
}

char *hash_dib(HDC hdc, const BITMAPINFO *bmi, const void *bits)
{
    DWORD dib_size = get_dib_size(bmi);
    HCRYPTHASH hash;
    char *buf;
    BYTE hash_buf[20];
    DWORD hash_size = sizeof(hash_buf);
    int i;
    static const char *hex = "0123456789abcdef";

    if(!crypt_prov) return NULL;

    if(!CryptCreateHash(crypt_prov, CALG_SHA1, 0, 0, &hash)) return NULL;

    if (!bits)
    {
        void *ddb_bits = HeapAlloc( GetProcessHeap(), 0, dib_size );
        GetBitmapBits( GetCurrentObject(hdc, OBJ_BITMAP), dib_size, ddb_bits );
        CryptHashData(hash, ddb_bits, dib_size, 0);
        HeapFree( GetProcessHeap(), 0, ddb_bits );
    }
    else CryptHashData(hash, bits, dib_size, 0);

    CryptGetHashParam(hash, HP_HASHVAL, NULL, &hash_size, 0);
    if(hash_size != sizeof(hash_buf)) return NULL;

    CryptGetHashParam(hash, HP_HASHVAL, hash_buf, &hash_size, 0);
    CryptDestroyHash(hash);

    buf = HeapAlloc(GetProcessHeap(), 0, hash_size * 2 + 1);

    for(i = 0; i < hash_size; i++)
    {
        buf[i * 2] = hex[hash_buf[i] >> 4];
        buf[i * 2 + 1] = hex[hash_buf[i] & 0xf];
    }
    buf[i * 2] = '\0';

    return buf;
}

void reset_bounds( HDC hdc )
{
    current_bounds = graphics_bounds;
    SetBoundsRect( hdc, NULL, DCB_RESET | DCB_ENABLE );
}

void compare_bounds( HDC hdc, const char *info )
{
    RECT rect;

    GetBoundsRect( hdc, &rect, DCB_RESET );

    if (current_bounds->left == -1 &&
        current_bounds->top == -1 &&
        current_bounds->right == -1 &&
        current_bounds->bottom == -1)
    {
        ok( 0, "missing bounds, got %s,\n", wine_dbgstr_rect( &rect ));
        return;
    }

    ok( EqualRect( current_bounds, &rect ), "%s: %s: expected bounds %s got %s\n", dst_format, info,
        wine_dbgstr_rect( current_bounds ), wine_dbgstr_rect( &rect ));
    current_bounds++;
}

void compare_hash_broken_todo(HDC hdc, const BITMAPINFO *bmi, BYTE *bits, const char *info, int num_broken, BOOL todo)
{
    char *hash = hash_dib(hdc, bmi, bits);
    BOOL ok_cond;
    int i;

    /* reset the bits for the next test */
    reset_bits( hdc, bmi, bits );

    if(!hash)
    {
        skip("SHA1 hashing unavailable on this platform\n");
        return;
    }

    for(i = 0; i <= num_broken; i++)
    {
        if(current_sha1[i] == NULL)
        {
            ok(current_sha1[i] != NULL, "missing hash, got \"%s\",\n", hash);
            HeapFree(GetProcessHeap(), 0, hash);
            return;
        }
    }

    ok_cond = !strcmp(hash, *current_sha1);

    for(i = 1; i <= num_broken; i++)
        ok_cond = ok_cond || broken( !strcmp(hash, current_sha1[i]) );

    todo_wine_if(todo)
        ok( ok_cond, "%s: %s: expected hash %s got %s\n",
            dst_format, info, *current_sha1, hash );

    current_sha1 += num_broken + 1;

    HeapFree(GetProcessHeap(), 0, hash);

    compare_bounds( hdc, info );
}

void compare_hash(HDC hdc, const BITMAPINFO *bmi, BYTE *bits, const char *info)
{
    compare_hash_broken_todo(hdc, bmi, bits, info, 0, FALSE);
}

BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPSTR lpszFileName)
{
    HDC hDC;
    int iBits;
    WORD wBitCount;
    DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
    BITMAP Bitmap0;
    BITMAPFILEHEADER bmfHdr;
    BITMAPINFOHEADER bi;
    LPBITMAPINFOHEADER lpbi;
    HANDLE fh, hDib, hPal, hOldPal2 = NULL;
    hDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else
        wBitCount = 24;
    GetObjectA(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap0.bmWidth;
    bi.biHeight = -Bitmap0.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrImportant = 0;
    bi.biClrUsed = 256;
    dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
        * Bitmap0.bmHeight;
    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;

    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = GetDC(NULL);
        hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
        RealizePalette(hDC);
    }


    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
        + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

    if (hOldPal2)
    {
        SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
    }

    fh = CreateFileA(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;

    bmfHdr.bfType = 0x4D42; // "BM"
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

    WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
    return TRUE;
}

void func_misc()
{
}
