// Notes by Grace Austen @Grace-Austen on github
// Code shown on zoom (Lecture 7, Sep 20)
// All non-professor notes will be signified by double forward slashes

#include <stdio.h>
#include <stdlib.h>

int main() {
    int b, w, mem[16];
    int i, count = 0;

    printf("reading bytes in octal from stdin\n");
    while(scanf("%o", &b) != EOF) { //read in from stdin to b(uffer) and check to see if it's reached EOF
        printf("  0%03o\n", b);
        if(count > 15) {
            printf("too many bytes");  //we're only reading in 16 bytes to mem or 8 integer values
            exit(0);
        }
        if(!(count & 1)) { //count even?
            w = b;
        } else {
            w = w | (b<<8);; //this is the bit shifting for little endian
            mem[count>>1] = w; //div count in half because it counts every octal in (2 of which are required for each int)
        }
        count++;
    }
    printf("%d bytes read from stdin\n", count);
    if(count & 1) { //count is odd (half an extra octal num read in)
        mem[count>>1] = w;
    }
    printf("\nmemory contents displayed as 16-bit words\n");
    printf("  in octal, little-endian format\n");
    printf("\n    memory   memory\n");
    printf("   contents  address\n");
    printf("  +--------+\n");
    for( i = 0; i < ((count+1)>>1); i++) {
        printf("   | %06o |  0%02o\n", mem[i], i<<1); //count is the address, mem is stored as an int but we're only using the first two bytes so addressing is a little funny
    }
    printf("  +--------+\n");

    return 0;
}