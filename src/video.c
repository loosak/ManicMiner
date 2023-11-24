#include "misc.h"
#include "system.h"

#include "video.h"

typedef struct
{
    BYTE    ink;
    BYTE    point;
}
PIXEL;

PIXEL   videoPixel[WIDTH * HEIGHT];

BYTE    charSet[128][10] =
{
    {0}, // null
    {0}, // paper
    {0}, // ink
    {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {8, 3, 5, 3, 9, 3, 5, 3, 9}, // fringe
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {3, 0, 0, 0},
    {2, 47, 0},
    {4, 3, 0, 3, 0},
    {7, 18, 63, 18, 18, 63, 18, 0},
    {6, 46, 42, 127, 42, 58, 0},
    {7, 35, 19, 8, 4, 50, 49, 0},
    {7, 16, 42, 37, 42, 16, 40, 0},
    {2, 3, 0},
    {3, 30, 33, 0},
    {3, 33, 30, 0},
    {4, 20, 8, 20, 0},
    {4, 8, 28, 8, 0},
    {2, 96, 0},
    {4, 8, 8, 8, 0},
    {2, 32, 0},
    {4, 8, 42, 8, 0},
    {6, 12, 18, 33, 18, 12, 0},
    {4, 34, 63, 32, 0},
    {6, 50, 41, 41, 41, 38, 0},
    {6, 18, 33, 37, 37, 26, 0},
    {5, 15, 8, 60, 8, 0},
    {6, 23, 37, 37, 37, 25, 0},
    {6, 30, 37, 37, 37, 24, 0},
    {6, 1, 1, 49, 13, 3, 0},
    {6, 26, 37, 37, 37, 26, 0},
    {6, 6, 41, 41, 41, 30, 0},
    {2, 20, 0},
    {2, 52, 0},
    {4, 8, 20, 34, 0},
    {6, 20, 20, 20, 20, 20, 0},
    {4, 34, 20, 8, 0},
    {6, 2, 1, 41, 5, 2, 0},
    {7, 30, 33, 45, 43, 45, 14, 0},
    {6, 48, 14, 9, 14, 48, 0},
    {6, 63, 37, 37, 37, 26, 0},
    {6, 30, 33, 33, 33, 18, 0},
    {6, 63, 33, 33, 18, 12, 0},
    {6, 63, 37, 37, 37, 33, 0},
    {6, 63, 5, 5, 5, 1, 0},
    {6, 30, 33, 33, 41, 26, 0},
    {6, 63, 4, 4, 4, 63, 0},
    {4, 33, 63, 33, 0},
    {6, 16, 32, 32, 32, 31, 0},
    {6, 63, 4, 10, 17, 32, 0},
    {6, 63, 32, 32, 32, 32, 0},
    {8, 56, 7, 12, 16, 12, 7, 56, 0},
    {7, 63, 2, 4, 8, 16, 63, 0},
    {6, 30, 33, 33, 33, 30, 0},
    {6, 63, 9, 9, 9, 6, 0},
    {7, 30, 33, 41, 49, 33, 30, 0},
    {6, 63, 9, 9, 25, 38, 0},
    {6, 18, 37, 37, 37, 24, 0},
    {6, 1, 1, 63, 1, 1, 0},
    {6, 31, 32, 32, 32, 31, 0},
    {6, 7, 24, 32, 24, 7, 0},
    {8, 7, 24, 32, 24, 32, 24, 7, 0},
    {7, 33, 18, 12, 12, 18, 33, 0},
    {6, 3, 4, 56, 4, 3, 0},
    {7, 33, 49, 41, 37, 35, 33, 0},
    {3, 63, 33, 0},
    {6, 2, 4, 8, 16, 32, 0},
    {3, 33, 63, 0},
    {6, 4, 2, 63, 2, 4, 0},
    {6, 64, 64, 64, 64, 64, 0},
    {6, 36, 62, 37, 33, 34, 0},
    {5, 16, 42, 42, 60, 0},
    {5, 63, 34, 34, 28, 0},
    {5, 28, 34, 34, 34, 0},
    {5, 28, 34, 34, 63, 0},
    {5, 28, 42, 42, 36, 0},
    {4, 62, 5, 1, 0},
    {5, 28, 162, 162, 126, 0},
    {5, 63, 2, 2, 60, 0},
    {2, 61, 0},
    {4, 32, 64, 61, 0},
    {5, 63, 12, 18, 32, 0},
    {2, 63, 0},
    {6, 62, 2, 60, 2, 60, 0},
    {5, 62, 2, 2, 60, 0},
    {5, 28, 34, 34, 28, 0},
    {5, 254, 34, 34, 28, 0},
    {5, 28, 34, 34, 254, 128},
    {4, 60, 2, 2, 0},
    {5, 36, 42, 42, 16, 0},
    {4, 2, 63, 2, 0},
    {5, 30, 32, 32, 30, 0},
    {6, 6, 24, 32, 24, 6, 0},
    {6, 30, 32, 28, 32, 30, 0},
    {6, 34, 20, 8, 20, 34, 0},
    {5, 30, 160, 160, 126, 0},
    {6, 34, 50, 42, 38, 34, 0},
    {4, 4, 59, 33, 0},
    {2, 63, 0},
    {4, 33, 59, 4, 0},
    {5, 8, 4, 8, 4, 0},
    {9, 60, 66, 153, 165, 165, 129, 66, 60, 0}
};

WORD    charSetLarge[128][8] =
{
    {0, 0, 0, 0, 0, 0, 0, 0}, // null
    {0, 0, 0, 0, 0, 0, 0, 0}, // paper
    {0, 0, 0, 0, 0, 0, 0, 0}, // ink
    {2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048},
    {768, 1152, 1024, 1024, 524, 2578, 2323, 2189},
    {2125, 2081, 2065, 2057, 2053, 2051, 2175, 2111},
    {2072, 2060, 2054, 2563, 3071, 3071, 2561, 2048},
    {2560, 2944, 2784, 2168, 2126, 2119, 2142, 2680},
    {3040, 2944, 2560, 2048, 2561, 3071, 3071, 2566},
    {2060, 2072, 2096, 2144, 2241, 2559, 3071, 2049},
    {2048, 2561, 3071, 3071, 2561, 2048, 2168, 2300},
    {2510, 2306, 2561, 2561, 2561, 2306, 2311, 2180},
    {2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048},
    {2048, 2048, 2048, 2076, 2082, 2081, 2065, 2065},
    {2049, 2561, 3071, 2563, 2063, 2108, 2160, 2080},
    {2072, 2566, 3071, 3071, 2561, 2048, 2561, 3071},
    {3071, 2561, 2048, 2561, 3071, 3071, 2566, 2060},
    {2072, 2096, 2144, 2241, 2559, 3071, 2049, 2048},
    {2561, 3071, 3071, 2577, 2577, 2577, 2617, 2819},
    {2819, 2180, 2048, 2561, 3071, 3071, 2577, 2065},
    {2097, 2161, 2259, 2463, 782, 1536, 1536, 1024},
    {1024, 1216, 704, 2432, 2048, 2048, 2048, 2048},
    {2048, 2048, 2048, 2048, 0, 0, 0, 0},
    {65535, 65535, 65535, 0, 65535, 65535, 65535, 65535}, // white note
    {65535, 65280, 65280, 0, 65280, 65280, 65535, 65535}, // black note
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 60, 7166, 7166, 60, 0, 0},
    {0, 7, 15, 0, 0, 15, 7, 0},
    {528, 8190, 8190, 528, 8190, 8190, 528, 0},
    {3132, 6270, 4162, 12355, 12355, 8134, 3980, 0},
    {6158, 7694, 1920, 480, 120, 7198, 7174, 0},
    {3968, 8156, 4222, 4578, 4030, 8156, 4160, 0},
    {0, 0, 8, 15, 7, 0, 0, 0},
    {0, 0, 2040, 4092, 6150, 4098, 0, 0},
    {0, 0, 4098, 6150, 4092, 2040, 0, 0},
    {128, 672, 992, 448, 992, 672, 128, 0},
    {0, 128, 128, 992, 992, 128, 128, 0},
    {0, 0, 8192, 14336, 6144, 0, 0, 0},
    {128, 128, 128, 128, 128, 128, 128, 0},
    {0, 0, 0, 6144, 6144, 0, 0, 0},
    {6144, 7680, 1920, 480, 120, 30, 6, 0},
    {2040, 4092, 6150, 4098, 6150, 4092, 2040, 0},
    {0, 4104, 4108, 8190, 8190, 4096, 4096, 0},
    {7684, 7942, 4482, 4290, 4194, 6206, 6172, 0},
    {2052, 6150, 4162, 4162, 4162, 8190, 4028, 0},
    {510, 510, 4352, 8160, 8160, 4352, 256, 0},
    {2174, 6270, 4162, 4162, 4162, 8130, 3970, 0},
    {4088, 8188, 4166, 4162, 4162, 8128, 3968, 0},
    {6, 6, 7682, 8066, 450, 126, 62, 0},
    {4028, 8190, 4162, 4162, 4162, 8190, 4028, 0},
    {60, 4222, 4162, 4162, 6210, 4094, 2044, 0},
    {0, 0, 0, 3096, 3096, 0, 0, 0},
    {0, 0, 4096, 7192, 3096, 0, 0, 0},
    {0, 192, 480, 816, 1560, 3084, 2052, 0},
    {576, 576, 576, 576, 576, 576, 576, 0},
    {0, 2052, 3084, 1560, 816, 480, 192, 0},
    {12, 14, 2, 7042, 7106, 126, 60, 0},
    {4088, 8188, 4100, 5060, 5060, 5116, 504, 0},
    {8176, 8184, 140, 134, 140, 8184, 8176, 0},
    {4098, 8190, 8190, 4162, 4162, 8190, 4028, 0},
    {2040, 4092, 6150, 4098, 4098, 6150, 3084, 0},
    {4098, 8190, 8190, 4098, 6150, 4092, 2040, 0},
    {4098, 8190, 8190, 4162, 4322, 6150, 7182, 0},
    {4098, 8190, 8190, 4162, 226, 6, 14, 0},
    {2040, 4092, 6150, 4226, 4226, 3974, 8076, 0},
    {8190, 8190, 64, 64, 64, 8190, 8190, 0},
    {0, 0, 4098, 8190, 8190, 4098, 0, 0},
    {3072, 7168, 4096, 4098, 8190, 4094, 2, 0},
    {4098, 8190, 8190, 192, 1008, 8126, 7182, 0},
    {4098, 8190, 8190, 4098, 4096, 6144, 7168, 0},
    {8190, 8190, 28, 120, 28, 8190, 8190, 0},
    {8190, 8190, 120, 480, 1920, 8190, 8190, 0},
    {4092, 8190, 4098, 4098, 4098, 8190, 4092, 0},
    {4098, 8190, 8190, 4162, 66, 126, 60, 0},
    {4092, 8190, 4098, 7170, 30722, 32766, 20476, 0},
    {4098, 8190, 8190, 66, 450, 8190, 7740, 0},
    {3100, 7230, 4194, 4162, 4290, 8078, 3852, 0},
    {14, 6, 4098, 8190, 8190, 4098, 6, 14},
    {4094, 8190, 4096, 4096, 4096, 8190, 4094, 0},
    {1022, 2046, 3072, 6144, 3072, 2046, 1022, 0},
    {2046, 8190, 7168, 1920, 7168, 8190, 2046, 0},
    {7182, 7998, 1008, 192, 1008, 7998, 7182, 0},
    {30, 62, 4192, 8128, 8128, 4192, 62, 30},
    {7694, 7942, 4482, 4290, 4194, 6206, 7198, 0},
    {0, 0, 8190, 8190, 4098, 4098, 0, 0},
    {6, 30, 120, 480, 1920, 7680, 6144, 0},
    {0, 0, 4098, 4098, 8190, 8190, 0, 0},
    {8, 12, 6, 3, 6, 12, 8, 0},
    {16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384},
    {6176, 8190, 8191, 4129, 4099, 6150, 2048, 0},
    {3584, 7968, 4384, 4384, 4064, 8128, 4096, 0},
    {4098, 8190, 4094, 4128, 4192, 8128, 3968, 0},
    {4032, 8160, 4128, 4128, 4128, 6240, 2112, 0},
    {3968, 8128, 4192, 4130, 4094, 8190, 4096, 0},
    {4032, 8160, 4384, 4384, 4384, 6624, 2496, 0},
    {4160, 8188, 8190, 4162, 6, 12, 0, 0},
    {18368, 53216, 34848, 34848, 65472, 32736, 32, 0},
    {4098, 8190, 8190, 64, 32, 8160, 8128, 0},
    {0, 0, 4128, 8166, 8166, 4096, 0, 0},
    {0, 24576, 57344, 32768, 32800, 65510, 32742, 0},
    {4098, 8190, 8190, 768, 1920, 7392, 6240, 0},
    {0, 0, 4098, 8190, 8190, 4096, 0, 0},
    {8160, 8160, 96, 1984, 96, 8160, 8128, 0},
    {32, 8160, 8128, 32, 32, 8160, 8128, 0},
    {4032, 8160, 4128, 4128, 4128, 8160, 4032, 0},
    {32800, 65504, 65472, 34848, 2080, 4064, 1984, 0},
    {1984, 4064, 2080, 34848, 65472, 65504, 32800, 0},
    {4128, 8160, 8128, 4192, 32, 224, 192, 0},
    {2112, 6368, 4512, 4384, 4896, 7776, 3136, 0},
    {32, 32, 4092, 8190, 4128, 6176, 2048, 0},
    {4064, 8160, 4096, 4096, 4064, 8160, 4096, 0},
    {0, 2016, 4064, 6144, 6144, 4064, 2016, 0},
    {4064, 8160, 6144, 3840, 6144, 8160, 4064, 0},
    {6240, 7392, 1920, 768, 1920, 7392, 6240, 0},
    {2016, 36832, 34816, 34816, 51200, 32736, 16352, 0},
    {6240, 7264, 5664, 4896, 4512, 6368, 6240, 0},
    {0, 192, 192, 4092, 7998, 4098, 4098, 0},
    {0, 0, 0, 8190, 8190, 0, 0, 0},
    {0, 4098, 4098, 7998, 4092, 192, 192, 0},
    {4, 6, 2, 6, 4, 6, 2, 0},
    {4080, 6168, 13260, 9252, 9252, 12876, 6168, 4080}
};

BYTE    textInk[2] = {0x0, 0x0};

int Video_TextWidth(char *text)
{
    int l = 0;

    for ( ; *text; text++)
    {
        l += *charSet[(int)*text];
    }

    return l;
}

void Video_PianoKey(int tile, BYTE ink)
{
    int pos = TILE2PIXEL(tile) - 4;
    int pixel;
    int row, bit;

    for (row = 0; row < 16; row++, pos += WIDTH)
    {
        pixel =  pos;
        for (bit = 0; bit < 7; bit++, pixel++)
        {
            if (videoPixel[pixel].point)
            {
                System_SetPixel(pixel, ink);
            }
        }
    }
}

void Video_AirBar(int pixel, BYTE ink)
{
    System_SetPixel(pixel, ink);
    System_SetPixel(pixel += WIDTH, ink);
    System_SetPixel(pixel += WIDTH, ink);
    System_SetPixel(pixel += WIDTH, ink);
}

void Video_TilePaper(int tile, BYTE paper)
{
    int point, pos;
    int pixel = TILE2PIXEL(tile);

    for (point = 0; point < 64; point++)
    {
        pos = ((point << 5) & 0x700) | (point & 0x7);
        if ((videoPixel[pixel + pos].point & 1) == 0)
        {
            System_SetPixel(pixel + pos, paper);
        }
    }
}

void Video_LevelPaperFill(BYTE paper)
{
    int dest;

    for (dest = 0; dest < 512; dest++)
    {
        Video_TilePaper(dest, paper);
    }
}

void Video_TileInk(int tile, BYTE ink)
{
    int point, pos;
    int pixel = TILE2PIXEL(tile);

    for (point = 0; point < 64; point++)
    {
        pos = ((point << 5) & 0x700) | (point & 0x7);
        if (videoPixel[pixel + pos].point & 1)
        {
            System_SetPixel(pixel + pos, ink);
        }
    }
}

void Video_LevelInkFill(BYTE ink)
{
    int dest;

    for (dest = 0; dest < 512; dest++)
    {
        Video_TileInk(dest, ink);
    }
}

void Video_PixelFill(int pixel, int size, BYTE ink)
{
    for ( ; size > 0; size--, pixel++)
    {
        videoPixel[pixel].point = 0;
        System_SetPixel(pixel, ink);
    }
}

int Video_Tile(int pos, BYTE *gfx, BYTE paper, BYTE ink, int rows)
{
    int     row, bit;
    int     pixel;
    BYTE    byte;
    BYTE    colour[2] = {paper, ink};

    for (row = 0; row < rows; row++, pos += WIDTH, gfx++)
    {
        pixel = pos;
        byte = *gfx;
        for (bit = 0; bit < 8; bit++, pixel--, byte >>= 1)
        {
            videoPixel[pixel].point = byte & B_LEVEL;
            System_SetPixel(pixel, colour[byte & 1]);
        }
    }

    return pos;
}

void Video_Miner(int pos, WORD *line, BYTE ink)
{
    int     row, bit;
    int     pixel;
    WORD    word;

    for (row = 0; row < 16; row++, pos += WIDTH, line++)
    {
        pixel = pos;
        word = *line;
        for (bit = 0; bit < 16; bit++, pixel++, word >>= 1)
        {
            if (word & 1)
            {
                if (videoPixel[pixel].point & B_ROBOT)
                {
                    Action = Die_Action;
                }

                videoPixel[pixel].point |= B_MINER | 1;
                System_SetPixel(pixel, ink);
            }
        }
    }
}

void Video_SpriteBlend(int pos, WORD *line, BYTE ink)
{
    int     row, bit;
    int     pixel;
    WORD    word;

    pos += 15;

    for (row = 0; row < 16; row++, pos += WIDTH, line++)
    {
        pixel = pos;
        word = *line;
        for (bit = 0; bit < 16; bit++, pixel--, word >>= 1)
        {
            if (word & 1)
            {
                videoPixel[pixel].point |= B_ROBOT | 1;
                System_SetPixel(pixel, ink);
            }
        }
    }
}

void Video_Sprite(int pos, WORD *line, BYTE paper, BYTE ink)
{
    int     row, bit;
    int     pixel;
    WORD    word;
    BYTE    colour[2] = {paper, ink};

    pos += 15;

    for (row = 0; row < 16; row++, pos += WIDTH, line++)
    {
        pixel = pos;
        word = *line;
        for (bit = 0; bit < 16; bit++, pixel--, word >>= 1)
        {
            videoPixel[pixel].point = word & 1;
            System_SetPixel(pixel, colour[word & 1]);
        }
    }
}

void Video_DrawPiano()
{
    char    *text = "\x17\x18\x18\x17\x18\x18\x18\x17\x18\x18\x17\x18\x18\x18\x17\x18\x18\x17\x18\x18\x18\x17\x18\x18\x17\x18\x18\x18\x17\x18\x18\x17";
    int     col, bit, pos = 128 * WIDTH;
    WORD    *byte, line;
    int     pixel;
    BYTE    ink[2] = {0x0, 0x7};

    for ( ; *text; text++)
    {
        byte = charSetLarge[(int)*text];

        for (col = 0; col < 8; col++, byte++, pos++)
        {
            pixel = pos;
            line = *byte;
            for (bit = 0; bit < 16; bit++, pixel += WIDTH, line >>= 1)
            {
                videoPixel[pixel].point = line & 1;
                System_SetPixel(pixel, ink[line & 1]);
            }
        }
    }
}

int TextCode(char *text)
{
    if (*text == 0x1) // paper
    {
        textInk[0] = *(text + 1);
        return 1;
    }

    if (*text == 0x2) // ink
    {
        textInk[1] = *(text + 1);
        return 1;
    }

    return 0;
}

void Video_WriteLarge(int pos, int x, char *text)
{
    int     col, bit;
    WORD    *byte, line;
    int     pixel;

    for ( ; *text; text++)
    {
        if (TextCode(text))
        {
            text++;
            continue;
        }

        byte = charSetLarge[(int)*text];

        for (col = 0; col < 8; col++, byte++, x++)
        {
            if (x >= 0 && x < WIDTH)
            {
                pixel = pos + x;
                line = *byte;
                for (bit = 0; bit < 16; bit++, pixel += WIDTH, line >>= 1)
                {
                    System_SetPixel(pixel, textInk[line & 1]);
                }
            }
        }
    }
}

void Video_Write(int pos, char *text)
{
    int     col, bit;
    BYTE    *byte, line;
    int     pixel;
    int     width;

    for ( ; *text; text++)
    {
        if (TextCode(text))
        {
            text++;
            continue;
        }

        byte = charSet[(int)*text];
        width = *byte++;

        for (col = 0; col < width; col++, byte++, pos++)
        {
            pixel = pos;
            line = *byte;
            for (bit = 0; bit < 8; bit++, pixel += WIDTH, line >>= 1)
            {
                System_SetPixel(pixel, textInk[line & 1]);
            }
        }
    }
}

void Video_CopyBytes(BYTE *src)
{
    PIXEL   *pixel = &videoPixel[0];
    int     size, bit;
    BYTE    byte;

    for (size = 0; size < 2048; size++, src++)
    {
        byte = *src;
        for (bit = 0; bit < 8; bit++, byte >>= 1, pixel++)
        {
            pixel->point = byte & 1;
        }
    }
}

void Video_CopyColour(BYTE *src, int dest, int size)
{
    for ( ; size > 0; size--, dest++, src++)
    {
        Video_TilePaper(dest, *src >> 4);
        Video_TileInk(dest, *src & 0x0f);
    }
}
