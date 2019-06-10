#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include "wave.h"

byte buf[5]; // 4 char string buffer
char filename[200]; //
FILE *fp;
FILE *pcm_data;

int main(int argc, char *argv[])
{
    strcat(filename, argv[1]);
    struct riff_chunk riff = {"RIFF", 0, "WAVE"};
    struct fmt_chunk fmt = {"fmt "};
    struct data_chunk data = {"data"};

    if ((fp = fopen(argv[1], "rb"))==NULL) {
        printf("Can't open the file. Exit.\n");
        return 1;
    }
    
    // Reading RIFF section
    if (fread(&riff.id, sizeof(byte), 12, fp)!= 12)  {
        printf("Can't read RIFF chunk or EOF is met. Exit.\n");
        return 1;
    } else {
        memccpy(buf, riff.id, '\0', 4);
        if (strcmp(buf, "RIFF")!=0) {
            printf("File format is not RIFF. Exit.\n");
            return 1;
        }
        memccpy(buf, riff.type, '\0', 4);
        if (strcmp(buf, "WAVE")!=0) {
            printf("File format is not WAVE. Exit.\n");
            return 1;
        }
    };

    // Reading fmt.id and fmt.size
    if (fread(&fmt, sizeof(byte), 8, fp)!=8) {
        printf("Can't read fmt chunk or EOF is met. Exit.\n");
        return 1;
    } else {
        memccpy(buf, fmt.id, '\0', 4);
        if (strcmp(buf, "fmt ")!=0) {
            printf("File have no fmt chunk. Exit.\n");
            return 1;
        }
    }
    
    // Reading fmt Sample Format Info
    if (fread(&fmt.compression, sizeof(byte), fmt.size, fp) != fmt.size) {
        printf("Can't read Sample Format Info in fmt chunk or EOF is met. Exit.\n");
        return 1;
    }

    printf("Compression: %d\n", fmt.compression);
    printf("Channels: %d\n", fmt.chanels);
    printf("Sample Rate: %d\n", fmt.sample_rate);
    printf("Bit Rate: %d\n", fmt.bit_per_sample);

    // Reading data/some chunk
    if (fread(&data, sizeof(byte), 8, fp)!=8) {
        printf("Error of reading data chunk. Exit.\n");
        return 1;
    } else {
        while (memccpy(buf, data.id, '\0', 4), strcmp(buf, "data")!=0) {
            fseek(fp, data.size, 1); // перемещаем указатель файла на конец чанка (его размер)
            fread(&data, sizeof(byte), 8, fp);
        }
    }

    // Reading PCM
    byte *dump = (byte*)malloc(data.size);
    if (dump == NULL) {
        printf("Allocation memory error");
        return 1;
    }
        
    if (fmt.compression == 1) {
        fmt.number_of_blocks = data.size / fmt.block_align;
        if ((fread(dump, fmt.block_align, fmt.number_of_blocks, fp))!=fmt.number_of_blocks) {  
            printf("Readin PCM data error.\n");
            return 1;
        } else {
            strcat(filename, ".raw");
            if ((pcm_data = fopen(filename, "wb"))==NULL) {
                printf("Can't open the PCM file for write. Exit.\n");
                return 1;
            }
            
            if(fwrite(dump, fmt.block_align, fmt.number_of_blocks, pcm_data)!=fmt.number_of_blocks) {
                printf("Can't write PCM file. Exit.\n");
                return 1;
            }
            printf("------------\nDone. PCM data writing in PCM file. Exit.\n");
        }
        
    } else {
        printf("Compression type is not PCM. Exit.\n");
        return 1;
    }
    free(dump);
    fclose(fp);
    fclose(pcm_data);
    return 0;
}