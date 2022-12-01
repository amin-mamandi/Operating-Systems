#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAXSTR 1000
#define PAGE_NUMBER 0xFFFFFFFF
#define OFFSET 0xFFFFFFFF

int main(int argc, char *argv[])
{
  char line[MAXSTR];
  int *page_table, *mem_map;
  unsigned int log_size, phy_size, page_size, d;
  unsigned int num_pages, num_frames;
  unsigned int offset, logical_addr, physical_addr, page_num, frame_num;

  /* Get the memory characteristics from the input file */
  fgets(line, MAXSTR, stdin);
  if((sscanf(line, "Logical address space size: %d^%d", &d, &log_size)) != 2){
    fprintf(stderr, "Unexpected line 1. Abort.\n");
    exit(-1);
  }
  fgets(line, MAXSTR, stdin);
  if((sscanf(line, "Physical address space size: %d^%d", &d, &phy_size)) != 2){
    fprintf(stderr, "Unexpected line 2. Abort.\n");
    exit(-1);
  }
  fgets(line, MAXSTR, stdin);
  if((sscanf(line, "Page size: %d^%d", &d, &page_size)) != 2){
    fprintf(stderr, "Unexpected line 3. Abort.\n");
    exit(-1);
  }

  num_frames = (int) pow(2, phy_size - page_size);
  num_pages  = (int) pow(2, log_size - page_size);
  printf("Number of Pages: %d, Number of Frames: %d\n",  num_pages, num_frames);

  /* Allocate arrays to hold the page table and memory frames map */
  page_table = (int *) malloc(num_pages * sizeof(int));
  mem_map    = (int *) malloc(num_frames * sizeof(int));

  /* Initialize page table to indicate that no pages are currently mapped to physical memory */
  /* Initialize memory map table to indicate no valid frames */
  
  memset(page_table, 0, sizeof(page_table));
  memset(mem_map, 0, sizeof(mem_map));
  
  int tmp = 0;
  int frame = 0;
  printf("\n");

  /* Read each accessed address from input file. Map the logical address to
     corresponding physical address */
  fgets(line, MAXSTR, stdin);
  while(!(feof(stdin))){
    sscanf(line, "0x%x", &logical_addr);
    fprintf(stdout, "Logical Address: 0x%x\n", logical_addr);


    /* Calculate page number and offset from the logical address */
    offset   = logical_addr & (OFFSET >> (log_size - page_size)); 
    page_num = (logical_addr & (PAGE_NUMBER << page_size)) >> page_size;
    // printf("**Offset**: 0x%x \n", offset);
    printf("Page Number: %d\n", page_num);

    if (page_table[page_num] == 0){
      printf("Page Fault!\n");
      printf("Frame Number: %d\n", tmp);
      mem_map[tmp] = offset;
      physical_addr = (tmp<<page_size) | offset;
      page_table[page_num] = tmp+1;
      printf("Physical Address: 0x%x\n", physical_addr);
      tmp++;
    }
    else {
      frame = page_table[page_num];
      frame--;
      printf("Frame Number: %d\n", frame);
      physical_addr = (frame<<page_size) | offset;
      printf("Physical Address: 0x%x\n", physical_addr);
    }
    // /* Read next line */
    printf("\n");
    fgets(line, MAXSTR, stdin);    
  }

  return 0;
}
