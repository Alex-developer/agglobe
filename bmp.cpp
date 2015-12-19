
#include "xearth.h"
#include "kljcpyrt.h"

#include "settings.h"

extern void LoadMarkers();

static void bmp_setup();
static void bmp_row(u_char *);
static void bmp_cleanup();

static int bmp_line;

static u16or32 *dith;

static bool bTrack;
static bool bFirst;


#pragma pack(1)
static struct BitmapHeader {
  BITMAPFILEHEADER bfh;
  BITMAPINFOHEADER bmih;
  RGBQUAD cmap[256];
  WORD padding;
} *Header;
#pragma pack()

static void *BitmapBits;


static void draw_outlined_string(HDC dc, COLORREF fg, COLORREF bg, int x, int y, const char *text, int len)
{
  SetBkMode(dc, TRANSPARENT);
  SetTextColor(dc, bg);
//  TextOut(dc, x+1, y, text, len);
//  TextOut(dc, x-1, y, text, len);
//  TextOut(dc, x, y+1, text, len);
//  TextOut(dc, x, y-1, text, len);
  SetTextColor(dc, fg);
  TextOut(dc, x, y, text, len);
}


static void mark_location(HDC dc, const MarkerInfo *info, COLORREF color, int dia)
{
  int         x, y;
  int         len;
  double      lat, lon;
  double      pos[3];
  char       *text;
  SIZE        extents;
  HGDIOBJ     op;
	HRGN	rgnCircle;
	HBRUSH hBrush;

  lat = info->lat * (M_PI/180);
  lon = info->lon * (M_PI/180);

  pos[0] = sin(lon) * cos(lat);
  pos[1] = sin(lat);
  pos[2] = cos(lon) * cos(lat);

  XFORM_ROTATE(pos, view_pos_info);

  if (proj_type == ProjTypeOrthographic)
  {
    /* if the marker isn't visible, return immediately
     */
    if (pos[2] <= 0) return;
  }
  else /* (proj_type == ProjTypeMercator) */
  {
    /* apply mercator projection
     */
    pos[0] = MERCATOR_X(pos[0], pos[2]);
    pos[1] = MERCATOR_Y(pos[1]);
  }

  x = (int)XPROJECT(pos[0]);
  y = (int)YPROJECT(pos[1]);

	if (bTrack)
	{
//		if (bFirst)
//		{
//			MoveToEx(dc,x,y,NULL);
//			bFirst = false;
//		} else
//		{
//			op = SelectObject(dc, CreatePen(PS_SOLID, 1, color));
//			LineTo(dc,x,y);
//		}

		op = SelectObject(dc, CreatePen(PS_SOLID, 1, color));
		Arc(dc, x-dia, y-dia, x+dia, y+dia, 0, 0, 0, 0);
		rgnCircle =CreateEllipticRgn(x-dia, y-dia, x+dia, y+dia);
		hBrush = CreateSolidBrush(color);
		FillRgn(dc,rgnCircle,hBrush);
		DeleteObject(rgnCircle);
		DeleteObject(hBrush);
	
	} else
	{
		op = SelectObject(dc, GetStockObject(BLACK_PEN));
		Arc(dc, x-(dia+1), y-(dia+1), x+(dia+1), y+(dia+1), 0, 0, 0, 0);
		Arc(dc, x-(dia-1), y-(dia-1), x+(dia-1), y+(dia-1), 0, 0, 0, 0);
		SelectObject(dc, CreatePen(PS_SOLID, 1, color));
		Arc(dc, x-dia, y-dia, x+dia, y+dia, 0, 0, 0, 0);
	}	

  text = info->label;
  if (text != NULL)
  {
    len = strlen(text);
    GetTextExtentPoint32(dc, text, len, &extents);

    switch (info->align)
    {
    case MarkerAlignLeft:
      x -= extents.cx + 4;
      y -= extents.cy / 2;
      break;

    case MarkerAlignRight:
    case MarkerAlignDefault:
      x += dia+1;
      y -= extents.cy / 2;
      break;

    case MarkerAlignAbove:
      x -= extents.cx / 2;
      y -= extents.cy + 4;
      break;

    case MarkerAlignBelow:
      x -= extents.cx / 2;
      y += 5;
      break;

    default:
      assert(0);
    }

    draw_outlined_string(dc, color, RGB(0, 0, 0), x, y, text, len);
  }

  DeleteObject(SelectObject(dc, op));
}


static void draw_label(HDC dc)
{
  int         dy;
  int         x, y;
  int         len;
  char        buf[128];
  SIZE        extents;
  TEXTMETRIC  tm;
  //SYSTEMTIME  now;

  GetTextMetrics(dc, &tm);

  dy = tm.tmHeight + 1;

  if (labelpos < 2) /* top left or top right */
  {
    y = 5;
    if (hght == GetSystemMetrics(SM_CYSCREEN)) {
      RECT wa;
      SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
      y += wa.top;
    }
  }
  else
  {
    y = hght - 5;
    y -= 3 * dy;                /* 3 lines of text */
    if (hght == GetSystemMetrics(SM_CYSCREEN)) {
      RECT wa;
      SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
      y -= GetSystemMetrics(SM_CYSCREEN) - wa.bottom;
    }
  }

  strftime(buf, sizeof(buf), "%d %b %y %H:%M %z", localtime(&current_time));
  len = strlen(buf);
  GetTextExtentPoint32(dc, buf, len, &extents);
  if (labelpos == 0 || labelpos == 2) /* top left or bottom left */
    x = 5;
  else
    x = wdth - 5 - extents.cx;
  draw_outlined_string(dc, RGB(255, 255, 255), RGB(0, 0, 0), x, y, buf, len);
  y += dy;

  sprintf(buf, "view %.1f %c %.1f %c",
          fabs(view_lat), ((view_lat < 0) ? 'S' : 'N'),
          fabs(view_lon), ((view_lon < 0) ? 'W' : 'E'));
  len = strlen(buf);
  GetTextExtentPoint32(dc, buf, len, &extents);
  if (labelpos == 0 || labelpos == 2) /* top left or bottom left */
    x = 5;
  else
    x = wdth - 5 - extents.cx;
  draw_outlined_string(dc, RGB(255, 255, 255), RGB(0, 0, 0), x, y, buf, len);
  y += dy;

  sprintf(buf, "sun %.1f %c %.1f %c",
          fabs(sun_lat), ((sun_lat < 0) ? 'S' : 'N'),
          fabs(sun_lon), ((sun_lon < 0) ? 'W' : 'E'));
  len = strlen(buf);
  GetTextExtentPoint32(dc, buf, len, &extents);
  if (labelpos == 0 || labelpos == 2) /* top left or bottom left */
    x = 5;
  else
    x = wdth - 5 - extents.cx;
  draw_outlined_string(dc, RGB(255, 255, 255), RGB(0, 0, 0), x, y, buf, len);
  y += dy;
}

void DrawTrack(HDC dc)
{
	int i;
	
	bTrack = true;
	bFirst = true;
	for (i=0;i<Settings.lElements;i++)
	{
		mark_location(dc, &Settings.satTrack[i]  , Settings.satTrack[i].lColour, 2);
	}
}

void bmp_output()
{
  char fn[MAX_PATH];
  HANDLE outf;
  HANDLE outmap;
  HDC dc;
  HBITMAP bmp;
  HGDIOBJ ob, of;
  MarkerInfo *minfo;

  bTrack = false;
  compute_positions();
  scan_map();
  do_dots();
  GetTempPath(sizeof(fn), fn);
  if (fn[strlen(fn)-1] != '\\') {
    strcat(fn, "\\");
  }
  strcat(fn, "AGGlobe.bmp");
  outf = CreateFile(fn, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (outf != INVALID_HANDLE_VALUE) {
    outmap = CreateFileMapping(outf, NULL, PAGE_READWRITE, 0, sizeof(struct BitmapHeader)+wdth*hght, NULL);
    if (outmap != NULL) {
      Header = (BitmapHeader *)MapViewOfFile(outmap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(struct BitmapHeader));
      if (Header != NULL) {
        bmp_setup();
        bmp = CreateDIBSection(NULL, (BITMAPINFO *)&Header->bmih, DIB_RGB_COLORS, &BitmapBits, outmap, sizeof(struct BitmapHeader));
        if (bmp != NULL) {
          LoadMarkers();
          render(bmp_row);
          if (do_markers || do_label) {
            dc = CreateCompatibleDC(0);
            ob = SelectObject(dc, bmp);
            of = SelectObject(dc, CreateFont(-8, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "MS Sans Serif"));
            if (do_markers) {
              minfo = marker_info;
              while (minfo->label != NULL)
              {
                mark_location(dc, minfo, RGB(255, 255, 0), 2);
                minfo += 1;
              }
            }
            if (Settings.satpos.label != NULL) 
			{
				mark_location(dc, &Settings.satpos , RGB(255, 0, 0), 4);
				DrawTrack( dc );
			}
			if (do_label) {
              draw_label(dc);
            }
            DeleteObject(SelectObject(dc, of));
            SelectObject(dc, ob);
            DeleteDC(dc);
          }
          DeleteObject(bmp);
        }
        bmp_cleanup();
        UnmapViewOfFile(Header);
      }
      CloseHandle(outmap);
    }
    CloseHandle(outf);
//	if (Settings.bDisplayondesktop )
//		SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, fn, 0);
  }
}


static void bmp_setup()
{
  int i, bmp_header_size;

  if (num_colors > 256)
    fatal("number of colors must be <= 256 with BMP output");

  dither_setup(num_colors);
  dith = (u16or32 *) malloc((unsigned) sizeof(u16or32) * wdth);
  assert(dith != NULL);

  for (i=0; i<dither_ncolors; i++)
  {
    Header->cmap[i].rgbRed   = dither_colormap[i*3+0];
    Header->cmap[i].rgbGreen = dither_colormap[i*3+1];
    Header->cmap[i].rgbBlue  = dither_colormap[i*3+2];
    Header->cmap[i].rgbReserved = 0;
  }
  Header->cmap[i].rgbRed   = 255;
  Header->cmap[i].rgbGreen = 0;
  Header->cmap[i].rgbBlue  = 0;
  Header->cmap[i].rgbReserved = 0;
  i++;
  Header->cmap[i].rgbRed   = 255;
  Header->cmap[i].rgbGreen = 255;
  Header->cmap[i].rgbBlue  = 0;
  Header->cmap[i].rgbReserved = 0;
  i++;

  bmp_header_size = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);

  ZeroMemory(&Header->bfh, sizeof(BITMAPFILEHEADER));
  Header->bfh.bfType = 'MB';
  Header->bfh.bfSize = bmp_header_size+wdth*hght;
  Header->bfh.bfOffBits = bmp_header_size;;
  ZeroMemory(&Header->bmih, sizeof(BITMAPINFOHEADER));
  Header->bmih.biSize = sizeof(BITMAPINFOHEADER);
  Header->bmih.biWidth = wdth;
  Header->bmih.biHeight = hght;
  Header->bmih.biPlanes = 1;
  Header->bmih.biBitCount = 8;
  Header->bmih.biCompression = BI_RGB;
  Header->bmih.biClrUsed = 256;
  Header->bmih.biClrImportant = 256;
  bmp_line = hght - 1;
}


static void bmp_row(u_char *row)
{
  int i;
  u16or32 *tmp;
  u_char *p;

  tmp = dith;
  dither_row(row, tmp);

  p = ((u_char *)BitmapBits) + bmp_line*wdth;

  for (i = 0; i < wdth; i++) {
    *p++ = (u_char)tmp[i];
  }

  bmp_line--;
}


static void bmp_cleanup()
{
  dither_cleanup();
  free(dith);
}
