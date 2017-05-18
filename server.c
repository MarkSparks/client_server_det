#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int n,a, determinant;

int det(int **m,int n);
int main(){
  int mSocket, newSocket;
  char iBuffer[2048];
  char oBuffer[1024];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  mSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Bind the address struct to the socket ----*/
  bind(mSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  if(listen(mSocket,5)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;
  newSocket = accept(mSocket, (struct sockaddr *) &serverStorage, &addr_size);

  /*---- Request for order n of matrix----*/
  strcpy(oBuffer,"0");
  send(newSocket,oBuffer,13,0);

  recv(newSocket, iBuffer, 1024, 0);
  printf("order n of matrix: %s\n",iBuffer);   

  n = atoi(iBuffer);
  a = n - 1;
  int **mat;
  int i;
  /* allocate the array */
  mat = malloc(n * sizeof *mat);
  for (i=0; i<n; i++){
    mat[i] = malloc(n * sizeof *mat[i]);
  }

   /*---- Request for string with matrix values----*/
  strcpy(oBuffer,"1");
  send(newSocket,oBuffer,13,0);

  recv(newSocket, iBuffer, 2048, 0);
  printf("matrix string: %s\n",iBuffer);  

  /**split string to get matrix value***/
  char *pt;
  char *end_str;
  int countR = 0;
  pt = strtok_r(iBuffer,";",&end_str);
  while (pt != NULL) {
      char *end_token;
      printf("pt(start) -> %s\n", pt);

      char *pt1;
      int countC = 0;
      pt1 = strtok (pt,",");
      while (pt1 != NULL) {
          //printf("%s\n", pt1);
          mat[countR][countC] = atoi(pt1);
          printf("[%d,%d] -> %d\n",countR,countC, mat[countR][countC]);
          pt1 = strtok (NULL, ",");
          countC++;
      }
      
      pt = strtok_r(NULL, ";",&end_str);
      printf("pt(end) -> %s\n", pt);
      countR++;
  }
  //call determinant function
  printf("The det is %d\n",det(mat,n));
  //printf("The detm is %d\n",detm(mat,n));

          
  //Tell client to exit
  strcpy(oBuffer,"2");
  send(newSocket,oBuffer,13,0);

  return 0;
}
int det(int **m,int n){
  if(n==2){
    printf("%d,%d\n%d,%d\n\n", m[0][0],m[0][1],m[1][0],m[1][1]);
    int d = (m[1][1]*m[0][0])-(m[1][0]*m[0][1]);
    return d;
  }else{
    int m_n = n - 1;
    printf("m_n -> %d\n",m_n);
    //declare determinant
    int d = 0;
    int f;
    
    //int m_mat[m_n][m_n];
    int **m_mat;
    int i;
    /* allocate the array */
    m_mat = malloc(m_n * sizeof *m_mat);
    for (i=0; i<m_n; i++){
      m_mat[i] = malloc(m_n * sizeof *m_mat[i]);
    }
    //declare counters
    int r,c,m_r,m_c;
    //iterate through column of original matrix
    for(c=0;c<n;c++){
        //int minorr=0,minorc=0;
        printf("c -> %d\n",c);
        int pos;
        //obtain cofactor
        f = m[0][c];
        /*set all rows and columns for the minor matrix*/
        int r_pos = 0,c_pos = 0;
         for(m_r = 0;m_r < n;m_r++){
            for(m_c = 0;m_c < n;m_c++){
                //get the column position of the m_matrix value
                if(m_r != 0){
                  printf("mr -> %d\n",m_r);

                  if((c%2) == 0 || c == 0){
                    //c_pos = c + 1;
                    pos = 1;
                  }else{
                    //c_pos = c - 1;
                    pos = -1;
                  }
                  if(m_c != c){
                    printf("mc -> %d\n",m_c); 
                    m_mat[r_pos][c_pos] = m[m_r][m_c];
                    printf("[r -> %d , c -> %d] -> %d\n\n",m_r,m_c,m[m_r][m_c]);
                    printf("[r_pos -> %d , c_pos -> %d] -> %d\n\n",r_pos,c_pos,m_mat[r_pos][c_pos]);
                    if(c_pos < m_n - 1){
                      c_pos++;
                    }else{
                      c_pos = 0;
                      r_pos++;
                    }
                  }
                }
             }
          }
          //printf("%d,%d\n%d,%d\n\n", m_mat[0][0],m_mat[0][1],m_mat[1][0],m_mat[1][1]);
          int temp = det(m_mat,m_n);
          printf("eqn -> %d * (%d * %d )\n",pos,f,temp);
          d = d + pos * (f * temp);
      }
         return(d);
    }
   
}
