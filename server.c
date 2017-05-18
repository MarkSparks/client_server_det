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

  /*Create socket (internet domain, stream socket, protocol(TCP))*/
  mSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*Server address struct*/

  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number using htons - network byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /* Bind the address struct to the socket*/
  bind(mSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*Listen on mSocket, backlog -> with 5 max connection requests queued*/
  if(listen(mSocket,5)==0){
    printf("Listening\n");
  }else{
    printf("Error\n");
  }

  /* Accept call creates a new socket for the incoming connection */
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
  /* allocate the array space dynamically */
  mat = malloc(n * sizeof *mat);
  for (i=0; i<n; i++){
    mat[i] = malloc(n * sizeof *mat[i]);
  }

   /* Request for string with matrix values */
  strcpy(oBuffer,"1");
  send(newSocket,oBuffer,13,0);

  recv(newSocket, iBuffer, 2048, 0);
  printf("matrix string: %s\n",iBuffer);  

  /* split string to get matrix value */
  char *pt;
  char *end_str;
  int countR = 0;
  pt = strtok_r(iBuffer,";",&end_str);
  //split by ; to get rows
  while (pt != NULL) {
      char *end_token;
      char *pt1;
      int countC = 0;
      pt1 = strtok (pt,",");
      //split string by , to get values
      while (pt1 != NULL) {
          mat[countR][countC] = atoi(pt1);
          pt1 = strtok (NULL, ",");
          countC++;
      }
      
      pt = strtok_r(NULL, ";",&end_str);
      printf("pt(end) -> %s\n", pt);
      countR++;
  }
  //call determinant function
  printf("The det is %d\n",det(mat,n));

  //Tell client to exit
  strcpy(oBuffer,"2");
  send(newSocket,oBuffer,13,0);

  return 0;
}
int det(int **m,int n){
  if(n==1){
    //handle when mat size is 1
    return m[0][0];
  }
  if(n==2){
    //when size of matrix is 2, multiply to get det
    printf("%d,%d\n%d,%d\n\n", m[0][0],m[0][1],m[1][0],m[1][1]);
    int d = (m[1][1]*m[0][0])-(m[1][0]*m[0][1]);
    return d;
  }else{
    int m_n = n - 1;//size of minor matrix

    //declare determinant variable
    int d = 0;
    int f;//cofactor
    
    //minor matrix array
    int **m_mat;
    int i;
    // allocate the array space
    m_mat = malloc(m_n * sizeof *m_mat);
    for (i=0; i<m_n; i++){
      m_mat[i] = malloc(m_n * sizeof *m_mat[i]);
    }

    //declare counters
    int c,m_r,m_c;
    //iterate through each column of original matrix
    for(c=0;c<n;c++){
        int pos;
        //obtain cofactor
        f = m[0][c];
        //iterate all values [rows,columns] for the matrix
        int r_pos = 0,c_pos = 0;//positions for the minor matrix
         for(m_r = 0;m_r < n;m_r++){
            for(m_c = 0;m_c < n;m_c++){
                //get the column position of the m_matrix value
                if(m_r != 0){//if first column of matrix skip
                  
                  //dtermin if positive of negative
                  if((c%2) == 0 || c == 0){
                    pos = 1;
                  }else{
                    pos = -1;
                  }

                  if(m_c != c){//if not current column: set m_matrix value
                    //set value for the minor matrix position
                    m_mat[r_pos][c_pos] = m[m_r][m_c];
                    if(c_pos < m_n - 1){
                      //if done with m_matrix columns in given row
                      c_pos++;
                    }else{
                      //else move to next row and reset column count
                      r_pos++;
                      c_pos = 0;
                    }
                  }
                }
             }
          }
          int temp = det(m_mat,m_n);
          printf("eqn -> %d * (%d * %d )\n",pos,f,temp);
          d = d + pos * (f * temp);
      }
         return(d);
    }
   
}
