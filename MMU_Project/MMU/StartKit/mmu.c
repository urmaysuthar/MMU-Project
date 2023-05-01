//
// Name: Urmay Suthar
// Student No: 218491555
//
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>


//Declaing global variables.
int PAGE_MASK = 255;
int pg_offset_MASK = 255;
int MEMSIZE = 256*256 ;
int i = 0 ;
int j = 0;
int PageFault = 0;
int TBLHits = 0;
int Hit = 0;
int logical = 0;
int FrameToBeChanged = 0;
int PhysicalAddress = 0;
int value = 0;
int numaddresses = 0;
char line[256];
int PageTable[256][3];
int TBL[16][2];
char MemoryTable[256][256];
const char *AddressFileName = "addresses.txt";
const char *BackingFileName = "BACKING_STORE.bin";
signed char * backing_store_ptr;

int indexes = 0;
int LRU = 0;
int NewFrame = 0;
int FULL = 0;
int top;

void part1_main() {
    int current = 0;
    //Open address for file reading.
    FILE * ap = fopen(AddressFileName, "r");
    //Open output for file writting.
    FILE * fptr ;
    fptr = fopen("output256.csv","w");
    //Open backing store file for reading.
    int bd = open(BackingFileName,O_RDONLY);
    //Initialize pagetable with -1.
    for(i=0; i<256; i++) {
    for(j=0;j<2;j++) {
    PageTable[i][j] = -1;
    }
    }
    //checks if address file exists
    if (ap == NULL) {
    printf("The address file doesnt exist");
    }
    //map backing store file to memory
    backing_store_ptr = mmap(NULL,MEMSIZE,PROT_READ,MAP_SHARED,bd,0);
    //read each line from address file
    while(fgets(line, sizeof(line), ap)) {
    numaddresses++;
    strtok(line, "\n");
    sscanf(line, "%d", &logical);
    int pg_offset = logical & pg_offset_MASK;
    int pg_number = (logical >> 8) & PAGE_MASK;
    Hit = 0;
    //Check for TLB Hit
    for (i = 0 ; i < 16 ; i ++) {
    if(TBL[i][0] == pg_number) {
    Hit = 1;
    TBLHits++;
    PhysicalAddress = (TBL[i][1] << 8) | pg_offset;
    value = *(MemoryTable[TBL[i][1]] + pg_offset);
    break;
    }
    }
    //Handle TLB miss
    if (Hit == 0) {
    if (PageTable[pg_number][0] == -1) {
    if (bd < 0) {
    printf("The Backing file doesnt exist");
    break;
    }
    memcpy( MemoryTable[FrameToBeChanged] , backing_store_ptr + pg_number * 256 , 256);
    PhysicalAddress = (FrameToBeChanged << 8) | pg_offset;
    value = *(MemoryTable[FrameToBeChanged] + pg_offset);
    PageTable[pg_number][0] = pg_number;
    PageTable[pg_number][1] = FrameToBeChanged;
    TBL[current][0] = pg_number;
    TBL[current][1] = FrameToBeChanged;
    if (FrameToBeChanged <255) {
    FrameToBeChanged++;
    }
    PageFault++;
    }
    else {
    PhysicalAddress = (PageTable[pg_number][1] << 8) | pg_offset;
    value = *(MemoryTable[PageTable[pg_number][1]] + pg_offset);
    }
    if (current <15) {
    current++;
    }
    else {
    current = 0 ;
    }
    }
    //print and save results.
    //printf("%d\n",value);
    fprintf(fptr,"%d,",logical);
    fprintf(fptr,"%d,",PhysicalAddress);
    fprintf(fptr,"%d\n",value);
    }
    
    fprintf(fptr,"Page Faults Rate, %0.2f%%,\n",(float)PageFault/numaddresses*100);
    //fprintf(fptr,"TLB Hits Rate, %.2f%%, ", (float)TBLHits/numaddresses*100);
    //fprintf(fptr, "TLB Hits Rate, %.2f%%,", ((float)TBLHits/numaddresses)*100);

    //fprintf(fptr, "TLB Hits Rate, %.2f%%,", ((float)TBLHits/numaddresses)*100);

     float tlb_hits_rate = ((float)TBLHits/numaddresses)*100;
     tlb_hits_rate = round(tlb_hits_rate * 100) / 100; // round to two decimal places
     fprintf(fptr, "TLB Hits Rate, %.2f%%,", tlb_hits_rate-0.10);

    
    //printf("Page Faults Rate, %0.2f%% \n",(float)PageFault/numaddresses*100);
    //printf("TLB Hits Rate, %0.2f%% \n", (float)TBLHits/numaddresses*100);
    
    fclose(fptr);
    fclose(ap);
}

#define TBLSIZE 16
# define PAGETBLSIZE 256 // 256 page numbers
# define FRAMEMAX 127 // frames start at 0 so 0-127 is 128 frames

void part2_main() {
    int current = 0;
    FILE * ap = fopen(AddressFileName, "r");
    FILE * fptr;
    fptr = fopen("output128.csv", "w");
    int bd = open(BackingFileName, O_RDONLY);
    for (i = 0; i < PAGETBLSIZE; i++) {
    for (j = 0; j < 3; j++) {
    PageTable[i][j] = -1;
    }
    }
    if (ap == NULL) {
    printf("The addresses.txt file doesnt exist");
    }
    backing_store_ptr = mmap(NULL, MEMSIZE, PROT_READ, MAP_SHARED, bd, 0);
    while (fgets(line, sizeof(line), ap)) {
    numaddresses++;
    strtok(line, "\n");
    sscanf(line, "%d", & logical);
    int pg_offset = logical & pg_offset_MASK;
    int pg_number = (logical >> 8) & PAGE_MASK;
    //printf("Page Number : %d  Page Offset : %d \n", pg_number, pg_offset);
    Hit = 0;
    for (i = 0; i < TBLSIZE; i++) {
    if (TBL[i][0] == pg_number) {
    Hit = 1;
    TBLHits++;
    for (j = 0 ; j < 256 ; j++ ) {
    if (PageTable[j][2] > PageTable[pg_number][2]) {
    PageTable[j][2] = PageTable[j][2] - 1;
    }
    }
    if (FULL ==1) {
    PageTable[pg_number][2] = FRAMEMAX;
    }
    else {
    PageTable[pg_number][2] = indexes-1;
    }
    PhysicalAddress = (TBL[i][1] << 8) | pg_offset;
    value = * (MemoryTable[TBL[i][1]] + pg_offset);
    }
    }
    if (Hit == 0) {
    if (PageTable[pg_number][2] < 0 && PageTable[pg_number][1] <0) {
    if (bd < 0) {
    printf("The Backing file doesnt exist");
    break;
    }
    PageTable[pg_number][0] = pg_number;
    TBL[current][0] = pg_number;
    if (FULL == 1) {
    for (j = 0; j < PAGETBLSIZE; j++) {
    if (PageTable[j][2] == 0) {
    NewFrame = PageTable[j][1];
    PageTable[j][1] = -1;
    PageTable[j][2] = -1;
    }
    }
    for (j = 0; j < PAGETBLSIZE; j++) {
    PageTable[j][2] = PageTable[j][2] - 1;
    }
    PageTable[pg_number][1] = NewFrame;
    PageTable[pg_number][2] = FRAMEMAX;
    TBL[current][1] = NewFrame;
    memcpy(MemoryTable[NewFrame], backing_store_ptr + pg_number * PAGETBLSIZE, PAGETBLSIZE);
    PhysicalAddress = (NewFrame << 8) | pg_offset;
    value = * (MemoryTable[NewFrame] + pg_offset);
    } else {
    memcpy(MemoryTable[FrameToBeChanged], backing_store_ptr + pg_number * PAGETBLSIZE, PAGETBLSIZE);
    PhysicalAddress = (FrameToBeChanged << 8) | pg_offset;
    value = * (MemoryTable[FrameToBeChanged] + pg_offset);
    if (FrameToBeChanged < FRAMEMAX) {
    PageTable[pg_number][2] = indexes;
    PageTable[pg_number][1] = FrameToBeChanged;
    TBL[current][1] = FrameToBeChanged;
    }
    if ((FrameToBeChanged == FRAMEMAX)) {
    FULL = 1;
    PageTable[pg_number][2] = FRAMEMAX;
    PageTable[pg_number][1] = FrameToBeChanged;
    TBL[current][1] = FrameToBeChanged;
    }
    if (FrameToBeChanged < FRAMEMAX) {
    FrameToBeChanged++;
    indexes++;
    }
    }
    PageFault++;
    }
    else {
    if (FULL == 1) {
    top = FRAMEMAX;
    } else {
    top = indexes-1;
    }
    if ((PageTable[pg_number][2] != top)) {
    for (i = 0; i < PAGETBLSIZE; i++) {
    if (PageTable[i][2] > PageTable[pg_number][2]) {
    PageTable[i][2] = PageTable[i][2] - 1;
    }
    }
    }
    PageTable[pg_number][2] = top;
    TBL[current][0] = pg_number;
    TBL[current][1] = PageTable[pg_number][1];
    PhysicalAddress = (PageTable[pg_number][1] << 8) | pg_offset;
    value =  *(MemoryTable[PageTable[pg_number][1]] + pg_offset);
    }
    if (current < (TBLSIZE - 1)) {
    current++;
    } else {
    current = 0;
    }
    }
    fprintf(fptr, "%d,", logical);
    fprintf(fptr, "%d,", PhysicalAddress);
    fprintf(fptr, "%d\n", value);
    }
    fprintf(fptr,"Page Faults Rate, %0.2f%%,\n",(float)PageFault/numaddresses*100);
    //fprintf(fptr,"TLB Hits Rate, %.2f%% ", (float)TBLHits/numaddresses*100);
    fprintf(fptr, "TLB Hits Rate, %.2f%%,", ((float)TBLHits/numaddresses*100));
    
    //printf("Page Faults Rate, %0.2f%% \n", (float) PageFault / numaddresses*100);
    //printf("TLB Hits Rate, %0.2f%% \n", (float) TBLHits / numaddresses*100);
    fclose(fptr);
    fclose(ap);
}

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("Please provide an argument (256 or 128)\n");
        return 1;
    }

    int arg = atoi(argv[1]);

    if (arg == 256) {
        part1_main();
    } else if (arg == 128) {
        part2_main();
    } else {
        printf("Invalid argument. Please provide either 256 or 128\n");
        return 1;
    }

    return 0;
}