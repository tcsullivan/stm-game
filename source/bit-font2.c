/*
	Copyright 2022 Bga <bga.email@gmail.com>
	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

        Modified by tcsullivan:
         - compacted code to only what is required
         - changed ordering of font bitmap data for easier rendering
         - added functional interface
*/

#include <stdint.h>

#define MAKE_GLYPH(hi, lo) (((hi * 3) << 8) | lo * 3)

const uint8_t halfGlyphData[] = {
    0x00, 0x00, 0x00,
    0x0C, 0x03, 0x00,
    0x0C, 0xC3, 0x30,
    0xF3, 0x3C, 0xCF,
    0xFF, 0x3C, 0xCF,
    0xF3, 0x3C, 0xFF,
    0xFF, 0x3C, 0xFF,
    0x3F, 0x0C, 0xFF,
    0xFF, 0x30, 0xFC,
    0xCF, 0x30, 0xFC,
    0xC3, 0x30, 0xFC,
    0x3F, 0x0C, 0xC3,
    0xFF, 0x30, 0x0C,
    0x0C, 0xC3, 0xFC,
    0xFF, 0x3F, 0x87,
    0xE1, 0xFC, 0xFF,
    0xF3, 0xEC, 0x31,
    0xCC, 0x3F, 0xCF,
    0xF3, 0x6C, 0xF3,
    0xBC, 0x3D, 0xCF,
    0x37, 0x0C, 0xFF,
    0xFE, 0x3C, 0xFB,
    0x30, 0x0C, 0xC3,
    0x30, 0x3C, 0xFF,
};

const uint16_t glyphData[] = {
    /* d0 */ MAKE_GLYPH(5, 4),
    /* d1 */ MAKE_GLYPH(2, 2),
    /* d2 */ MAKE_GLYPH(8, 11),
    /* d3 */ MAKE_GLYPH(9, 12),
    /* d4 */ MAKE_GLYPH(4, 10),
    /* d5 */ MAKE_GLYPH(7, 8),
    /* d6 */ MAKE_GLYPH(7, 4),
    /* d7 */ MAKE_GLYPH(9, 2),
    /* d8 */ MAKE_GLYPH(6, 4),
    /* d9 */ MAKE_GLYPH(6, 12),
    /* space */ MAKE_GLYPH(0, 0),
    /* dot */ MAKE_GLYPH(0, 1),
    /* comma */ MAKE_GLYPH(0, 2),
    /* colon */ MAKE_GLYPH(1, 1),
    /* asterick */ MAKE_GLYPH(2, 1),
    /* questionMark */ MAKE_GLYPH(9, 1),
    /* quote */ MAKE_GLYPH(2, 0),
    /* doubleQuote */ MAKE_GLYPH(3, 0),
    /* lowTriangle */ MAKE_GLYPH(1, 4),
    /* A */ MAKE_GLYPH(6, 3),
    /* B */ MAKE_GLYPH(21, 21),
    /* C */ MAKE_GLYPH(23, 4),
    /* D */ MAKE_GLYPH(18, 19),
    /* E */ MAKE_GLYPH(7, 7),
    /* F */ MAKE_GLYPH(7, 22),
    /* G */ MAKE_GLYPH(20, 4),
    /* H */ MAKE_GLYPH(4, 3),
    /* I */ MAKE_GLYPH(2, 2),
    /* J */ MAKE_GLYPH(10, 4),
    /* K */ MAKE_GLYPH(19, 18),
    /* L */ MAKE_GLYPH(22, 11),
    /* M */ MAKE_GLYPH(14, 3),
    /* N */ MAKE_GLYPH(18, 3),
    /* O */ MAKE_GLYPH(5, 4),
    /* P */ MAKE_GLYPH(6, 22),
    /* Q */ MAKE_GLYPH(5, 17),
    /* R */ MAKE_GLYPH(6, 18),
    /* S */ MAKE_GLYPH(7, 8),
    /* T */ MAKE_GLYPH(13, 2),
    /* U */ MAKE_GLYPH(3, 4),
    /* V */ MAKE_GLYPH(3, 19),
    /* W */ MAKE_GLYPH(3, 15),
    /* X */ MAKE_GLYPH(17, 16),
    /* Y */ MAKE_GLYPH(4, 2),
    /* Z */ MAKE_GLYPH(9, 11),
    /* caret */ MAKE_GLYPH(16, 0),
    /* squareBracketOpen */ MAKE_GLYPH(23, 17),
    /* squareBracketClose */ MAKE_GLYPH(10, 12)
};

const char glyphKeys[47] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	' ',
	'.',
	',',
	':',
	'!',
	'?',
	'\'',
	'"',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'^',
	'[',
	']'
};

const uint8_t *bitfontGetBitmapLo(unsigned int gindex)
{
    if (gindex < sizeof(glyphKeys))
        return halfGlyphData + (glyphData[gindex] & 0x00FF);
    else
        return 0;
}

const uint8_t *bitfontGetBitmapHi(unsigned int gindex)
{
    if (gindex < sizeof(glyphKeys))
        return halfGlyphData + ((glyphData[gindex] >> 8) & 0xFF);
    else
        return 0;
}

unsigned int bitfontGetGlyph(char c)
{
    for (unsigned int i = 0; i < sizeof(glyphKeys); ++i) {
        if (glyphKeys[i] == c)
            return i;
    }

    return 15; // Glyph for '?'
}
