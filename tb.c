#include <stdio.h>
#include <stdint.h>
/* hexdump -C fat.part */

#define BOOT_INDEX  0 
#define FAT_INDEX 1
#define ROOT_DIR 2

#define DIR_CODE 0x04
#define FILE_CODE 0x0

#define END_F 0xffffffff
#define FREE_CLUSTER 0x00000000

#define BLOCK_SIZE 4096
#define FAT_SIZE 1024
#define ROOT_DIR_SIZE 128
 
// DIR ENTRY
typedef struct dir_entry{
  uint8_t filename[16];
  uint8_t attributes;
  uint8_t reserved[7];
  uint32_t first_block;
  uint32_t size;
};

// BLOCK
uint8_t block[BLOCK_SIZE];

uint32_t fat[FAT_SIZE];

struct dir_entry root_dir[ROOT_DIR_SIZE];

FILE *ptr_myfile;

void initBootBlock(){
  
  int i;
  for( i=0;i<BLOCK_SIZE;i++){
    block[i] = 0xa5;
  }

//  ptr_myfile = fopen("fat.part","wb");
  if (!ptr_myfile)
  {
    printf("Unable to open file!");
    return 1;
  }
  fseek(ptr_myfile,0, SEEK_SET);
  fwrite(&block,sizeof(block), 1, ptr_myfile);
  fflush(ptr_myfile);
//  fclose(ptr_myfile);

}
void initFatBlock(){
  
  int i;
  fat[BOOT_INDEX] = -1;
  fat[FAT_INDEX] = -1;
  fat[ROOT_DIR] = -1;
  for(i=ROOT_DIR+1;i<FAT_SIZE;i++){
    fat[i] = 0;
  }


//  ptr_myfile = fopen("fat.part","wb");
  if (!ptr_myfile)
  {
    printf("Unable to open file!");
    return 1;
  }

  fseek(ptr_myfile,BLOCK_SIZE, SEEK_SET);

  fwrite(&fat,sizeof(fat), 1, ptr_myfile);
  fflush(ptr_myfile);
//  fclose(ptr_myfile);

}

void initRootDir(){
  
  int i;

  // Inicicialização do root
  // root.filename = 0 ;
  // root.attributes = DIR_CODE;
  // root.reserved =
  // root.first_block =
  // root.size = 
  int k;
  for ( i = 0; i < ROOT_DIR_SIZE; i++)
  {
    root_dir[i].first_block = 0;
    root_dir[i].size = 0;
    root_dir[i].attributes = 0;
    for (k = 0; i < 16; k++)
    {
      root_dir[i].filename[k] = 0x55;
    }
    
    for (k = 0; i < 7; k++)
    {
      root_dir[i].reserved[k] = 0 ; 
    }

  }

  //  ptr_myfile = fopen("fat.part","wb");
  if (!ptr_myfile)
  {
    printf("Unable to open file!");
    return 1;
  }

  fseek(ptr_myfile,BLOCK_SIZE * 2, SEEK_SET);

  fwrite(&root_dir,sizeof(root_dir), 1, ptr_myfile);
  fflush(ptr_myfile);
//  fclose(ptr_myfile);

}

void loadBloco(){

    ptr_myfile = fopen("fat.part","r+");
    if (!ptr_myfile)
    {
      printf("Unable to open file!");
      return 1;
    }
    fseek(ptr_myfile,0, SEEK_SET);
    /*SEEK_SET comeca no inicio do arquivo
    SEEK_END comeca no fim do arquivo
    SEEK_CUR comeca na posicao corrente do arquivo*/
    fread(&block,sizeof(block), 1, ptr_myfile);
    fclose(ptr_myfile);
    int i;
    for( i=0;i<BLOCK_SIZE;i++){
      printf("0x%x\n", block[i]);
    }
}


void init(){

  ptr_myfile = fopen("fat.part","wb");
  // // Inicializa boot
  initBootBlock();  
  
  initFatBlock();

  initRootDir();
  // int i;

  
  
  // fat[BOOT_INDEX] = 0xa5;
  
  // // Inicializa a fat
  // fat[FAT_INDEX] = END_F;
    
  // // Inicializa o root dir
  // fat[ROOT_DIR] = END_F;
  
  // // Inicializa o restante da fat
  // for(i=ROOT_DIR+1;i<1024;i++){
  //   fat[i] = FREE_CLUSTER;
  // }

  // writeFat(fat);
  
}

int main (void) {
  init();


  return 0;
}