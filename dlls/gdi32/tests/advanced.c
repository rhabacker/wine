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

/* Comparing hashes does not work because drawing simple lines is already different from native win32 */
/* #define WITH_HASH */

typedef struct {
    HDC dc;
    BITMAPINFO *bmi;
    BYTE *bits;
    HBITMAP dib;
#ifdef WITH_HASH
    char *hash_dib;
#endif
    char test_name[1024];
    char save_path[MAX_PATH];
    char hash_name[1024];
    RECT bounds;
    double angle;
    double shearX;
} TestData;

TestData test_data;
TestData *td = &test_data;

double radians (double d)
{
    return d * M_PI / 180;
}

void set_transform(HDC dc, RECT *bounds, double angle)
{
    XFORM xf;
    double r = radians(angle);
    xf.eM11 = cos(r) + sin(r) * td->shearX;
    xf.eM22 = cos(r);
    xf.eM12 = -sin(r);
    xf.eM21 = sin(r) + cos(r) * td->shearX;
    xf.eDx = (bounds->right - bounds->left) / 4;
    xf.eDy = (bounds->bottom - bounds->top) / 4;
    SetWorldTransform(dc, &xf);
}

BOOL save_bitmap( TestData *td)
{
    return SaveHBITMAPToFile(td->dib, td->save_path);
}

void init( TestData *td , const char *test_name)
{
    sprintf(td->test_name, "%s", test_name);
    sprintf(td->save_path, "%s-%3.1f-%2.1f.bmp", td->test_name, td->shearX, td->angle);
    sprintf(td->hash_name, "sha1_advanced_%s_%3.1f_%2.1f", td->test_name, td->shearX, td->angle);
    trace("%s\n", td->test_name);
    reset_bits( td->dc, td->bmi, td->bits );
    MoveToEx( td->dc, -5, 0, NULL);
    LineTo( td->dc, 5, 0);
    MoveToEx( td->dc, 0, 5, NULL);
    LineTo( td->dc, 0, -5);
}

void check_hash( TestData *td )
{
#ifdef WITH_HASH
    if ( td->hash_dib )
        HeapFree(GetProcessHeap(), 0, td->hash_dib);
    td->hash_dib = hash_dib( td->dc, td->bmi, td->bits );
    printf("const char *%s = \"%s\";\n", td->hash_name, td->hash_dib );
#endif
    // instead we dump the display to a file for visual inspection
    save_bitmap(td);
}

static void test_gdi(double angle, double shearX)
{
    char bmibuf[sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD)];
    td->bmi = (BITMAPINFO *)bmibuf;
    HBITMAP orig_bm;
    HPALETTE default_palette, old_hpal;
    RECT r;

    td->dc = CreateCompatibleDC(NULL);
    td->angle = angle;
    td->shearX = shearX;

    SetGraphicsMode(td->dc, GM_ADVANCED);
    set_transform(td->dc, &td->bounds, td->angle);

    memset(td->bmi, 0, sizeof(bmibuf));
    td->bmi->bmiHeader.biSize = sizeof(td->bmi->bmiHeader);
    td->bmi->bmiHeader.biHeight = td->bounds.right;
    td->bmi->bmiHeader.biWidth = td->bounds.bottom;
    td->bmi->bmiHeader.biBitCount = 32;
    td->bmi->bmiHeader.biPlanes = 1;
    td->bmi->bmiHeader.biCompression = BI_RGB;

    td->dib = CreateDIBSection(0, td->bmi, DIB_RGB_COLORS, (void**)&(td->bits), NULL, 0);
    orig_bm = SelectObject(td->dc, td->dib);

    default_palette = create_default_palette( 8 );
    old_hpal = SelectPalette( td->dc, default_palette, FALSE );

    current_bounds = &td->bounds;
    SetBoundsRect(td->dc, &td->bounds, DCB_SET);

    SetRect(&r, 0, 0, 128, 128);

    init( td, "LineTo");
    MoveToEx(td->dc, r.left, r.top, NULL);
    LineTo(td->dc, r.right, r.bottom);
    check_hash( td );

    init( td, "Rectangle" );
    ok(Rectangle(td->dc, r.left, r.top, r.right, r.bottom), td->test_name);
    check_hash( td );

    init( td, "Ellipse" );
    ok(Ellipse(td->dc, r.left, r.top, r.right, r.bottom), td->test_name);
    check_hash( td );

    init( td, "Arc" );
    ok(Arc(td->dc, r.left, r.top, r.right, r.bottom, r.left+10, r.top, r.right-10, r.bottom), td->test_name);
    check_hash( td );

    init( td, "ArcTo" );
    ok(ArcTo(td->dc, r.left, r.top, r.right, r.bottom, r.left+10, r.top, r.right-10, r.bottom), td->test_name);
    check_hash( td );

    init( td, "Chord" );
    ok(Chord(td->dc, r.left, r.top, r.right, r.bottom, r.left+10, r.top, r.right-10, r.bottom), td->test_name);
    check_hash( td );

    init( td, "Pie" );
    ok(Pie(td->dc, r.left, r.top, r.right, r.bottom, r.left+10, r.top, r.right-10, r.bottom), td->test_name);
    check_hash( td );

    init( td, "FillRect" );
    ok(FillRect(td->dc, &r, GetStockObject(WHITE_BRUSH)), td->test_name);
    check_hash( td );

    SelectObject( td->dc, orig_bm );
    DeleteObject( td->dib );
    SelectPalette( td->dc, old_hpal, FALSE );
    DeleteDC( td->dc );
}

static void test_advanced_graphics()
{
    td->bounds.left = 0;
    td->bounds.top = 0;
    td->bounds.right = 256;
    td->bounds.bottom = 256;

    test_gdi(0, 0.0);
    test_gdi(30, 0.0);
    test_gdi(60, 0.0);
    test_gdi(90, 0.0);
    test_gdi(0, 0.1);
    test_gdi(30, 0.1);
    test_gdi(60, 0.1);
    test_gdi(90, 0.1);
}

START_TEST(advanced)
{
    CryptAcquireContextW(&crypt_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

    test_advanced_graphics();

    CryptReleaseContext(crypt_prov, 0);
}
