#include <stdio.h>
#include <stdlib.h>

#define MBR_SECTOR_SIZE 512

struct partition_table {
  char partion1[16];
  char partion2[16];
  char partion3[16];
  char partion4[16];
};

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <mbr_file>\n", argv[0]);
    return 1;
  }

  // Read the MBR file
  FILE *fp = fopen(argv[1], "rb");
  
  if (fp == NULL) {
    fprintf(stderr, "Failed to open file %s\n", argv[1]);
    return 1;
  }

  // Se asigna en el vector mbr los valores que estan en el archivo MBR
  unsigned char mbr[512];
  int nread = fread(mbr, 1, sizeof(mbr), fp);
  if (nread != sizeof(mbr)) {
    printf("Failed to read MBR file: %s\n", argv[1]);
    return 1;
  }

  printf("Valor de nread: %d\n", nread);

  int contBits = 0;
  int particion = 1;
  struct partition_table partition_table;

  if(mbr[510] != 0x55 && mbr[511]!= 0xAA){
    printf("Error! Este no es un archivo MBR\n");
    return 1; 
  }

  for(int i = 446; i < nread-2; i++){
    //printf("Leyendo: %02x \n", mbr[i]);
    if(particion <= 4 && contBits <= 15){
      switch(particion){
        case 1:
          partition_table.partion1[contBits] = mbr[i];
          printf("Almacenado %02x en particion 1\n", partition_table.partion1[contBits]);
          break;
        case 2:
          partition_table.partion2[contBits] = mbr[i];
          printf("Almacenado %02x en particion 2\n", partition_table.partion2[contBits]);
          break;
        case 3:
          partition_table.partion3[contBits] = mbr[i];
          printf("Almacenado %02x en particion 3\n", partition_table.partion3[contBits]);
          break;
        case 4:
          partition_table.partion4[contBits] = mbr[i];
          printf("Almacenado %02x en particion 4\n", partition_table.partion4[contBits]);
          break;
      }
      contBits++;
    }  
    else{
      i = i - 1;
      contBits = 0;
      particion++;
    }
  } 
  
  return 0;
}
