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

#ifndef BIT_FONT2_H
#define BIT_FONT2_H

#include <stdint.h>

/**
 * Looks up the glyph number for the given character.
 * Returns glyph for '?' if character is not supported.
 */
unsigned int bitfontGetGlyph(char c);

/**
 * Returns bitmap for "hi" portion of given glyph.
 * Returns NULL if glyph index is too large.
 */
const uint8_t *bitfontGetBitmapHi(unsigned int gindex);

/**
 * Returns bitmap for "lo" portion of given glyph.
 * Returns NULL if glyph index is too large.
 */
const uint8_t *bitfontGetBitmapLo(unsigned int gindex);

#endif // BIT_FONT2_H

