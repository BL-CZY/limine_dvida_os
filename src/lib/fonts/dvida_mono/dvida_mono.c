#include "dvida_mono.h"

font_char_t dvida_mono[128] = 
{
    //the first 32 are empty
    #pragma region empty
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    (font_char_t){},
    #pragma endregion empty
    //char: space
    (font_char_t){8, 16,   {00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,}},
    //char: !
    (font_char_t){8, 16,   {00000000,
                            00000000,
                            00011000,
                            00011000,
                            00011000,
                            00011000,
                            00011000,
                            00011000,
                            00011000,
                            00011000,
                            00011000,
                            00000000,
                            00011000,
                            00111100,
                            00011000,
                            00000000,}},

    //char: "
    (font_char_t){8, 16,   {00000000,
                            00000000,
                            01100110,
                            01100110,
                            01100110,
                            01100110,
                            01100110,
                            01100110,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,
                            00000000,}},
};