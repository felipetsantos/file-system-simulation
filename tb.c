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

// CORES SHELL
#define GREEN  "\x1B[32m"
#define BLUE  "\x1B[34m"
#define WHITE  "\x1B[37m"

// COMANDOS
#define EXIT "exit"
#define MKDIR "mkdir"
#define INIT "init"
#define LOAD "load\n"
#define LS "ls\n"
#define RMDIR "rmdir\n"
#define CREATE "create\n"
#define RM "rm\n"
#define WRITE "write\n"
#define CAT "cat\n"
#define SAUDACOES "VOLTE SEMPRE!"




// • load - carregar o sistema de arquivos do disco
// • ls [/caminho/diretorio] - listar diret ́orio
// • mkdir [/caminho/diretorio] - criar diret ́orio
// • rmdir [/caminho/diretorio] - remover diret ́orio
// • create [/caminho/arquivo] - criar arquivo
// • rm [/caminho/arquivo] - excluir arquivo
// • write ”string” [/caminho/arquivo] - anexar dados em um arquivo
// • cat [/caminho/arquivo] - ler o conte ́
// udo de um arquivo

// dir entry
typedef struct dir_entry{
  uint8_t filename[16];
  uint8_t attributes;
  uint8_t reserved[7];
  uint32_t first_block;
  uint32_t size;
};

// block
uint8_t block[BLOCK_SIZE];

// fat
uint32_t fat[FAT_SIZE];

// root dir
struct dir_entry root_dir[ROOT_DIR_SIZE];

FILE *ptr_myfile;


// Inicializa o boot
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

// Inicializa o fat
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

// Inicializa o root dir
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

// Inicializa o cluster
void initClusterBlock(){
	int i;
  int k;
  struct dir_entry dir;
  
  dir.first_block = 0;
  dir.size = 0;
  dir.attributes = 0;
  for (k = 0; k < 16; k++)
  {
    dir.filename[k] = 0;
  }
  
  for (k = 0; k < 7; k++)
  {
    dir.reserved[k] = 0 ; 
  }

	if (!ptr_myfile)
	{
		printf("Unable to open file!");
		return 1;
	}
	
	/**
  * Inicializa o i em 3 pois os blocos anteriores já 
	* foram inicializados anteriormente
	*/
	for(i=3;i<CLUSTER_BLOCKS;i++){
		fseek(ptr_myfile,BLOCK_SIZE*(i+4), SEEK_SET);
		fwrite(&dir,sizeof(dir), 1, ptr_myfile);
		fflush(ptr_myfile);
	}
}

// Carrega um bloco para a estrutura block 
void loadBlock(int block_index){

    if (!ptr_myfile)
    {
      printf("Unable to open file!");
      return 1;
    }
    /*
    * SEEK_SET comeca no inicio do arquivo
    * SEEK_END comeca no fim do arquivo
    * SEEK_CUR comeca na posicao corrente do arquivo
    */
    fseek(ptr_myfile,BLOCK_SIZE*block_index, SEEK_SET);
    fread(&block, sizeof(block), 1, ptr_myfile);
}


void init(){

  

  // Inicializa boot block com 0xa5
  initBootBlock();  
  // Incializa fat
  initFatBlock();
  // Inicializa o root dir
  initRootDir();
  // Inicializa os outros blocos do cluster
  initClusterBlock();

  
  
}
// Cria diretório 
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

void shell(){
  char cmd[1024];
  
  while(strcmp(EXIT,cmd) != 0){
        printf("%s08103842@l1846641 %s~/ $  %s",GREEN,BLUE,WHITE);
        fgets(cmd, sizeof(cmd), stdin);
        strtok(cmd, "\n");
  }
   printf("%s",SAUDACOES );


}
int main (void) {
  shell(); 
  //ptr_myfile = fopen("fat.part","w+");
	//mkdir("teste");  
	//init();
  
  //fclose(ptr_myfile);

  return 0;
}
