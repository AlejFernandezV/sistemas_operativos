#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MBR_SECTOR_SIZE 512

/**
 * @brief Estructura que representa una partición.
 */
struct particion {
  char particion[16]; 
};

/**
 * @brief Estructura que contiene información de una partición.
 */
struct info_particion {
  unsigned char indicador_arranque;
  int sec_inicial_head;
  int sec_inicial_sector;
  int sec_inicial_cylinder;
  unsigned char tipo;
  int sec_final_head;
  int sec_final_sector;
  int sec_final_cylinder;
  unsigned int lba;
  unsigned int tamanio;
};

/**
 * @brief Almacena la información de las particiones en una estructura.
 *
 * @param vec_info_particiones Vector de estructuras "info_particion" donde se almacenará la información.
 * @param vec_particiones Vector de estructuras "particion" que contiene los datos de las particiones.
 */
void almacenar_info_particiones(struct info_particion *vec_info_particiones, struct particion *vec_particiones);

/**
 * @brief Calcula el valor LBA (Logical Block Address) a partir de los valores de cilindro y sector.
 *
 * @param c Valor del cilindro.
 * @param s Valor del sector.
 * @return Valor LBA calculado.
 */
int calcular_lba(unsigned char c, unsigned char s);

/**
 * @brief Imprime la información de las particiones en una tabla.
 * @param table Puntero a la tabla de información de particiones.
 */
void imprimir_info_particiones(struct info_particion *table);

/**
 * @brief Obtiene el nombre del sistema de archivos según el tipo de partición.
 * @param partition_type Tipo de partición.
 * @return El nombre del sistema de archivos.
 */
const char *get_filesystem_name(unsigned char partition_type);

/**
 * @brief Concatena cuatro números hexadecimales en un solo valor entero sin signo.
 * @param a Primer número hexadecimal.
 * @param b Segundo número hexadecimal.
 * @param c Tercer número hexadecimal.
 * @param d Cuarto número hexadecimal.
 * @return Valor entero sin signo resultante de la concatenación.
 */
unsigned int concatenarNumerosHex(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Uso: Se debe colocar como segundo argumento el nombre de un archivo MBR.\n", argv[0]);
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

  //Declaracion de variables y vectores
  int contBits = 0;
  int particion = 1;
  struct particion vec_particiones[4];
  struct info_particion vec_info_particiones[4];

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
          vec_particiones[particion-1].particion[contBits] = mbr[i];
          break;
        case 2:
          vec_particiones[particion-1].particion[contBits] = mbr[i];
          break;
        case 3:
          vec_particiones[particion-1].particion[contBits] = mbr[i];
          break;
        case 4:
          vec_particiones[particion-1].particion[contBits] = mbr[i];
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
  //Almacena la informacion de los bits de 4 las particiones en un vector de 4 posiciones 
  almacenar_info_particiones(vec_info_particiones, vec_particiones); 
  //Imprime el vector con la informacion de las particiones
  imprimir_info_particiones(vec_info_particiones);
  return 0;
}

void almacenar_info_particiones(struct info_particion *vec_info_particiones, struct particion *vec_particiones) {
  for (int contParti = 0; contParti < 4; contParti++) {
    for(int contBits = 0; contBits < 8; contBits++){
      switch(contBits){
        case 0:
          vec_info_particiones[contParti].indicador_arranque = vec_particiones[contParti].particion[contBits];
          break;
        case 1:
          vec_info_particiones[contParti].sec_inicial_head = vec_particiones[contParti].particion[contBits];
          break;
        case 2:
          vec_info_particiones[contParti].sec_inicial_sector = vec_particiones[contParti].particion[contBits];
          break;
        case 3:
          vec_info_particiones[contParti].sec_inicial_cylinder = vec_particiones[contParti].particion[contBits];
          break;
        case 4:
          vec_info_particiones[contParti].tipo = vec_particiones[contParti].particion[contBits];
          break;
        case 5:
          vec_info_particiones[contParti].sec_final_head = vec_particiones[contParti].particion[contBits];
          break;
        case 6:
          vec_info_particiones[contParti].sec_final_sector = vec_particiones[contParti].particion[contBits];
          break;
        case 7:
          vec_info_particiones[contParti].sec_final_cylinder = vec_particiones[contParti].particion[contBits];
          break;
      }
      vec_info_particiones[contParti].lba = calcular_lba(vec_info_particiones[contParti].sec_inicial_cylinder, vec_info_particiones[contParti].sec_inicial_sector);
      vec_info_particiones[contParti].tamanio = concatenarNumerosHex(vec_particiones[contParti].particion[12],vec_particiones[contParti].particion[13],vec_particiones[contParti].particion[14],vec_particiones[contParti].particion[15]);
    }
  }
}

int calcular_lba(unsigned char c, unsigned char s){
  unsigned int C = c;
  unsigned int S = s;
  return (C*16*63) + (16*63) + (S-1);
}

void imprimir_info_particiones(struct info_particion *table) {
  for (int i = 0; i < 4; i++) {
    printf("Particion %d:\n", i + 1);
    printf("  Sector inicial: %d\n", concatenarNumerosHex(0x00,table[i].sec_inicial_cylinder,table[i].sec_inicial_head,table[i].sec_inicial_sector));
    printf("  Sector final: %d\n", concatenarNumerosHex(0x00,table[i].sec_final_cylinder,table[i].sec_final_head,table[i].sec_final_sector));
    printf("  Tamanio: %d MB \n", (table[i].tamanio/1000));
    printf("  Codigo: %#x\n", table[i].tipo);
    printf("  File system: %s\n", get_filesystem_name(table[i].tipo));
  }
}

const char *get_filesystem_name(unsigned char partition_type) {
  switch (partition_type) {
     case 0x00:
            return "Empty";
            
        case 0x01:
            return "DOS 12-bit FAT";
            
        case 0x02:
            return "XENIX root";
            
        case 0x03:
            return "XENIX /usr";
            
        case 0x04:
            return "DOS 3.0+ 16-bit FAT up to 32M";
            
        case 0x05:
            return "DOS 3.3+ Extended Partition";
            
        case 0x06:
            return "DOS 3.31+ 16-bit FAT over 32M";
            
        case 0x07:
            return "OS/2 IFS e.g., HPFS, Windows NT NTFS, exFAT, Advanced Unix, QNX2.x pre-1988";
            
        case 0x08:
            return "OS/2 v1.0-1.3 only, AIX boot partition, SplitDrive, Commodore DOS, DELL partition spanning multiple drives, QNX 1.x and 2.x";
            
        case 0x09:
            return "AIX data partition, Coherent filesystem, QNX 1.x and 2.x";
            
        case 0x0a:
            return "OS/2 Boot Manager, Coherent swap partition, OPUS";
            
        case 0x0b:
            return "WIN95 OSR2 FAT32";
            
        case 0x0c:
            return "WIN95 OSR2 FAT32, LBA-mapped";
            
        case 0x0d:
            return "SILICON SAFE";
            
        case 0x0e:
            return "WIN95: DOS 16-bit FAT, LBA-mapped";
            
        case 0x0f:
            return "WIN95: Extended partition, LBA-mapped";
            
        case 0x10:
            return "OPUS ?";
            
        case 0x11:
            return "Hidden DOS 12-bit FAT, Leading Edge DOS 3.x logically sectored FAT";
            
        case 0x12:
            return "Configuration/diagnostics partition";
            
        case 0x14:
            return "Hidden DOS 16-bit FAT <32M, AST DOS with logically sectored FAT";
            
        case 0x16:
            return "Hidden DOS 16-bit FAT >=32M";
            
        case 0x17:
            return "Hidden IFS e.g., HPFS";
            
        case 0x18:
            return "AST SmartSleep Partition";
            
        case 0x19:
            return "Unused";
            
        case 0x1b:
            return "Hidden WIN95 OSR2 FAT32";
            
        case 0x1c:
            return "Hidden WIN95 OSR2 FAT32, LBA-mapped";
            
        case 0x1e:
            return "Hidden WIN95 16-bit FAT, LBA";
        case 0x20:
            return "Windows Mobile update XIP";
            
        case 0x21:
            return "FSo2";
            
        case 0x22:
            return "NEC MS-DOS 3.x";
            
        case 0x24:
            return "NEC MS-DOS 3.x, Logical sectored FAT with 16-bit FAT sector";
            
        case 0x27:
            return "Hidden WIN95 OSR2 FAT32, LBA-mapped";
            
        case 0x39:
            return "Plan 9 partition";
            
        case 0x3c:
            return "PartitionMagic recovery partition";
            
        case 0x40:
            return "Venix 80286";
            
        case 0x41:
            return "PPC PReP PowerPC Reference Platform Boot";
            
        case 0x42:
            return "SFS Secure Filesystem";
            
        case 0x4d:
            return "QNX 4.x";
            
        case 0x4e:
            return "QNX 4.x 2nd part";
            
        case 0x4f:
            return "QNX 4.x 3rd part";
            
        case 0x50:
            return "OnTrack DM";
            
        case 0x51:
            return "OnTrack DM6 Aux1";
            
        case 0x52:
            return "CP/M";
            
        case 0x53:
            return "OnTrack DM6 Aux3";
            
        case 0x54:
            return "OnTrack DM6";
            
        case 0x55:
            return "EZ-Drive";
            
        case 0x56:
            return "Golden Bow VFeature";
            
        case 0x57:
            return "DrivePro";
            
        case 0x5c:
            return "Priam EDisk";
            
        case 0x61:
            return "SpeedStor";
            
        case 0x63:
            return "GNU HURD or SysV";
            
        case 0x64:
            return "Novell NetWare 286, 2.xx";
            
        case 0x65:
            return "Novell NetWare 386, 3.xx or 4.xx";
            
        case 0x66:
            return "Novell NetWare SMS Partition";
            
        case 0x67:
            return "Novell";
            
        case 0x68:
            return "Novell";
            
        case 0x69:
            return "Novell";
            
        case 0x70:
            return "DiskSecure Multi-Boot";
            
        case 0x75:
            return "IBM PC/IX";
            
        case 0x80:
            return "Minix old";
            
        case 0x81:
            return "Minix new";
            
        case 0x82:
            return "Linux swap or Solaris";
            
        case 0x83:
            return "Linux";
            
        case 0x84:
            return "OS/2 hidden C: drive";
            
        case 0x85:
            return "Linux extended";
            
        case 0x86:
            return "NTFS volume set";
            
        case 0x87:
            return "NTFS volume set";
            
        case 0x88:
            return "Linux plaintext";
            
        case 0x8e:
            return "Linux LVM";
            
        case 0x93:
            return "Amoeba";
            
        case 0x94:
            return "Amoeba BBT";
            
        case 0x9f:
            return "BSD/OS";
            
        case 0xa0:
            return "IBM Thinkpad hibernation";
            
        case 0xa5:
            return "FreeBSD";
            
        case 0xa6:
            return "OpenBSD";
            
        case 0xa7:
            return "Apple MacOS X";
            
        case 0xa8:
            return "Apple Darwin UFS";
            
        case 0xa9:
            return "NetBSD";
            
        case 0xab:
            return "Apple boot";
            
        case 0xaf:
            return "HFS or HFS+";
            
        case 0xb7:
            return "BSDI BSD/386 filesystem";
            
        case 0xb8:
            return "BSDI BSD/386 swap";
            
        case 0xbb:
            return "Boot Wizard hidden";
            
        case 0xbc:
            return "Acronis backup";
            
        case 0xbe:
            return "Solaris boot";
            
        case 0xbf:
            return "Solaris";
            
        case 0xc0:
            return "CTOS";
            
        case 0xc1:
            return "DR-DOS/Novell DOS secured";
            
        case 0xc4:
            return "DR-DOS 6.0 FAT16 < 32M";
            
        case 0xc6:
            return "DR-DOS 6.0 FAT16 >= 32M";
            
        case 0xc7:
            return "Syrinx";
            
        case 0xda:
            return "Non-FS data";
            
        case 0xdb:
            return "CP/M or CTOS or PTS or TSS";
            
        case 0xde:
            return "Dell PowerEdge Server utilities FAT fs";
            
        case 0xdf:
            return "BootIt";
            
        case 0xe1:
            return "DOS access or SpeedStor 12-bit FAT extended partition";
            
        case 0xe3:
            return "DOS R/O or SpeedStor";
            
        case 0xe4:
            return "SpeedStor 16-bit FAT extended partition < 1024 cyl.";
            
        case 0xeb:
            return "BeOS BFS";
            
        case 0xee:
            return "EFI GPT disk";
            
        case 0xef:
            return "EFI FAT-12/16/32";
            
        case 0xf0:
            return "Linux/PA-RISC boot loader";
            
        case 0xf1:
            return "SpeedStor";
            
        case 0xf4:
            return "SpeedStor large partition";
            
        case 0xf2:
            return "DOS secondary";
            
        case 0xfb:
            return "VMware File System partition";
            
        case 0xfc:
            return "VMware swap partition";
            
        case 0xfd:
            return "Linux raid partition with autodetect using persistent superblock";
            
        case 0xfe:
            return "LANstep";
            
        case 0xff:
            return "BBT";
            
        default:
            return "Unknown partition type";
  }
}

unsigned int concatenarNumerosHex(unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
    unsigned int resultado = 0;
    resultado |= a;
    resultado <<= 8;
    resultado |= b;
    resultado <<= 8;
    resultado |= c;
    resultado <<= 8;
    resultado |= d;
    return resultado;
}