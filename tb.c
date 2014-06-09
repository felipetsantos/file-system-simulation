#include <stdio.h>
#include <stdint.h>
/* hexdump -C fat.part */

#define BOOT_INDEX  0 
#define FAT_INDEX 1
#define ROOT_DIR 2

#define DIR_CODE 4
#define FILE_CODE 0x0

#define END_F 0xffffffff
#define FREE_CLUSTER 0x00000000

// Tamanhos
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
#define INIT "init"
#define LOAD "load"
#define LS "ls"
#define MKDIR "mkdir"
#define RMDIR "rmdir"
#define CREATE "create"
#define RM "rm"
#define WRITE "write"
#define CAT "cat"
#define SAUDACOES "VOLTE SEMPRE!"

// FILE ATTRIBUTES

#define READ_ONLY 1
#define EXEC 2
#define HIDDEN 4
#define DIRECTORY 8
#define DELETED 128


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
struct dir_entry block_dir[ROOT_DIR_SIZE];

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
void saveFatBlock(){
  if (!ptr_myfile)
  {
    printf("Unable to open file!");
    return 1;
  }  
  fseek(ptr_myfile,BLOCK_SIZE, SEEK_SET);

  fwrite(&fat,sizeof(fat), 1, ptr_myfile);
  fflush(ptr_myfile);
}
void saveDirBlock(int i){
  if (!ptr_myfile)
  {
    printf("Unable to open file!");
    return 1;
  }
  fseek(ptr_myfile,BLOCK_SIZE * i, SEEK_SET);
  fwrite(&block_dir,sizeof(block_dir), 1, ptr_myfile);
  fflush(ptr_myfile);
}
// Inicializa o root dir
void initRootDir(){
  int i;
  int k;
  for ( i = 0; i < ROOT_DIR_SIZE; i++)
  {
    block_dir[i].first_block = 0;
    block_dir[i].size = 0;
    block_dir[i].attributes = 0;
    for (k = 0; k < 16; k++)
    {
      block_dir[i].filename[k] = 0;
    }
    
    for (k = 0; k < 7; k++)
    {
      block_dir[i].reserved[k] = 0 ; 
    }

  }
  saveDirBlock(2);
}

void resetDirEntry(struct dir_entry dir){
    int k;
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
}
// Inicializa o cluster
void initClusterBlock(){
  int i;
  
  for ( i = 0; i < ROOT_DIR_SIZE; i++)
  {
    resetDirEntry(block_dir[i]);
  }

  if (!ptr_myfile)
  {
    printf("Unable to open file!");
    return 1;
  }
  
  /**
    * Inicializa o i em 3 pois os blocos anteriores já 
  * foram inicializados
  */
  for(i=3;i<CLUSTER_BLOCKS;i++){
    fseek(ptr_myfile,BLOCK_SIZE*(i+4), SEEK_SET);
    fwrite(&block_dir,sizeof(block_dir), 1, ptr_myfile);
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
    fread(&block_dir, sizeof(block_dir), 1, ptr_myfile);
}
void loadFat(){

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
    fseek(ptr_myfile,BLOCK_SIZE, SEEK_SET);
    fread(&fat, sizeof(fat), 1, ptr_myfile);
}

void init(){

  

  // Inicializa boot block com 0xa5
  initBootBlock();
  printf("Boot inicializado\n");  
  
  // Incializa fat
  initFatBlock();
  printf("Fat inicializada\n"); 

  // Inicializa o root dir
  initRootDir();
  printf("Root dir inicializada\n"); 

  // Inicializa os outros blocos do cluster
  initClusterBlock();
  printf("Cluster inicializado\n"); 

  
  
}
/*
<<<<<<< HEAD
void getArrayPath(char *filename,char *path[]){\n
=======
void getArrayPath(char *filename,char *path[]){
>>>>>>> 9ef0b29c07d2725fbfeb213715c5a509b42b58f8
  int count;
  count = 0;

  params = strtok(params,"/");
  
  
  while(params!=NULL){
    param[count] = params;
    params = strtok(NULL," ");
    count++;
<<<<<<< HEAD
  } 
}*/
int searchDir(char *filename,int *block){
  int i,index;
  uint8_t file[16];
  strcpy(file,filename);
  index = *block;
  loadBlock(index);
  for(i=0;i<ROOT_DIR_SIZE;i++){
    
    if(isDir(block_dir[i]) && memcmp(block_dir[i].filename, file, sizeof(block_dir[i].filename)) == 0 
    ){
      *block = block_dir[i].first_block;
      return i;
    }
      
  }
  return -1;
}

int getFreeFatPosition(){
  int i;
  loadFat();
  for(i=ROOT_DIR+1;i<FAT_SIZE;i++){
    if(fat[i] == 0){
<<<<<<< HEAD
       return i;
    }
  }   
 return -1;
}

int isDir(struct dir_entry dir){
  
  if((dir.attributes & DIRECTORY) && !(dir.attributes & DELETED) ){
    return 1;    
  }else{
    return 0;  
  }

}
// Cria diretório 
void mkdir(const char* filename){
  // quebra o caminho do diretório
  char *toke;
  char *name;
  int block,dir_exist,free_fat,i,has_error;
  has_error = 0;
  block = ROOT_DIR;
  toke = strtok(strdup(filename),"/");
  name = toke;

  while(toke!=NULL){
    
    name = toke;
    toke= strtok(NULL,"/");
    
    dir_exist = searchDir(name,&block);
    if( dir_exist == -1 && toke != NULL){
      printf("Diretório ou arquivo não encontrado\n");
      has_error = 1;
    }
      
    
  }
  

  if(has_error == 0){
    // Testa se o diretório existe
    if(dir_exist == -1){
      free_fat = getFreeFatPosition();
     for(i=0;i<ROOT_DIR_SIZE;i++){
      if(block_dir[i].filename[0] == 0 ||  block_dir[i].attributes & DELETED){
        
        resetDirEntry(block_dir[i]);
        
        strcpy(block_dir[i].filename,name);
        block_dir[i].first_block = free_fat;
        block_dir[i].attributes = DIRECTORY;      

        break;
      }
      }
      saveDirBlock(block); 
      fat[free_fat] = -1;
      saveFatBlock();
       
    }else{
      printf("Já existe um diretório com o nome %s \n",name);
    }
  }
  
}


void ls(const char* filename){
  char *toke;
  char *name;
  int block,dir_exist,free_fat,i;
  block = ROOT_DIR;
  if(strcmp("/",filename) == 0 ){
    dir_exist = 1;

  }else{
    toke = strtok(strdup(filename),"/");
    name = toke;

    while(toke!=NULL){
      
      name = toke;
      toke= strtok(NULL,"/");
      
      dir_exist = searchDir(name,&block);
      if( dir_exist == -1 && toke != NULL){
        printf("Diretório ou arquivo não encontrado\n");
      }
        
      
    }
  }
  if(dir_exist == -1){
    printf("Diretório ou arquivo não encontrado\n");
  }else{
    loadBlock(block);
    for(i=0;i<ROOT_DIR_SIZE;i++){
      if(isDir(block_dir[i]) == 1){
        printf("\t%s\n",block_dir[i].filename);
      }
      
    }
  }

}

void rmdir(const char* filename){
  char *toke;
  char *name;
  int block,dir_exist,dir_block,i,k,is_empty;
  block = ROOT_DIR;
  if(strcmp("/",filename) == 0 ){
    printf("O diretório raiz não pode ser removido\n");
  }else{
    toke = strtok(strdup(filename),"/");
    name = toke;

    while(toke!=NULL){
      
      name = toke;
      toke= strtok(NULL,"/");
      dir_block = block;
      dir_exist = searchDir(name,&block);
      if( dir_exist == -1 && toke != NULL){
        printf("Diretório ou arquivo não encontrado\n");
      }
        
      
    }
    if(dir_exist == -1){
      printf("Diretório ou arquivo não encontrado\n");
    }else{

      for(i=0;i<ROOT_DIR_SIZE;i++){
       
        if(isDir(block_dir[i]) && strcmp(block_dir[i].filename,name) == 0){
          loadBlock(block);
          is_empty =0;
          for (k = 0; k < ROOT_DIR_SIZE; k++)
          {
            if(isDir(block_dir[k])){
              is_empty =1;
            }
          }
          loadBlock(dir_block);
          if(is_empty == 0){
            block_dir[i].attributes = block_dir[i].attributes | DELETED;
            loadFat();
            fat[block_dir[i].first_block] = 0;
            saveDirBlock(dir_block);
            saveFatBlock();
            break;
          }else{
            printf("Para remover o diretório ele deve estar vazio\n");
            break;
          }

        }
        
      }
    }

  }

}
void shell(){
  char buffer[1024];
  char *cmd;
 
  //char *params[];
  char *params;


  cmd = "";
  params = "";
  while(strcmp(EXIT,cmd) != 0){
     char *user,*host;
     user = "08103842";
     host = "l1847524";
        printf("%s %s@%s %s~/ $  %s",GREEN,user,host,BLUE,WHITE);
        fgets(buffer, sizeof(buffer), stdin);
        strtok(buffer, "\n");
        params = strtok(strdup(buffer)," ");
        cmd = params;
        selectCommand(cmd,strtok(NULL,""));
  }
  
  printf("%s \n",SAUDACOES);


}

// Seleciona o comando digitado no shell
void  selectCommand(char *cmd,char *params){
  

  
  // INIT
  if(strcmp(INIT,cmd) == 0){
    
    char *param[0];

    // Executa o comando init
    ptr_myfile = fopen("fat.part","w+");

    // Testa se os parametros estão corretos
    if(testParams(params,0,param) == 1){
      init();
    }else{
      printf("O comando %s não tem parametros\n",INIT);
    }
   fclose(ptr_myfile);
  //LOAD
  }else if(strcmp(LOAD,cmd) == 0){
    printf("%s ainda não foi implementado\n",LOAD);

  // LS
  }else if(strcmp(LS,cmd) == 0){
        char *param[1];

        
        ptr_myfile = fopen("fat.part","r+");

        // Testa se os parametros estão corretos
        if(testParams(params,1,param) == 1){
           ls(param[0]);
        }else{
          printf("Formato do %s: %s [PATH]\n",LS,LS);
        }
        fclose(ptr_myfile);
  // MKDIR
  }else if(strcmp(MKDIR,cmd) == 0){
        char *param[1];

        // Executa o comando mkdir
        ptr_myfile = fopen("fat.part","r+");

        // Testa se os parametros estão corretos
        if(testParams(params,1,param) == 1){
           mkdir(param[0]);
        }else{
          printf("Formato do %s: %s [PATH/DIRNAME]\n",MKDIR,MKDIR);
        }

        fclose(ptr_myfile);
  //RMDIR
  }else if(strcmp(RMDIR,cmd) == 0){
        char *param[1];

        // Executa o comando mkdir
        ptr_myfile = fopen("fat.part","r+");

        // Testa se os parametros estão corretos
        if(testParams(params,1,param) == 1){
           rmdir(param[0]);
        }else{
          printf("Formato do %s: %s [PATH/DIRNAME]\n",RMDIR,RMDIR);
        }
        fclose(ptr_myfile);
  //CREATE
  }else if(strcmp(CREATE,cmd) == 0){
    printf("%s ainda não foi implementado\n",CREATE);

  //RM
  }else if(strcmp(RM,cmd) == 0){
    printf("%s ainda não foi implementado\n",RM);

  //WRITE
  }else if(strcmp(WRITE,cmd) == 0){
    printf("%s ainda não foi implementado\n",WRITE);

  //CAT
  }else if(strcmp(CAT,cmd) == 0){
    printf("%s ainda não foi implementado\n",CAT);

  // EXIT
  }else if(strcmp(EXIT,cmd) == 0){
    // DO NOTHING
  }else{
    printf("Comando não encontrado\n");
  }

}

// Testa o número de parametros
int testParams(char *params,int number,char *param[]){
  //char *param[number];
  int count;
  count = 0;

  params = strtok(params," ");
  
  
  while(params!=NULL){
    if(count < number){
      param[count] = params;
    }
    params = strtok(NULL," ");
    //printf("%s\n",params);
    count++;
  }

  if(count == number){
    return 1;
  }else{
    return 0;
  }

}
int main (void) {

  shell(); 
  return 0;
}