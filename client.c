#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(){
  int n;
  int clientSocket;
  char iBuffer[1024];
  char oBuffer[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  while(1){

    /*---- Read the message from the server into the buffer ----*/
    recv(clientSocket, iBuffer, 1024, 0);

    /*---- Print the received message ----*/
    //printf("Request option: %s;\n",iBuffer);   

    int option = atoi(iBuffer);


    if(option == 0 ){

      //getMatrix
      printf("Enter the value 'n' for the 'nxn' matrix: ");
      scanf("%d",&n);
      int c;
      while ((c = getchar()) != EOF && c != '\n');

      char out[sizeof(n)];
      sprintf(out, "%d", n);
      strcpy(oBuffer,out);
      send(clientSocket,oBuffer,13,0);


     }else if(option == 1){ 
      
      //getString size
      int size = (4 * (n * n)) + (n * 2);
      printf("String size: %d\n",size);
      
      char str[size];
      int c,r;
      char i;
      char rd[3];
      //scanf("%c",&i);
      fflush(stdin);
      //strcat(str,"0");
      int count;
      /*for (count = 0; count < size; ++count)
      {
        str[count] = '';
      }*/
      memset(str,'\0',size);

      int pos = 0;
      for(c = 0;c < n; c++){
        for(r = 0;r < n; r++){
          printf("Enter value for row %d, column %d:\n",r+1,c+1);
          scanf("%s",rd);
          int c;
          while ((c = getchar()) != EOF && c != '\n');
          printf("Read -> %s\n",rd );
          //str[pos] = rd;
          strcat(str,rd);
          printf("String -> %s\n",str);
          if(r != n-1){
            strcat(str,",");
          }
        }
        strcat(str,";");
      }

      strcpy(oBuffer,str);
      send(clientSocket,oBuffer,1024,0);
    }else if(option == 2){
      return;
    } else{
      printf("%s;\n",iBuffer);   

    }
  }



  return 0;
}

/*void main(){
  a = n-1;
  int matrix[a][a];
  insertValues(n,matrix);
  printf("The determinant is %d\n",det(matrix,n));
}*/