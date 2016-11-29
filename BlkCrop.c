#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>


#define CTU_SIZE    64


int main(int argc, char *argv[])
{
    int ifd, ofd;
    uint32_t width;
    uint32_t height;
    uint32_t wxh;
    uint32_t idx;
    uint8_t buf[CTU_SIZE];
    uint32_t width_in_blk;
    int offset;
    
    char output[255];

    if (argc < 4)
    {
        fprintf(stderr, "useage: %s [input_file] [width] [height] [idx]\n", argv[0]);
        
        return -1;
    }

    width       = 0;
    height      = 0;
    
    ifd = open(argv[1], O_RDONLY);
    if (ifd < 0)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    width   = atoi(argv[2]);
    height  = atoi(argv[3]);
    wxh     = width * height;
    idx     = atoi(argv[4]);

    sprintf(output, "idx_%d.yuv", idx);
    ofd = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR);

    // Y
    width_in_blk = width / CTU_SIZE;
    offset = (idx / width_in_blk) * (CTU_SIZE * CTU_SIZE * width_in_blk) + (idx % width_in_blk) * CTU_SIZE;
    
    lseek(ifd, offset, SEEK_SET);

    for (int i = 0; i < CTU_SIZE; i++)
    {
        read(ifd, buf, CTU_SIZE);
        write(ofd, buf, CTU_SIZE);
        lseek(ifd, offset + (i+1)* width, SEEK_SET);    
    }

    // U
    lseek(ifd, wxh, SEEK_SET);

    width_in_blk = width / CTU_SIZE;
    offset = wxh + (idx / width_in_blk) * (CTU_SIZE / 2 * CTU_SIZE / 2 * width_in_blk) + (idx % width_in_blk) * CTU_SIZE / 2;
    
    for (int i = 0; i < CTU_SIZE / 2; i++)
    {
        read(ifd, buf, CTU_SIZE / 2);
        write(ofd, buf, CTU_SIZE / 2);
        lseek(ifd, (offset) + (i+1) * (width / 2), SEEK_SET);       
    }

    // V
    lseek(ifd, wxh + wxh / 4, SEEK_SET);

    width_in_blk = width / CTU_SIZE;
    offset = wxh + wxh / 4 + (idx / width_in_blk) * (CTU_SIZE / 2 * CTU_SIZE / 2 * width_in_blk) + (idx % width_in_blk) * CTU_SIZE / 2;
    
    for (int i = 0; i < CTU_SIZE / 2; i++)
    {
        read(ifd, buf, CTU_SIZE / 2);
        write(ofd, buf, CTU_SIZE / 2);
        lseek(ifd, (offset) + (i+1) * (width / 2), SEEK_SET);   
    }

    close(ifd);
    close(ofd);

    return 0;   
}
