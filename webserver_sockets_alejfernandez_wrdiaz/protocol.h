typedef struct{
  char comando[16];
  char filename[PATH_MAX];
  char mensaje[PATH_MAX];
  int size;   
  mode_t mode; 
}file_info;

typedef struct{
  char comando[16];
  char filename[PATH_MAX]; 
}request;