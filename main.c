#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <inttypes.h>
#include "wave.h"

int main(int argc, char *argv[])
{
    char filename[200];
    FILE *fp;
    FILE *pcm_data;

    strcat(filename, argv[1]);
    struct riff_chunk riff = {"RIFF", 0, "WAVE"};
    struct fmt_chunk fmt = {"fmt "};
    struct data_chunk data = {"data"};

    if ((fp = fopen(argv[1], "rb"))==NULL) {
        fprintf(stderr,"Can't open the file. Exit.\n");
        return 1;
    }
    
    // Reading RIFF section
    if (fread(&riff.id, sizeof(byte), 12, fp)!= 12)  {
        fprintf(stderr,"Can't read RIFF chunk or EOF is met. Exit.\n");
        return 1;
    } else {
        if (strncmp("RIFF", riff.id, sizeof(riff.id))) {
            fprintf(stderr,"File format is not RIFF. Exit.\n");
            return 1;
        }
        if (strncmp("WAVE", riff.type,sizeof(riff.type))) {
            fprintf(stderr,"File format is not WAVE. Exit.\n");
            return 1;
        }
    };

    // Reading fmt.id and fmt.size
    if (fread(&fmt, sizeof(byte), 8, fp)!=8) {
        fprintf(stderr,"Can't read fmt chunk or EOF is met. Exit.\n");
        return 1;
    } else {
        if (strncmp("fmt ", fmt.id, sizeof(fmt.id))) {
            fprintf(stderr,"File have no fmt chunk. Exit.\n");
            return 1;
        }

    }
    
    // Reading fmt Sample Format Info
    if (fread(&fmt.compression, sizeof(byte), fmt.size, fp) != fmt.size) {
        fprintf(stderr,"Can't read Sample Format Info in fmt chunk or EOF is met. Exit.\n");
        return 1;
    }

    printf("Compression: %" PRIu16 "\n", fmt.compression);
    printf("Channels: %" PRIu16 "\n", fmt.chanels);
    printf("Sample Rate: %" PRIu32 "\n", fmt.sample_rate);
    printf("Bit Rate: %" PRIu32 "\n", fmt.bit_per_sample);

    // Reading data/some chunk
    if (fread(&data, sizeof(byte), 8, fp)!=8) {
        fprintf(stderr,"Error of reading data chunk. Exit.\n");
        return 1;
    } else {
        while (strncmp("data", data.id, sizeof(data.id))) {
            fseek(fp, data.size, 1); // перемещаем указатель файла на конец чанка (его размер)
            fread(&data, sizeof(byte), 8, fp);
        }
    }

    // Reading PCM
    byte *dump = malloc(data.size);
    if (!dump) {
        fprintf(stderr,"Allocation memory error");
        return 1;
    }
        
    if (fmt.compression == 1) {
        fmt.number_of_blocks = data.size / fmt.block_align;
        if ((fread(dump, fmt.block_align, fmt.number_of_blocks, fp))!=fmt.number_of_blocks) {  
            fprintf(stderr,"Readin PCM data error.\n");
            return 1;
        } else {
            strcat(filename, ".raw");
            if ((pcm_data = fopen(filename, "wb"))==NULL) {
                fprintf(stderr,"Can't open the PCM file for write. Exit.\n");
                return 1;
            }
            
            if(fwrite(dump, fmt.block_align, fmt.number_of_blocks, pcm_data)!=fmt.number_of_blocks) {
                fprintf(stderr,"Can't write PCM file. Exit.\n");
                return 1;
            }
            printf("------------\nDone. PCM data writing in PCM file. Exit.\n");
        }
        
    } else {
        fprintf(stderr,"Compression type is not PCM. Exit.\n");
        return 1;
    }
    free(dump);
    fclose(fp);
    fclose(pcm_data);
    return 0;
}