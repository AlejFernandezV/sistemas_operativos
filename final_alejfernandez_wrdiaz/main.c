#include <stdio.h>
#include <stdlib.h>

#define MBR_SECTOR_SIZE 512

struct particiones {
  char particion1[16];
  char particion2[16];
  char particion3[16];
  char particion4[16];
};

struct partition_table {
  unsigned char bootable;
  unsigned char start_chs[3];
  unsigned char partition_type;
  unsigned char end_chs[3];
  unsigned int start_sector;
  unsigned int size;
};

void print_partition_table(struct partition_table *table);

const char *get_filesystem_name(unsigned char partition_type);

unsigned int hex_a_int(unsigned char *hex_string);

void mostrar_info_particion(char particion[]);

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <mbr_file>\n", argv[0]);
    return 1;
  }

  // Lee el archivo MBR
  FILE *fp = fopen(argv[1], "rb");
  
  if (fp == NULL) {
    fprintf(stderr, "Failed to open file %s\n", argv[1]);
    return 1;
  }

  // Se asigna en el vector mbr los valores que estan en el archivo MBR
  unsigned char mbr[MBR_SECTOR_SIZE];
  int nread = fread(mbr, 1, sizeof(mbr), fp);
  if (nread != sizeof(mbr)) {
    printf("Failed to read MBR file: %s\n", argv[1]);
    return 1;
  }

  //Imprime el valor de los bits del archivo MBR
  printf("Valor de nread: %d\n", nread);

  int contBits = 0;
  int particion = 1;
  struct particiones s_partitions;

  //Verifica que el archivo que se abrió sea MBR, es decir que 
  //termine en los caracteres hexadecimales 0x55 y 0xAA
  if(mbr[510] != 0x55 && mbr[511]!= 0xAA){
    printf("Error! Este no es un archivo MBR\n");
    return 1; 
  }

  //Almacenamiento de 16 en 16 los valores del archivo MBR en los vectores particion
  //Recorre a partir del caracter 446 hasta el 510 
  for(int i = 446; i < nread-2; i++){
    if(particion <= 4 && contBits <= 15){
      switch(particion){
        case 1:
          s_partitions.particion1[contBits] = mbr[i];
          printf("Almacenado %02x en particion 1\n", s_partitions.particion1[contBits]);
          break;
        case 2:
          s_partitions.particion2[contBits] = mbr[i];
          printf("Almacenado %02x en particion 2\n", s_partitions.particion2[contBits]);
          break;
        case 3:
          s_partitions.particion3[contBits] = mbr[i];
          printf("Almacenado %02x en particion 3\n", s_partitions.particion3[contBits]);
          break;
        case 4:
          s_partitions.particion4[contBits] = mbr[i];
          printf("Almacenado %02x en particion 4\n", s_partitions.particion4[contBits]);
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

void print_partition_table(struct partition_table *table) {
  int i;

  for (i = 0; i < 4; i++) {
    printf("Partition %d:\n", i + 1);
    printf("  Start sector: %d\n", table[i].start_sector);
    printf("  End sector: %d\n", table[i].start_sector + table[i].size - 1);
    printf("  Size: %d sectors\n", table[i].size);
    printf("  Partition code: %#x\n", table[i].partition_type);
    printf("  File system: %s\n", get_filesystem_name(table[i].partition_type));
  }
}

const char *get_filesystem_name(unsigned char partition_type) {
  switch (partition_type) {
    case 0x00: return "Empty";
    case 0x01: return "FAT12";
    case 0x02: return "FAT16 (>= 32 MB)";
    case 0x04: return "FAT16 (< 32 MB)";
    case 0x06: return "FAT32";
    case 0x07: return "NTFS";
    case 0x08: return "Linux";
    case 0x0B: return "Linux swap";
    default: return "Unknown";
  }
}

unsigned int hex_a_int(unsigned char *hex_string) {
  unsigned int decimal = 0;
  int i = 0;
  unsigned int power = 1;

  while (hex_string[i] != '\0') {
    unsigned char digit = hex_string[i];
    unsigned int value;

    if (digit >= '0' && digit <= '9') {
      value = digit - '0';
    } else if (digit >= 'a' && digit <= 'f') {
      value = digit - 'a' + 10;
    } else if (digit >= 'A' && digit <= 'F') {
      value = digit - 'A' + 10;
    } else {
      return -1;
    }

    decimal += value * power;
    power *= 16;
    i++;
  }

  return decimal;
}