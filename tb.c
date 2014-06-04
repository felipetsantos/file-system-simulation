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
#define CLUSTER_BLOCKS 1024
 
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

// FAT
uint32_t fat[FAT_SIZE];

// ROOT DIR
struct dir_entry root_dir[ROOT_DIR_SIZE];

FILE *ptr_myfile;

// Inicializa o Boot
void initBootBlock(){
  
  int i;
  for( i=0;i<BLOCK_SIZE;i++){
    block[i] = 0xa5;
  }

  if (!ptr_myfile)
  {
    printf("Unable to open file!");
    return 1;
  }
  fseek(ptr_myfile,0, SEEK_SET);
  fwrite(&block,sizeof(block), 1, ptr_myfile);
  fflush(ptr_myfile);


}
// Inicializa o Fat
void initFatBlock(){
  
  int i;
  fat[BOOT_INDEX] = -1;
  fat[FAT_INDEX] = -1;
  fat[ROOT_DIR] = -1;
  for(i=ROOT_DIR+1;i<FAT_SIZE;i++){
    fat[i] = 0;
  }

  if (!ptr_myfile)
  {
    printf("Unable to open file!");
    return 1;
  }

  fseek(ptr_myfile,BLOCK_SIZE, SEEK_SET);

  fwrite(&fat,sizeof(fat), 1, ptr_myfile);
  fflush(ptr_myfile);


}

// Inicializa o ROOT DIR
void initRootDir(){
  int i;
  int k;
  for ( i = 0; i < ROOT_DIR_SIZE; i++)
  {
    root_dir[i].first_block = 0;
    root_dir[i].size = 0;
    root_dir[i].attributes = 0;
    for (k = 0; k < 16; k++)
    {
      root_dir[i].filename[k] = 0;
    }
    
    for (k = 0; k < 7; k++)
    {
      root_dir[i].reserved[k] = 0 ; 
    }

  }

  if (!ptr_myfile)
  {
    printf("Unable to open file!");
    return 1;
  }

  fseek(ptr_myfile,BLOCK_SIZE * 2, SEEK_SET);
  fwrite(&root_dir,sizeof(root_dir), 1, ptr_myfile);
  fflush(ptr_myfile);


}

void initClusterBlock(){
	int i;
	for(i=0;i<BLOCK_SIZE;i++){
		block[i] = 0;
	}

	if (!ptr_myfile)
	{
		printf("Unable to open file!");
		return 1;
	}
	
	/**Inicializa o for em 3 pois os blocos anteriores já 
	* foram inicializados anteriormente
	*/
	for(i=3;i<CLUSTER_BLOCKS;i++){
		fseek(ptr_myfile,BLOCK_SIZE*(i+4), SEEK_SET);
		fwrite(&block,sizeof(block), 1, ptr_myfile);
		fflush(ptr_myfile);
	}
	
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

  // Inicializa boot block com 0xa5
  initBootBlock();  
  // Incializa fat
  initFatBlock();
  // Inicializa o root dir
  initRootDir();
  // Inicializa os outros blocos do cluster
  initClusterBlock();

  fclose(ptr_myfile);
  
}

void mkdir(const char* filename){
	// quebra o caminho do diretório
	char *toke;
	char *name;
	toke = strtok(strdup(filename),"/");
	name = toke;
	while(toke!=NULL){
		
		name = toke;
		toke= strtok(NULL,"/");	
	}
	printf("tamano do caracter %s \n",name);
}
int main (void) {
	//mkdir("teste");  
	init();


  return 0;
}
