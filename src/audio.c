#include "misc.h"
#include "system.h"
#include "game.h"
#include "video.h"

#include "audio.h"

#define VOLUME      32768 / 4
#define MUSICVOLUME VOLUME / 8
#define SFXVOLUME   VOLUME / 4

#define NCHANNELS   8
#define NMUSIC      5
#define NSFX        3 // sfx channels

#define EV_NOTEOFF  0x00
#define EV_NOTEON   0x10
#define EV_UNDRAW   0x20
#define EV_DRAW     0x30
#define EV_BORDER   0x40
#define EV_END      0x50

#define L           0
#define R           1

#define KEYBOARD    16

typedef struct
{
    int type;
    int pos;
}
KEY;

typedef struct
{
    int     left[3], right[3];    // pseudo stereo/panning effects
    UINT    phase, frequency;
    EVENT   DoPhase;
}
CHANNEL;

typedef struct
{
    int     *pitch;
    int     data;
    int     length;
    CHANNEL *channel;
    int     clock;
    EVENT   DoSfx, DoPlay;
}
SFX;

int     panTable[2][225] =
{
    {
        256, 254, 253, 252, 251, 250, 249, 248, 246, 245, 244, 243, 242, 241, 240, 238,
        237, 236, 235, 234, 233, 232, 230, 229, 228, 227, 226, 225, 224, 222, 221, 220,
        219, 218, 217, 216, 214, 213, 212, 211, 210, 209, 208, 206, 205, 204, 203, 202,
        201, 200, 198, 197, 196, 195, 194, 193, 192, 190, 189, 188, 187, 186, 185, 184,
        182, 181, 180, 179, 178, 177, 176, 174, 173, 172, 171, 170, 169, 168, 166, 165,
        164, 163, 162, 161, 160, 158, 157, 156, 155, 154, 153, 152, 150, 149, 148, 147,
        146, 145, 144, 142, 141, 140, 139, 138, 137, 136, 134, 133, 132, 131, 130, 129,
        128, 126, 125, 124, 123, 122, 121, 120, 118, 117, 116, 115, 114, 113, 112, 110,
        109, 108, 107, 106, 105, 104, 102, 101, 100, 99, 98, 97, 96, 94, 93, 92,
        91, 90, 89, 88, 86, 85, 84, 83, 82, 81, 80, 78, 77, 76, 75, 74,
        73, 72, 70, 69, 68, 67, 66, 65, 64, 62, 61, 60, 59, 58, 57, 56,
        54, 53, 52, 51, 50, 49, 48, 46, 45, 44, 43, 42, 41, 40, 38, 37,
        36, 35, 34, 33, 32, 30, 29, 28, 27, 26, 25, 24, 22, 21, 20, 19,
        18, 17, 16, 14, 13, 12, 11, 10, 9, 8, 6, 5, 4, 3, 2, 1, 0
    },
    {
        0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18,
        19, 20, 21, 22, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35, 36,
        37, 38, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 54,
        56, 57, 58, 59, 60, 61, 62, 64, 65, 66, 67, 68, 69, 70, 72, 73,
        74, 75, 76, 77, 78, 80, 81, 82, 83, 84, 85, 86, 88, 89, 90, 91,
        92, 93, 94, 96, 97, 98, 99, 100, 101, 102, 104, 105, 106, 107, 108, 109,
        110, 112, 113, 114, 115, 116, 117, 118, 120, 121, 122, 123, 124, 125, 126, 128,
        129, 130, 131, 132, 133, 134, 136, 137, 138, 139, 140, 141, 142, 144, 145, 146,
        147, 148, 149, 150, 152, 153, 154, 155, 156, 157, 158, 160, 161, 162, 163, 164,
        165, 166, 168, 169, 170, 171, 172, 173, 174, 176, 177, 178, 179, 180, 181, 182,
        184, 185, 186, 187, 188, 189, 190, 192, 193, 194, 195, 196, 197, 198, 200, 201,
        202, 203, 204, 205, 206, 208, 209, 210, 211, 212, 213, 214, 216, 217, 218, 219,
        220, 221, 222, 224, 225, 226, 227, 228, 229, 230, 232, 233, 234, 235, 236, 237,
        238, 240, 241, 242, 243, 244, 245, 246, 248, 249, 250, 251, 252, 253, 254, 256
    }
};

UINT    frequencyTable[128] =
{
    0x00082d01, 0x0008a976, 0x00092d51, 0x0009b904, 0x000a4d05, 0x000ae9d3, 0x000b8ff4, 0x000c3ff6,
    0x000cfa6f, 0x000dc000, 0x000e914f, 0x000f6f10, 0x00105a02, 0x001152ec, 0x00125aa2, 0x00137208,
    0x00149a0a, 0x0015d3a6, 0x00171fe9, 0x00187fed, 0x0019f4df, 0x001b8000, 0x001d229e, 0x001ede22,
    0x0020b404, 0x0022a5d7, 0x0024b545, 0x0026e410, 0x00293414, 0x002ba74d, 0x002e3fd2, 0x0030ffda,
    0x0033e9c0, 0x00370000, 0x003a453d, 0x003dbc44, 0x00416809, 0x00454baf, 0x00496a8b, 0x004dc820,
    0x00526829, 0x00574e9b, 0x005c7fa4, 0x0061ffb4, 0x0067d380, 0x006e0000, 0x00748a7b, 0x007b7887,
    0x0082d012, 0x008a9760, 0x0092d516, 0x009b9041, 0x00a4d054, 0x00ae9d36, 0x00b8ff49, 0x00c3ff6a,
    0x00cfa6ff, 0x00dc0000, 0x00e914f5, 0x00f6f110, 0x0105a025, 0x01152ec0, 0x0125aa2e, 0x01372082,
    0x0149a0a7, 0x015d3a6d, 0x0171fe92, 0x0187fed4, 0x019f4e00, 0x01b80000, 0x01d229ec, 0x01ede220,
    0x020b404a, 0x022a5d82, 0x024b545c, 0x026e4104, 0x0293414e, 0x02ba74db, 0x02e3fd24, 0x030ffda9,
    0x033e9c02, 0x03700000, 0x03a453da, 0x03dbc43e, 0x04168094, 0x0454bb05, 0x0496a8b6, 0x04dc8208,
    0x052682a0, 0x0574e9b2, 0x05c7fa49, 0x061ffb56, 0x067d37ff, 0x06e00000, 0x0748a7aa, 0x07b78887,
    0x082d0128, 0x08a975ff, 0x092d517a, 0x09b90410, 0x0a4d0533, 0x0ae9d375, 0x0b8ff493, 0x0c3ff69b,
    0x0cfa7011, 0x0dc00000, 0x0e914f54, 0x0f6f110e, 0x105a0250, 0x1152ebfe, 0x125aa2f4, 0x13720820,
    0x149a0a66, 0x15d3a6ea, 0x171fe927, 0x187fed37, 0x19f4e022, 0x1b800000, 0x1d229efa, 0x1ede21c7,
    0x20b404a1, 0x22a5d85c, 0x24b54583, 0x26e41040, 0x2934153e, 0x2ba74d5b, 0x2e3fd24f, 0x30ffdaf7
};

short   musicScore[2][2128] =
{
    {
        EV_BORDER | 0x000, 48, 57, 12, 32, 57, 2,
        EV_BORDER | 0x400, 304, 61, 12, 288, 61, 2,
        EV_BORDER | 0x600, 48, 64, 12, 32, 64, 2,

        EV_BORDER | 0x600, 48, 45, 0, 52, 64, 12, 32, 45, 0, 36, 64, 2,
        EV_BORDER | 0x700, 304, 49, 0, 49, 52, 12, 288, 49, 0, 33, 52, 2,
        EV_BORDER | 0x200, 304, 49, 0, 49, 52, 0, 307, 73, 0, 52, 76, 12, 288, 49, 0, 33, 52, 0, 291, 73, 0, 36, 76, 2,

        EV_BORDER | 0x200, 48, 45, 0, 307, 73, 0, 52, 76, 12, 32, 45, 0, 291, 73, 0, 36, 76, 2,
        EV_BORDER | 0x700, 304, 49, 0, 49, 52, 12, 288, 49, 0, 33, 52, 2,
        EV_BORDER | 0x000, 304, 49, 0, 49, 52, 0, 51, 69, 0, 308, 73, 12, 288, 49, 0, 33, 52, 0, 35, 69, 0, 292, 73, 2,

        EV_BORDER | 0x000, 48, 45, 0, 51, 69, 0, 308, 73, 12, 32, 45, 0, 35, 69, 0, 292, 73, 2,
        EV_BORDER | 0x700, 304, 49, 0, 49, 52, 12, 288, 49, 0, 33, 52, 2,
        EV_BORDER | 0x000, 304, 49, 0, 49, 52, 0, 52, 57, 12, 288, 49, 0, 33, 52, 0, 36, 57, 2,

        EV_BORDER | 0x000, 48, 45, 0, 52, 57, 12, 32, 45, 0, 36, 57, 2,
        EV_BORDER | 0x400, 304, 49, 0, 49, 52, 0, 308, 61, 12, 288, 49, 0, 33, 52, 0, 292, 61, 2,
        EV_BORDER | 0x600, 304, 49, 0, 49, 52, 0, 52, 64, 12, 288, 49, 0, 33, 52, 0, 36, 64, 2,

        EV_BORDER | 0x600, 48, 47, 0, 51, 62, 0, 52, 64, 12, 32, 47, 0, 35, 62, 0, 36, 64, 2,
        EV_BORDER | 0x000, 48, 50, 0, 49, 52, 12, 32, 50, 0, 33, 52, 2,
        EV_BORDER | 0x200, 48, 50, 0, 49, 52, 0, 51, 74, 0, 52, 76, 12, 32, 50, 0, 33, 52, 0, 35, 74, 0, 36, 76, 2,

        EV_BORDER | 0x200, 48, 47, 0, 51, 74, 0, 52, 76, 12, 32, 47, 0, 35, 74, 0, 36, 76, 2,
        EV_BORDER | 0x000, 48, 50, 0, 49, 52, 12, 32, 50, 0, 33, 52, 2,
        EV_BORDER | 0x000, 48, 50, 0, 49, 52, 0, 307, 68, 0, 52, 74, 12, 32, 50, 0, 33, 52, 0, 291, 68, 0, 36, 74, 2,

        EV_BORDER | 0x000, 48, 47, 0, 307, 68, 0, 52, 74, 12, 32, 47, 0, 291, 68, 0, 36, 74, 2,
        EV_BORDER | 0x000, 48, 50, 0, 49, 52, 12, 32, 50, 0, 33, 52, 2,
        EV_BORDER | 0x700, 48, 50, 0, 49, 52, 0, 308, 56, 12, 32, 50, 0, 33, 52, 0, 292, 56, 2,

        EV_BORDER | 0x700, 48, 47, 0, 308, 56, 12, 32, 47, 0, 292, 56, 2,
        EV_BORDER | 0x200, 48, 50, 0, 49, 52, 0, 52, 59, 12, 32, 50, 0, 33, 52, 0, 36, 59, 2,
        EV_BORDER | 0x700, 48, 50, 0, 49, 52, 0, 308, 66, 12, 32, 50, 0, 33, 52, 0, 292, 66, 2,

        EV_BORDER | 0x700, 304, 44, 0, 308, 66, 12, 288, 44, 0, 292, 66, 2,
        EV_BORDER | 0x000, 48, 50, 0, 49, 52, 12, 32, 50, 0, 33, 52, 2,
        EV_BORDER | 0x200, 48, 50, 0, 49, 52, 0, 51, 74, 0, 308, 78, 12, 32, 50, 0, 33, 52, 0, 35, 74, 0, 292, 78, 2,

        EV_BORDER | 0x200, 48, 40, 0, 51, 74, 0, 308, 78, 12, 32, 40, 0, 35, 74, 0, 292, 78, 2,
        EV_BORDER | 0x000, 48, 50, 0, 49, 52, 12, 32, 50, 0, 33, 52, 2,
        EV_BORDER | 0x000, 48, 50, 0, 49, 52, 0, 307, 68, 0, 52, 74, 12, 32, 50, 0, 33, 52, 0, 291, 68, 0, 36, 74, 2,

        EV_BORDER | 0x000, 304, 44, 0, 307, 68, 0, 52, 74, 12, 288, 44, 0, 291, 68, 0, 36, 74, 2,
        EV_BORDER | 0x000, 48, 50, 0, 49, 52, 12, 32, 50, 0, 33, 52, 2,
        EV_BORDER | 0x700, 48, 50, 0, 49, 52, 0, 308, 56, 12, 32, 50, 0, 33, 52, 0, 292, 56, 2,

        EV_BORDER | 0x700, 48, 40, 0, 308, 56, 12, 32, 40, 0, 292, 56, 2,
        EV_BORDER | 0x200, 48, 50, 0, 49, 52, 0, 52, 59, 12, 32, 50, 0, 33, 52, 0, 36, 59, 2,
        EV_BORDER | 0x700, 48, 50, 0, 49, 52, 0, 308, 66, 12, 32, 50, 0, 33, 52, 0, 292, 66, 2,

        EV_BORDER | 0x700, 48, 45, 0, 308, 66, 12, 32, 45, 0, 292, 66, 2,
        EV_BORDER | 0x700, 304, 49, 0, 49, 52, 12, 288, 49, 0, 33, 52, 2,
        EV_BORDER | 0x200, 304, 49, 0, 49, 52, 0, 307, 73, 0, 308, 78, 12, 288, 49, 0, 33, 52, 0, 291, 73, 0, 292, 78, 2,

        EV_BORDER | 0x200, 48, 40, 0, 307, 73, 0, 308, 78, 12, 32, 40, 0, 291, 73, 0, 292, 78, 2,
        EV_BORDER | 0x700, 304, 49, 0, 49, 52, 12, 288, 49, 0, 33, 52, 2,
        EV_BORDER | 0x000, 304, 49, 0, 49, 52, 0, 51, 69, 0, 308, 73, 12, 288, 49, 0, 33, 52, 0, 35, 69, 0, 292, 73, 2,

        EV_BORDER | 0x000, 48, 45, 0, 51, 69, 0, 308, 73, 12, 32, 45, 0, 35, 69, 0, 292, 73, 2,
        EV_BORDER | 0x700, 304, 49, 0, 49, 52, 12, 288, 49, 0, 33, 52, 2,
        EV_BORDER | 0x000, 304, 49, 0, 49, 52, 0, 52, 57, 12, 288, 49, 0, 33, 52, 0, 36, 57, 2,

        EV_BORDER | 0x000, 48, 45, 0, 52, 57, 12, 32, 45, 0, 36, 57, 2,
        EV_BORDER | 0x400, 304, 49, 0, 49, 52, 0, 308, 61, 12, 288, 49, 0, 33, 52, 0, 292, 61, 2,
        EV_BORDER | 0x600, 304, 49, 0, 49, 52, 0, 52, 64, 12, 288, 49, 0, 33, 52, 0, 36, 64, 2,

        EV_BORDER | 0x000, 304, 49, 0, 52, 69, 12, 288, 49, 0, 36, 69, 2,
        EV_BORDER | 0x300, 48, 52, 0, 49, 57, 12, 32, 52, 0, 33, 57, 2,
        EV_BORDER | 0x300, 48, 52, 0, 49, 57, 0, 51, 76, 0, 52, 81, 12, 32, 52, 0, 33, 57, 0, 35, 76, 0, 36, 81, 2,

        EV_BORDER | 0x300, 304, 49, 0, 51, 76, 0, 52, 81, 12, 288, 49, 0, 35, 76, 0, 36, 81, 2,
        EV_BORDER | 0x300, 48, 52, 0, 49, 57, 12, 32, 52, 0, 33, 57, 2,
        EV_BORDER | 0x200, 48, 52, 0, 49, 57, 0, 307, 73, 0, 52, 76, 12, 32, 52, 0, 33, 57, 0, 291, 73, 0, 36, 76, 2,

        EV_BORDER | 0x200, 304, 49, 0, 307, 73, 0, 52, 76, 12, 288, 49, 0, 291, 73, 0, 36, 76, 2,
        EV_BORDER | 0x300, 48, 52, 0, 49, 57, 12, 32, 52, 0, 33, 57, 2,
        EV_BORDER | 0x000, 48, 52, 0, 52, 57, 12, 32, 52, 0, 36, 57, 2,

        EV_BORDER | 0x000, 304, 49, 0, 52, 57, 12, 288, 49, 0, 36, 57, 2,
        EV_BORDER | 0x400, 48, 52, 0, 49, 57, 0, 308, 61, 12, 32, 52, 0, 33, 57, 0, 292, 61, 2,
        EV_BORDER | 0x600, 48, 52, 0, 49, 57, 0, 52, 64, 12, 32, 52, 0, 33, 57, 0, 36, 64, 2,

        EV_BORDER | 0x000, 48, 50, 0, 52, 69, 12, 32, 50, 0, 36, 69, 2,
        EV_BORDER | 0x500, 304, 54, 0, 49, 57, 0, 50, 59, 12, 288, 54, 0, 33, 57, 0, 34, 59, 2,
        EV_BORDER | 0x400, 304, 54, 0, 49, 57, 0, 50, 59, 0, 307, 78, 0, 52, 81, 12, 288, 54, 0, 33, 57, 0, 34, 59, 0, 291, 78, 0, 36, 81, 2,

        EV_BORDER | 0x400, 48, 50, 0, 307, 78, 0, 52, 81, 12, 32, 50, 0, 291, 78, 0, 36, 81, 2,
        EV_BORDER | 0x500, 304, 54, 0, 49, 57, 0, 50, 59, 12, 288, 54, 0, 33, 57, 0, 34, 59, 2,
        EV_BORDER | 0x200, 304, 54, 0, 49, 57, 0, 50, 59, 0, 51, 74, 0, 308, 78, 12, 288, 54, 0, 33, 57, 0, 34, 59, 0, 35, 74, 0, 292, 78, 2,

        EV_BORDER | 0x200, 48, 50, 0, 51, 74, 0, 308, 78, 12, 32, 50, 0, 35, 74, 0, 292, 78, 2,
        EV_BORDER | 0x000, 304, 54, 0, 49, 57, 0, 50, 59, 12, 288, 54, 0, 33, 57, 0, 34, 59, 2,
        EV_BORDER | 0x200, 48, 59, 12, 32, 59, 2,

        EV_BORDER | 0x200, 48, 59, 12, 32, 59, 2,
        EV_BORDER | 0x400, 48, 62, 12, 32, 62, 2,
        EV_BORDER | 0x700, 304, 66, 12, 288, 66, 2,

        EV_BORDER | 0x500, 304, 44, 0, 308, 66, 12, 288, 44, 2,
        EV_BORDER | 0x700, 48, 50, 0, 49, 52, 12, 32, 50, 0, 33, 52, 2,
        EV_BORDER | 0x700, 48, 50, 0, 49, 52, 12, 32, 50, 0, 33, 52, 0, 292, 66, 2,

        EV_BORDER | 0x500, 48, 40, 0, 308, 66, 12, 32, 40, 0, 292, 66, 2,
        EV_BORDER | 0x500, 48, 50, 0, 49, 52, 0, 308, 63, 12, 32, 50, 0, 33, 52, 0, 292, 63, 2,
        EV_BORDER | 0x600, 48, 50, 0, 49, 52, 0, 52, 64, 12, 32, 50, 0, 33, 52, 0, 36, 64, 2,

        EV_BORDER | 0x700, 48, 45, 0, 51, 69, 0, 308, 73, 12, 32, 45, 2,
        EV_BORDER | 0x200, 304, 49, 0, 49, 52, 12, 288, 49, 0, 33, 52, 2,
        EV_BORDER | 0x200, 304, 49, 0, 49, 52, 12, 288, 49, 0, 33, 52, 0, 35, 69, 0, 292, 73, 2,

        EV_BORDER | 0x700, 48, 40, 0, 51, 69, 0, 308, 73, 12, 32, 40, 0, 35, 69, 0, 292, 73, 2,
        EV_BORDER | 0x000, 304, 49, 0, 49, 52, 0, 52, 69, 12, 288, 49, 0, 33, 52, 0, 36, 69, 2,
        EV_BORDER | 0x400, 304, 49, 0, 49, 52, 0, 308, 61, 12, 288, 49, 0, 33, 52, 0, 292, 61, 2,

        EV_BORDER | 0x400, 48, 45, 0, 49, 50, 0, 306, 54, 0, 308, 61, 26, 292, 61, 2,
        EV_BORDER | 0x200, 52, 59, 12, 32, 45, 0, 33, 50, 0, 290, 54, 0, 36, 59, 2,

        EV_BORDER | 0x700, 304, 44, 0, 49, 50, 0, 50, 52, 0, 308, 66, 26, 292, 66, 2,
        EV_BORDER | 0x600, 52, 64, 12, 288, 44, 0, 33, 50, 0, 34, 52, 0, 36, 64, 2,

        EV_BORDER | 0x700, 48, 45, 0, 305, 49, 0, 50, 52, 0, 52, 57, 12, 32, 45, 0, 289, 49, 0, 34, 52, 0, 36, 57, 16,
        EV_BORDER | 0x000, 32, 45, 2,
        EV_BORDER | 0x000, 48, 45, 0, 305, 49, 0, 50, 52, 0, 51, 57, 0, 52, 69, 12, 32, 45, 0, 289, 49, 0, 34, 52, 0, 35, 57, 0, 36, 69, 2,

        EV_BORDER | 0x700, 48, 45, 0, 305, 49, 0, 50, 52, 0, 51, 57, 0, 52, 69, 12, 32, 45, 0, 289, 49, 0, 34, 52, 0, 35, 57, 0, 36, 69, 30,
        EV_END, MUS_STOP
    },
    {
        16, 40, 0, 17, 52, 11, 1, 1, 17, 54, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 55, 11, 1, 1, 17, 57, 11, 0, 0, 1, 1,
        16, 40, 0, 17, 59, 11, 1, 1, 17, 55, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 59, 23, 0, 0, 1, 1,

        16, 40, 0, 17, 58, 11, 1, 1, 17, 54, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 58, 23, 0, 0, 1, 1,
        16, 40, 0, 17, 57, 11, 1, 1, 17, 53, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 57, 23, 0, 0, 1, 1,

        16, 40, 0, 17, 52, 11, 1, 1, 17, 54, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 55, 11, 1, 1, 17, 57, 11, 0, 0, 1, 1,
        16, 40, 0, 17, 59, 11, 1, 1, 17, 55, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 59, 11, 1, 1, 17, 64, 11, 0, 0, 1, 1,

        16, 43, 0, 17, 62, 11, 1, 1, 17, 59, 11, 0, 0, 1, 1,
        16, 50, 0, 17, 55, 11, 1, 1, 17, 59, 11, 0, 0, 1, 1,
        16, 43, 0, 17, 62, 23, 0, 1, 16, 50, 23, 0, 0, 1, 1,

        16, 40, 0, 17, 52, 11, 1, 1, 17, 54, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 55, 11, 1, 1, 17, 57, 11, 0, 0, 1, 1,
        16, 40, 0, 17, 59, 11, 1, 1, 17, 55, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 59, 23, 0, 0, 1, 1,

        16, 40, 0, 17, 58, 11, 1, 1, 17, 54, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 58, 23, 0, 0, 1, 1,
        16, 40, 0, 17, 57, 11, 1, 1, 17, 53, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 57, 23, 0, 0, 1, 1,

        16, 40, 0, 17, 52, 11, 1, 1, 17, 54, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 55, 11, 1, 1, 17, 57, 11, 0, 0, 1, 1,
        16, 40, 0, 17, 59, 11, 1, 1, 17, 55, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 59, 11, 1, 1, 17, 64, 11, 0, 0, 1, 1,

        16, 43, 0, 17, 62, 11, 1, 1, 17, 59, 11, 0, 0, 1, 1,
        16, 50, 0, 17, 55, 11, 1, 1, 17, 59, 11, 0, 0, 1, 1,
        16, 43, 0, 17, 62, 23, 0, 1, 16, 50, 23, 0, 0, 1, 1,

        16, 47, 0, 17, 59, 11, 1, 1, 17, 61, 11, 0, 0, 1, 1,
        16, 54, 0, 17, 63, 11, 1, 1, 17, 64, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 66, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 54, 0, 17, 66, 23, 0, 0, 1, 1,

        16, 43, 0, 17, 67, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 51, 0, 17, 67, 23, 0, 0, 1, 1,
        16, 47, 0, 17, 66, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 50, 0, 17, 66, 23, 0, 0, 1, 1,

        16, 47, 0, 17, 59, 11, 1, 1, 17, 61, 11, 0, 0, 1, 1,
        16, 54, 0, 17, 63, 11, 1, 1, 17, 64, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 66, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 54, 0, 17, 66, 23, 0, 0, 1, 1,

        16, 43, 0, 17, 67, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 51, 0, 17, 67, 23, 0, 0, 1, 1,
        16, 47, 0, 17, 66, 23, 0, 1, 16, 50, 23, 0, 0, 1, 1,

        16, 47, 0, 17, 59, 11, 1, 1, 17, 61, 11, 0, 0, 1, 1,
        16, 54, 0, 17, 63, 11, 1, 1, 17, 64, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 66, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 54, 0, 17, 66, 23, 0, 0, 1, 1,

        16, 43, 0, 17, 67, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 51, 0, 17, 67, 23, 0, 0, 1, 1,
        16, 47, 0, 17, 66, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 50, 0, 17, 66, 23, 0, 0, 1, 1,

        16, 47, 0, 17, 59, 11, 1, 1, 17, 61, 11, 0, 0, 1, 1,
        16, 54, 0, 17, 63, 11, 1, 1, 17, 64, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 66, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 54, 0, 17, 66, 23, 0, 0, 1, 1,

        16, 43, 0, 17, 67, 11, 1, 1, 17, 63, 11, 0, 0, 1, 1,
        16, 51, 0, 17, 67, 23, 0, 0, 1, 1,
        16, 47, 0, 17, 66, 23, 0, 1, 16, 50, 23, 0, 0, 1, 1,

        16, 40, 0, 17, 64, 11, 1, 1, 17, 66, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 67, 11, 1, 1, 17, 69, 11, 0, 0, 1, 1,
        16, 40, 0, 17, 71, 11, 1, 1, 17, 67, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 71, 23, 0, 0, 1, 1,

        16, 40, 0, 17, 70, 11, 1, 1, 17, 66, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 70, 23, 0, 0, 1, 1,
        16, 40, 0, 17, 69, 11, 1, 1, 17, 65, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 69, 23, 0, 0, 1, 1,

        16, 40, 0, 17, 64, 11, 1, 1, 17, 66, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 67, 11, 1, 1, 17, 69, 11, 0, 0, 1, 1,
        16, 40, 0, 17, 71, 11, 1, 1, 17, 67, 11, 0, 0, 1, 1,
        16, 47, 0, 17, 71, 11, 1, 1, 17, 76, 11, 0, 0, 1, 1,

        16, 43, 0, 17, 74, 11, 1, 1, 17, 71, 11, 0, 0, 1, 1,
        16, 50, 0, 17, 67, 11, 1, 1, 17, 71, 11, 0, 0, 1, 1,
        16, 43, 0, 17, 74, 47, 0, 0, 1, 1,
        EV_END, MUS_PLAY
    }
};

KEY     keyPos[96] =
{
    {0, -20}, {1, -20}, {0, -19}, {1, -19}, {0, -18}, {0, -17}, {1, -17}, {0, -16}, {1, -16}, {0, -15}, {1, -15}, {0, -14}, // 0
    {0, -13}, {1, -13}, {0, -12}, {1, -12}, {0, -11}, {0, -10}, {1, -10}, {0, -9}, {1, -9}, {0, -8}, {1, -8}, {0, -7}, // 12
    {0, -6}, {1, -6}, {0, -5}, {1, -5}, {0, -4}, {0, -3}, {1, -3}, {0, -2}, {1, -2}, {0, -1}, {1, -1}, {0, 0}, // 24
    {0, 1}, {1, 1}, {0, 2}, {1, 2}, {0, 3}, {0, 4}, {1, 4}, {0, 5}, {1, 5}, {0, 6}, {1, 6}, {0, 7}, // 36
    {0, 8}, {1, 8}, {0, 9}, {1, 9}, {0, 10}, {0, 11}, {1, 11}, {0, 12}, {1, 12}, {0, 13}, {1, 13}, {0, 14}, // 48
    {0, 15}, {1, 15}, {0, 16}, {1, 16}, {0, 17}, {0, 18}, {1, 18}, {0, 19}, {1, 19}, {0, 20}, {1, 20}, {0, 21}, // 60
    {0, 22}, {1, 22}, {0, 23}, {1, 23}, {0, 24}, {0, 25}, {1, 25}, {0, 26}, {1, 26}, {0, 27}, {1, 27}, {0, 28}, // 72
    {0, 29}, {1, 29}, {0, 30}, {1, 30}, {0, 31}, {0, 32}, {1, 32}, {0, 33}, {1, 33}, {0, 34}, {1, 34}, {0, 35}  // 84
};

int     sfxPitch[6][226] =
{
    {84, 81, 78, 75, 72, 69, 66, 63, 60, 57, 54, 51, 48, 45, 42, 39, 0},
    {84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 0},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 0},
    {88, 87, 87, 87, 87, 87, 87, 87, 87, 86, 86, 86, 86, 86, 86, 86, 86, 85, 85, 85, 85, 85, 85, 85, 85, 84, 84, 84, 84, 84, 84, 84, 84, 83, 83, 83, 83, 83, 83, 83, 83, 82, 82, 82, 82, 82, 82, 82, 82, 81, 81, 81, 81, 81, 81, 81, 81, 80, 80, 80, 80, 80, 80, 80, 80, 79, 79, 79, 79, 79, 79, 79, 79, 78, 78, 78, 78, 78, 78, 78, 78, 77, 77, 77, 77, 77, 77, 77, 77, 76, 76, 76, 76, 76, 76, 76, 76, 75, 75, 75, 75, 75, 75, 75, 75, 74, 74, 74, 74, 74, 74, 74, 74, 73, 73, 73, 73, 73, 73, 73, 73, 72, 72, 72, 72, 72, 72, 72, 72, 71, 71, 71, 71, 71, 71, 71, 71, 70, 70, 70, 70, 70, 70, 70, 70, 69, 69, 69, 69, 69, 69, 69, 69, 68, 68, 68, 68, 68, 68, 68, 68, 67, 67, 67, 67, 67, 67, 67, 67, 66, 66, 66, 66, 66, 66, 66, 66, 65, 65, 65, 65, 65, 65, 65, 65, 64, 64, 64, 64, 64, 64, 64, 64, 63, 63, 63, 63, 63, 63, 63, 63, 62, 62, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 61, 61, 61, 61, 60, 60, 60, 60, 60, 60, 60, 60, 0},
    {36, 42, 48, 54, 60, 66, 72, 78, 84, 0},
    {0}
};

CHANNEL audioChannel[NCHANNELS] =
{
    {{0, 0, 0}, {0, 0, 0}, 0, 0, DoNothing},
    {{0, 0, 0}, {0, 0, 0}, 0, 0, DoNothing},
    {{0, 0, 0}, {0, 0, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing},
    {{MUSICVOLUME, -MUSICVOLUME, 0}, {-MUSICVOLUME, MUSICVOLUME, 0}, 0, 0, DoNothing}
};

CHANNEL *musicChannel[NMUSIC] = // mapping
{
    &audioChannel[3],
    &audioChannel[4],
    &audioChannel[5],
    &audioChannel[6],
    &audioChannel[7]
};

SFX     sfxInfo[NSFX] =
{
    {sfxPitch[SFX_NONE], 0, 0, &audioChannel[0], 0, DoNothing, DoNothing},
    {sfxPitch[SFX_NONE], 0, 0, &audioChannel[1], 0, DoNothing, DoNothing},
    {sfxPitch[SFX_NONE], 0, 0, &audioChannel[2], 0, DoNothing, DoNothing}
};

int     musicIndex = 0;
int     musicClock = 0, musicDelta;
short   *curMusic;

int     audioMusicPlaying = MUS_STOP;
int     audioSamples = 0;
int     musicChannels = 0;

int     sfxClock = 0;

TIMER   timerPhase, timerTick;
int     phaseRate;
CHANNEL *curChannel;
SFX     *curSfx;

EVENT   NoteOff[2] = {DoNothing, DoNothing};
EVENT   NoteOn[2] = {DoNothing, DoNothing};

void DoPhase()
{
    UINT    phase = curChannel->phase >> 31;

    curChannel->left[2] = curChannel->left[phase];
    curChannel->right[2] = curChannel->right[phase];

    curChannel->phase += curChannel->frequency * phaseRate;
}

void DoChannelOff()
{
    curChannel->DoPhase = DoNothing;
    curChannel->left[2] = 0;
    curChannel->right[2] = 0;
}

void ChannelStereo(CHANNEL *channel, int left, int right)
{
    left = SFXVOLUME * left >> 8;
    right = SFXVOLUME * right >> 8;

    channel->left[0] = left;
    channel->right[0] = right;
    channel->left[1] = -left;
    channel->right[1] = -right;
}

void ChannelPan(CHANNEL *channel, int pan)
{
    ChannelStereo(channel, panTable[L][pan], panTable[R][pan]);
}

void DoSfxOff()
{
    curSfx->DoSfx = DoNothing;
    curSfx->channel->DoPhase = DoChannelOff;
}

void DoSfxOn()
{
    curSfx->channel->DoPhase = DoPhase;
    curSfx->DoSfx = curSfx->DoPlay;
    curSfx->DoSfx();
}

void DoSfxPlay()
{
    curSfx->channel->frequency = frequencyTable[*curSfx->pitch];
    curSfx->clock += curSfx->length;

    curSfx->pitch++;
    if (*curSfx->pitch > 0)
    {
        return;
    }

    curSfx->DoSfx = DoSfxOff;
}

void DoSfxAir()
{
    ChannelPan(curSfx->channel, curSfx->data);
    DoSfxPlay();

    curSfx->data--;
}

void DoSfxVictory()
{
    DoSfxPlay();
    if (*curSfx->pitch == 0 && --curSfx->data)
    {
        curSfx->pitch = sfxPitch[SFX_VICTORY];
        curSfx->DoSfx = DoSfxVictory;
    }
}

void DoSfxMiner()
{
    curSfx->channel->DoPhase = DoPhase;
    curSfx->clock += curSfx->length;
    curSfx->DoSfx = DoSfxOff;
}

// not much reason to lock the audio callback prior to setting up any sfx
void Audio_MinerSfx(int note, int length)
{
    sfxInfo[0].channel->frequency = frequencyTable[note];
    sfxInfo[0].clock = sfxClock;
    ChannelPan(sfxInfo[0].channel, minerX - 8);
    sfxInfo[0].length = length;
    sfxInfo[0].DoSfx = DoSfxMiner;
}

void Audio_Sfx(int sfx)
{
    if (sfx != SFX_KONG) // kill sfx
    {
        sfxInfo[0].DoSfx = DoSfxOff;
        sfxInfo[1].DoSfx = DoSfxOff;
        curSfx = &sfxInfo[2];
    }
    else
    {
        curSfx = &sfxInfo[1];
    }

    switch (sfx)
    {
      case SFX_KONG:
        sfxInfo[1].length = 5;
        sfxInfo[1].pitch = sfxPitch[sfx];
        ChannelPan(sfxInfo[1].channel, 120); // 120 = robot[0].x + 8
        sfxInfo[1].DoPlay = DoSfxPlay;
        break;

      case SFX_AIR:
        sfxInfo[2].length = 1;
        sfxInfo[2].pitch = &sfxPitch[sfx][224 - gameAirOld];
        sfxInfo[2].data = gameAirOld;
        sfxInfo[2].DoPlay = DoSfxAir;
        break;

      case SFX_VICTORY:
        sfxInfo[2].length = 1;
        sfxInfo[2].pitch = sfxPitch[sfx];
        sfxInfo[2].data = 50;
        ChannelStereo(sfxInfo[2].channel, 256, -256);
        sfxInfo[2].DoPlay = DoSfxVictory;
        break;

      case SFX_DIE:
        sfxInfo[2].length = 1;
        sfxInfo[2].pitch = sfxPitch[sfx];
        ChannelPan(sfxInfo[2].channel, minerX - 8);
        sfxInfo[2].DoPlay = DoSfxPlay;
        break;

      case SFX_GAMEOVER:
        sfxInfo[2].length = 2;
        sfxInfo[2].pitch = sfxPitch[sfx];
        ChannelStereo(sfxInfo[2].channel, 256, -256);
        sfxInfo[2].DoPlay = DoSfxPlay;
        break;
    }

    curSfx->DoSfx = DoSfxOn;
    curSfx->clock = sfxClock;
}

void MusicReset()
{
    int index;

    for (index = 0; index < NMUSIC; index++)
    {
        musicChannel[index]->DoPhase = DoChannelOff;
    }

    curMusic = musicScore[musicIndex];
    musicDelta = 0;
    musicClock = 0;
}

void DoNoteOffWhite()
{
    Video_PianoKey(KEYBOARD * 32 + keyPos[*curMusic].pos, 0x7);
}

void DoNoteOffBlack()
{
    Video_TilePaper(KEYBOARD * 32 + keyPos[*curMusic].pos, 0x0);
}

void DoNoteOnWhite()
{
    Video_PianoKey(KEYBOARD * 32 + keyPos[*curMusic].pos, 0x5);
}

void DoNoteOnBlack()
{
    Video_TilePaper(KEYBOARD * 32 + keyPos[*curMusic].pos, 0x2);
}

void Audio_Callback(short *buffer, int length)
{
    short   left, right;
    int     channel, data, time;

    length /= 2;

    while (length)
    {
        if (audioSamples == 0)
        {
            audioSamples = Timer_Update(&timerTick);

            if (audioMusicPlaying)
            {
                if (musicDelta == musicClock)
                {
                    do
                    {
                        data = *curMusic++;
                        channel = data & 0x0f; 
                        switch (data & 0xf0)
                        {
                          case EV_BORDER:
                            System_Border(data >> 8);
                            time = 0;
                            continue;

                          case EV_DRAW:
                            NoteOn[data >> 8]();
                            // FALLTHRU
                          case EV_NOTEON:
                            musicChannel[channel]->frequency = frequencyTable[*curMusic++];
                            musicChannel[channel]->DoPhase = DoPhase;
                            break;

                          case EV_UNDRAW:
                            NoteOff[data >> 8]();
                            curMusic++;
                            // FALLTHRU
                          case EV_NOTEOFF:
                            musicChannel[channel]->DoPhase = DoChannelOff;
                            break;

                          case EV_END:
                            audioMusicPlaying = *curMusic;
                            MusicReset();
                            time = audioMusicPlaying ? 0 : 1;
                            continue;
                        }

                        time = *curMusic++;
                        musicDelta += time;
                    }
                    while (time == 0);
                }

                musicClock++;
            }

            curSfx = &sfxInfo[0];
            for (channel = 0; channel < NSFX; channel++, curSfx++)
            {
                if (curSfx->clock == sfxClock)
                {
                    curSfx->DoSfx();
                }
            }

            sfxClock++;
        }

        while (audioSamples && length)
        {
            phaseRate = Timer_Update(&timerPhase);

            left = right = 0;

            curChannel = &audioChannel[0];
            for (channel = 0; channel < musicChannels; channel++, curChannel++)
            {
                curChannel->DoPhase();
                left += curChannel->left[2];
                right += curChannel->right[2];
            }

            *buffer++ = left;
            *buffer++ = right;

            audioSamples--;
            length--;
        }
    }
}

void Audio_Play(int playing)
{
    audioMusicPlaying = playing;
    musicChannels = playing == MUS_PLAY ? NCHANNELS : NSFX;
}

void Audio_Music(int music, int playing)
{
    System_LockAudio();

    musicIndex = music;
    MusicReset();

    sfxInfo[0].DoSfx = DoSfxOff;
    sfxInfo[1].DoSfx = DoSfxOff;
    sfxInfo[2].DoSfx = DoSfxOff;

    audioSamples = 0;
    Audio_Play(playing);

    System_UnlockAudio();
}

void Audio_Init()
{
    Timer_Set(&timerPhase, 65536, SAMPLERATE);
    Timer_Set(&timerTick, SAMPLERATE, TICKRATE);

    NoteOff[0] = DoNoteOffWhite;
    NoteOff[1] = DoNoteOffBlack;
    NoteOn[0] = DoNoteOnWhite;
    NoteOn[1] = DoNoteOnBlack;
}
