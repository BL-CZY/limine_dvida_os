#include "terminal.h"
#include "../../lib/std/stdio.h"
#include "../../lib/time/time.h"

io_state_t current_io_state;
uint64_t terminal_color_buffer[50][160];
char terminal_text_buffer[50][160];
uint64_t frame_buffer_width;
uint64_t frame_buffer_height;
uint32_t *frame_buffer_addr;
uint64_t terminal_width;
uint64_t terminal_height;
size_t current_row;
size_t current_col;
size_t cursor_row;
size_t cursor_col;
uint32_t bg_color;
uint32_t fg_color;
//1 means on, 0 means off
int cursor_blink_phase;
int cursor_blink_speed;
int internal_counter = 0;


void update_cursor_blink() {
    if(cursor_blink_phase == 1 && internal_counter == cursor_blink_speed) {
        internal_counter = 0;
        cursor_blink_phase = 0;
        set_cursor(cursor_row, cursor_col, 1);
    } else if(internal_counter == cursor_blink_speed) {
        internal_counter = 0;
        cursor_blink_phase = 1;
        set_cursor(cursor_row, cursor_col, 0);
    } else {
        ++internal_counter;
    }
}

void render_char(char character, int row, int col) {
    //get the character
    size_t font_offset = character * 16;
    //render the character
    for(int i = 0; i < 16; ++i)
    {
        for(int j = 0; j < 8; ++j)
        {
            //location -> pixel
            size_t pixel_offset = (row * 16 + i) * frame_buffer_width + col * 8 + j;

            //if the bit is 1, then it's fg, otherwise bg
            if(((builtin_font[font_offset + i] >> (7 - j)) & 0x1) == 0x1) {
                *(uint32_t *)(frame_buffer_addr + pixel_offset) = fg_color;
            } else {
                *(uint32_t *)(frame_buffer_addr + pixel_offset) = bg_color;
            }
        }
    }
}

void terminal_init(void *addr, const uint64_t width, const uint64_t height, const int blink_speed) {
    //basic info part
    frame_buffer_addr = (uint32_t *)addr;
    frame_buffer_width = width;
    frame_buffer_height = height;
    terminal_width = width/8;
    terminal_height = height/16;
    current_row = 0;
    current_col = 0;
    bg_color = 0x000000;
    fg_color = 0xFFFFFF;
    cursor_row = 0;
    cursor_col = 0;

    //cursor part
    set_cursor(current_row, current_col, 0);
    cursor_blink_phase = 1;
    cursor_blink_speed = blink_speed;
    register_update_function(update_cursor_blink);

    //buffer part
    for(int i = 0; i < 50; ++i) {
        for(int j = 0; j < 160; ++j)
        {
            terminal_color_buffer[i][j] = 0xFFFFFF00000000;
            terminal_text_buffer[i][j] = '\0';
        }
    }
}

void terminal_clear() {
    for(int i = 0; i < (int)terminal_height; ++i) {
        for(int j = 0; j < (int)terminal_width; ++j) {
            terminal_text_buffer[i][j] = '\0';
            terminal_color_buffer[i][j] = 0xFFFFFF00000000;
            render_char('\0', i, j);
        }
    }
    current_row = 0;
    current_col = 0;
    set_cursor(current_col, current_col, 0);
}

void terminal_set_resolution(const uint64_t width, const uint64_t height) {
    terminal_width = width/8;
    terminal_height = height/16;
}

size_t strlen(char* str) {
    size_t size = 0;
    while(str[size])
    {
        ++size;
    }
    return size;
}

void set_cursor(int row, int col, int remove) {
    //check if the position goes out of the buffer
    uint64_t offset = row * terminal_width + col;
    if(offset >= terminal_width * terminal_height)
    {
        return;
    }

    //! USE OF GOTO TO SKIP THE REMOVE CURSOR PART IF NOT TOLD SO
    if(!remove) {
        goto render;
    }

    //remove the cursor
    for(int i = 0; i < 16; ++i)
    {
        for(int j = 0; j < 8; ++j)
        {
            //location -> pixel
            size_t pixel_offset = (cursor_row * 16 + i) * frame_buffer_width + cursor_col * 8 + j;
            *(uint32_t *)(frame_buffer_addr + pixel_offset) = bg_color;
        }
    }

    return;

    render:
    
    //! USE OF GOTO TO SKIP THE rendering part if the cursor is now invisible
    if(cursor_blink_phase == 0) {
        goto set_cursor_position;
    }
    //render the cursor
    for(int i = 0; i < 16; ++i)
    {
        for(int j = 0; j < 8; ++j)
        {
            //location -> pixel
            size_t pixel_offset = (current_row * 16 + i) * frame_buffer_width + current_col * 8 + j;
            *(uint32_t *)(frame_buffer_addr + pixel_offset) = 0xFFFFFF;
        }
    }

    set_cursor_position:

    cursor_row = current_row;
    cursor_col = current_col;
}

void render_buffer() {
    for(int i = 0; i < (int)terminal_height; ++i) {
        for(int j = 0; j < (int)terminal_width; ++j) {
            bg_color = (uint32_t)(terminal_color_buffer[i][j]);
            fg_color = (uint32_t)((terminal_color_buffer[i][j] >> 32));
            render_char(terminal_text_buffer[i][j], i, j);
        }
    }
    current_row = terminal_height - 1;
    current_col = 0;
    set_cursor(current_row, current_col, 0);
}

void terminal_moveup() {
    for(int i = 1; i < (int)terminal_height; ++i) {
        for(int j = 0; j < (int)terminal_width; ++j) {
            terminal_color_buffer[i - 1][j] = terminal_color_buffer[i][j];
            terminal_text_buffer[i - 1][j] = terminal_text_buffer[i][j];
        }
    }
    for(int i = 0; i < (int)terminal_width; ++i) {
        terminal_color_buffer[terminal_height - 1][i] = 0xFFFFFF00000000;
        terminal_text_buffer[terminal_height - 1][i] = '\0';
    }

    render_buffer();
}

void terminal_advance() {
    if(++current_col == terminal_width) {
        current_col = 0;
        if(++current_row == terminal_height) {
            current_row = terminal_height - 1;
            terminal_moveup();
        }
    }
}

void terminal_back() {
    if(current_col == 0) {
        current_col = terminal_width - 1;
        if(current_row == 0) {
            current_row = 0;
        } else {
            --current_row;
        }
    } else {
        --current_col;
    }
}

void terminal_newline() {
    current_col = 0;
    if(++current_row == terminal_height) {
        current_row = terminal_height - 1;
        terminal_moveup();
    }
}

void terminal_putchar(char character) {
    //get the character
    size_t font_offset = character * 16;
    
    //render the character
    for(int i = 0; i < 16; ++i)
    {
        for(int j = 0; j < 8; ++j)
        {
            //location -> pixel
            size_t pixel_offset = (current_row * 16 + i) * frame_buffer_width + current_col * 8 + j;

            //if the bit is 1, then it's fg, otherwise bg
            if(((builtin_font[font_offset + i] >> (7 - j)) & 0x1) == 0x1) {
                *(uint32_t *)(frame_buffer_addr + pixel_offset) = fg_color;
            } else {
                *(uint32_t *)(frame_buffer_addr + pixel_offset) = bg_color;
            }
        }
    }

    terminal_color_buffer[current_row][current_col] = ((uint64_t)(fg_color) << 32) | ((uint64_t)(bg_color));
    terminal_text_buffer[current_row][current_col] = character;
    terminal_advance();
}

void kprintf_no_format(char *format) {
    for(size_t i = 0; i < strlen(format); ++i) {
        terminal_putchar(format[i]);
    }
}

void kprintf(char *format, ...) {
    set_cursor(current_row, current_col, 1);
    size_t length = strlen(format);
    bg_color = 0x000000;
    fg_color = 0xFFFFFF;
    va_list args;
    va_start(args, format);
    enum PrintState print_state = Print;
    
    for(size_t i = 0; i < length; ++i) {
        char c = format[i];
        switch(print_state) {
            case Print:
                switch(c) {
                    case '%':
                        print_state = Format;
                        break;

                    case '\n':
                        terminal_newline();
                        break;

                    default:
                        terminal_putchar(c);
                        break;
                }
                break;
            
            case Format:
                switch(c) {
                    case '%':
                        terminal_putchar(c);
                        print_state = Print;
                        break;

                    case 'c':
                        // color, there are two options, foreground and background
                        print_state = Color;
                        break;

                    case 's':
                        // string
                        char *str = va_arg(args, char *);
                        kprintf_no_format(str);
                        break;

                    case 'u':
                        // print unsigned number
                        uint64_t num = (uint64_t)va_arg(args, unsigned long long int);
                        char buffer[20];
                        int ind = 0;
                        if(num == 0)
                        {
                            terminal_putchar('0');
                        }
                        while(num != 0)
                        {
                            buffer[ind] = '0' + num % 10;
                            num /= 10;
                            ++ind;
                        }
                        --ind;
                        while(ind >= 0)
                        {
                            terminal_putchar(buffer[ind]);
                            --ind;
                        }
                        print_state = Print;
                        break;


                    case 'x':
                        // hexadecimal
                        kprintf_no_format("0x");
                        uint64_t num1 = (uint64_t)va_arg(args, unsigned long long int);
                        char buffer1[20];
                        int ind1 = 0;
                        if(num1 == 0)
                        {
                            terminal_putchar('0');
                        }
                        while(num1 != 0)
                        {
                            if(num1 % 16 >= 10) {
                                buffer1[ind1] = 'A' + num1 % 16 - 10;
                            } else {
                                buffer1[ind1] = '0' + num1 % 16;
                            }
                            num1 /= 16;
                            ++ind1;
                        }
                        --ind1;
                        while(ind1 >= 0)
                        {
                            terminal_putchar(buffer1[ind1]);
                            --ind1;
                        }
                        print_state = Print;
                        break;
                    
                    case 'b':
                        // binary
                        kprintf_no_format("0b");
                        uint64_t num2 = (uint64_t)va_arg(args, unsigned long long int);
                        char buffer2[64];
                        int ind2 = 0;
                        if(num2 == 0)
                        {
                            terminal_putchar('0');
                        }
                        while(num2 != 0)
                        {
                            buffer2[ind2] = '0' + num2 % 2;
                            num2 /= 2;
                            ++ind2;
                        }
                        --ind2;
                        while(ind2 >= 0)
                        {
                            terminal_putchar(buffer2[ind2]);
                            --ind2;
                        }
                        print_state = Print;
                        break;

                    default:
                        print_state = Print;
                        break;
                }
                break;

            case Color:
                switch(c) {
                    case 'f':
                        // set foreground color
                        uint32_t color1 = (uint32_t)va_arg(args, unsigned int);
                        fg_color = color1;
                        print_state = Print;
                        break;

                    case 'b':
                        // set background color
                        uint32_t color2 = (uint32_t)va_arg(args, unsigned int);
                        bg_color = color2;
                        print_state = Print;
                        break;

                    default:
                        print_state = Print;
                        break;
                }

            default:
                break;
        }
    }

    bg_color = 0x000000;
    fg_color = 0xFFFFFF;

    set_cursor(current_row, current_col, 0);
}

void erasef(size_t num) {
    for(size_t i = 0; i < num; ++i) {
        if(current_row == 0 && current_col == 0) {
            return;
        }
        terminal_back();
        fg_color = 0xFFFFFF;
        bg_color = 0x000000;
        render_char('\0', current_row, current_col);
        set_cursor(current_row, current_col, 1);
    }
}