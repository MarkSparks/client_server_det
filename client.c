#include <stdlib.h>
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

  /*Create socket (internet domain, stream socket, protocol(TCP))*/
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  //check error
  if (clientSocket < 0){
      perror ("socket");
      exit (0);
  }
  
  /*Server address struct*/

  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number using htons - network byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /**Connect the socket to the server using the address struct**/
  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  /*on connection, keep listening to server*/
  while(1){
    //Read the message from the server into the buffer, 
    //Get the request from server
    recv(clientSocket, iBuffer, 1024, 0);
    int option = atoi(iBuffer);//change read string to int 

    if(option == 0 ){// Server matrix size request
      printf("Enter the value 'n' for the 'nxn' matrix: ");
      scanf("%d",&n);
      int c;
      while ((c = getchar()) != EOF && c != '\n');//read all chars until newline or EOF

      //prepare and send mat array
      char out[sizeof(n)];
      sprintf(out, "%d", n);
      strcpy(oBuffer,out);
      send(clientSocket,oBuffer,13,0);

    }else if(option == 1){ //Server matrix request
      
      //calculate suitable string size
      int size = (4 * (n * n)) + (n * 2);
      
      char str[size];
      int c,r;
      char rd[3];
      int count;
      memset(str,'\0',size);

      fflush(stdin);
      int pos = 0;
      //loop for each row column of the matrix
      /**
      * Create string of sructure 1,2,3;4,5,6;7,8,9
      **/
      for(c = 0;c < n; c++){
        for(r = 0;r < n; r++){
          //get value from user
          printf("Enter value for row %d, column %d:\n",r+1,c+1);
          scanf("%s",rd);
          int c;
          while ((c = getchar()) != EOF && c != '\n');
     
          strcat(str,rd);
          if(r != n-1){
            //separator for values in same row
            strcat(str,",");
          }
        }
        //separator for different columns
        strcat(str,";");
      }

      strcpy(oBuffer,str);
      send(clientSocket,oBuffer,1024,0);
    }else if(option == 2){//server requests exit
      return;
    } else{//print content from server, including the matrix det
      printf("%s;\n",iBuffer);   
    }
  }
  return 0;
}
