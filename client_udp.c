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

  /*Create socket (internet domain,connectionless datagram, protocol in given socket type(UDP))*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
  //check error
  if (clientSocket < 0){
      perror ("socket error");
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

  printf("Enter the value 'n' for the 'nxn' matrix: ");
  scanf("%d",&n);
  int ch;
  while ((ch = getchar()) != EOF && ch != '\n');//read all chars until newline or EOF

  //calculate suitable string size
  int size = (4 * (n * n)) + (n * 2);
  
  char str[size];
  int c,r;
  char rd[3];
  int count;
  memset(str,'\0',size);

  fflush(stdin);
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

  printf("matrix strng -> %s\n",str );

  //send trigger to server
  strcpy(oBuffer,str);
  int len = sendto(clientSocket, oBuffer, strlen(oBuffer), 0, (struct sockaddr *) &serverAddr, addr_size);
  if (len < 0){
    error("ERROR in trigger sendto");
  }

  //receive result
  len = recvfrom(clientSocket, iBuffer, 1024, 0, (struct sockaddr *) &serverAddr, &addr_size);
  if ( len < 0 ) {
      perror( "recvfrom failed" );
  }
  printf("%s;\n",iBuffer); 

  return 0;
}
