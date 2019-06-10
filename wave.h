/*
http://microsin.net/programming/pc/wav-format.html
http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
http://www.dragonwins.com/domains/getteched/wav/index.htm
*/

typedef char                byte;  // 1 byte \ 8 bit 
typedef short int           word;  // 2 byte \ 16 bit
typedef unsigned int        dword; // 4 byte \ 32 bit

struct riff_chunk
{
    byte     id[4];             // 4b "RIFF" string  >|
    dword    size;              // 4b                 |-> 12 byte
    byte     type[4];           // 4b "WAVE" string  >|
};

struct fmt_chunk 
{
    byte     id[4];             // 4b "fmt" string
    dword    size;              // 4b _____
    word     compression;       // 2b
    word     chanels;           // 2b
    dword    sample_rate;       // 4b _____
    dword    byte_per_sec;      // 4b
    word     block_align;       // 2b
    word     bit_per_sample;    // 2b _____
    word     extra_format_size; // 2b _____
    byte*    extra_format_data; // 8b _____
    dword    number_of_blocks;  // 4b _____
};

struct data_chunk
{
    byte     id[4];             // 4 "data" string
    dword    size;              // 4
};

int dump(word*, dword*);