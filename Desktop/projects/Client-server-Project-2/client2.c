//  COEN_233_Client2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT 2000
#define CLIENT_PORT 2001
#define START_OF_PACKET_ID 0XFFFF
#define END_OF_PACKET_ID START_OF_PACKET_ID
#define CLIENT_ID 0XFF
#define ACC_PER 0XFFF8
#define SEGMENT_NO 0
#define BUFFER_SIZE 100
#define RETRY_COUNT 3

struct PermittedAccessMessagePacket
{
    int startOfPacketId;
    short int clientId;
    int acc_OK;
    short int segmentNo;
    int length;
    char technology;
    unsigned int sourceSubscriberNo;
    int endOfPacketId;
};

struct NotPaidMessagePacket
{
    int startOfPacketId;
    short int clientId;
    int notPaid;
    short int segmentNo;
    int length;
    char technology;
    unsigned int sourceSubscriberNo;
    int endOfPacketId;
};

struct SubsriberNotExistMessagePacket
{
    int startOfPacketId;
    short int clientId;
    int notExist;
    short int segmentNo;
    int length;
    char technology;
    unsigned int sourceSubscriberNo;
    int endOfPacketId;
};

struct AccessPermissionRequestPacket
{
    int startOfPacketId;
    short int clientId;
    int Acc_Per;
    short int segmentNo;
    int length;
    char technology;
    unsigned int sourceSubscriberNo;
    int endOfPacketId;
};

//Function declarations
int createSocket(void);
int getTestCaseNumber(void);
struct AccessPermissionRequestPacket getAccessPermissionRequestPacket(int testCaseNumber);
void sendAccessPacket(struct AccessPermissionRequestPacket accessPermissionRequestPacket, int retryCount, int testCaseNumber);
long receiveServerResponse(char buffer[], int size);
void printServerResponse(char buffer[]);
void printSubscriberHasNotPaidPacket(char buffer[]);
void printSubscriberDoesNotExistPacket(char buffer[]);
void printSubscriberDoesNotExistPacket(char buffer[]);
void printSubscriberPermittedToAccessNetworkPacket(char buffer[]);

int sockfd;
unsigned int destinationStructLength;
struct sockaddr_in serverAddr;

int main()
{
    sockfd = createSocket();

    destinationStructLength = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    while(1)
    {
        int testCaseNumber = getTestCaseNumber();
        //Get access packet to send to client
        struct AccessPermissionRequestPacket accessPermissionRequestPacket = getAccessPermissionRequestPacket(testCaseNumber);

        int retryCount;
        for(retryCount = 0; retryCount < RETRY_COUNT; retryCount++)
        {
            //Send access packet to server
            char buffer[BUFFER_SIZE] = {0};
            sendAccessPacket(accessPermissionRequestPacket, retryCount, testCaseNumber);

            //Receive server response
            long receiveReturnCode = receiveServerResponse(buffer, sizeof(buffer));

            if (receiveReturnCode < 0)
            {
                if(errno == EWOULDBLOCK)
                {
                    printf("Timed out while waiting for response from server\n\n");
                }
            }
            else
            {
                printServerResponse(buffer);
                break;
            }
        }
        //Check for timeout test case
        if(retryCount == RETRY_COUNT)
        {
            printf("Server does not respond\n");
        }
        printf("\n");
    }
    return 0;
}

//Function to create and bind client socket
int createSocket()
{
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket created successfully\n\n");

    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    if((setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))) < 0)
    {
        printf("Error setting timeout");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in myAddr;
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(CLIENT_PORT);
    myAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&myAddr, sizeof(myAddr)) < 0)
    {
        printf("Bind error");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

//Fuction to display test cases and accept test case number
int getTestCaseNumber()
{
    int test_case_number;

    printf("****** List of test cases ******\n");
    printf("1. Subscriber permitted to access network messages\n");
    printf("2. Subscriber has not paid\n");
    printf("3. Subscriber does not exist\n");
    printf("4. Timeout\n");
    printf("5. Timeout and then acknowledge\n");
    printf("6. End\n");
    printf("Enter the test case number:\n");
    scanf("%d", &test_case_number);
    printf("\n");
    return test_case_number;
}

//Function to get access permission request packet
struct AccessPermissionRequestPacket getAccessPermissionRequestPacket(int testCaseNumber)
{
    struct AccessPermissionRequestPacket accessPermissionRequestPacket;

    accessPermissionRequestPacket.startOfPacketId = START_OF_PACKET_ID;
    accessPermissionRequestPacket.clientId = CLIENT_ID;
    accessPermissionRequestPacket.Acc_Per = ACC_PER;
    accessPermissionRequestPacket.segmentNo = SEGMENT_NO;
    accessPermissionRequestPacket.technology = 03;
    accessPermissionRequestPacket.length = (sizeof(accessPermissionRequestPacket.technology) + sizeof(accessPermissionRequestPacket.sourceSubscriberNo));
    accessPermissionRequestPacket.endOfPacketId = END_OF_PACKET_ID;

    switch (testCaseNumber)
    {
    case 1:
        accessPermissionRequestPacket.sourceSubscriberNo = 4253333333;
        break;
    case 2:
        accessPermissionRequestPacket.sourceSubscriberNo = 4253333111;
        break;
    case 3:
        accessPermissionRequestPacket.sourceSubscriberNo = 1234567891;
        break;
    case 4:
    case 5:
        accessPermissionRequestPacket.sourceSubscriberNo = 00;
        break;
    case 6:
        exit(0);
    default:
        break;
    }

    return accessPermissionRequestPacket;
}

//Function to send access packet to server
void sendAccessPacket(struct AccessPermissionRequestPacket accessPermissionRequestPacket, int retryCount, int testCaseNumber)
{
    char buffer[BUFFER_SIZE];
    if (retryCount == 1 && testCaseNumber == 5)
    {
        printf("Changing the Source subscriber number\n");
        accessPermissionRequestPacket.sourceSubscriberNo = 4251992345;
        accessPermissionRequestPacket.technology = 04;
        sprintf(buffer,
                "%X %hX %X %hX %d %hhu %u %X",
                accessPermissionRequestPacket.startOfPacketId,
                accessPermissionRequestPacket.clientId,
                accessPermissionRequestPacket.Acc_Per,
                accessPermissionRequestPacket.segmentNo,
                accessPermissionRequestPacket.length,
                accessPermissionRequestPacket.technology,
                accessPermissionRequestPacket.sourceSubscriberNo,
                accessPermissionRequestPacket.endOfPacketId);
    }

    sprintf(buffer,
            "%X %hX %X %hX %d %hhu %u %X",
            accessPermissionRequestPacket.startOfPacketId,
            accessPermissionRequestPacket.clientId,
            accessPermissionRequestPacket.Acc_Per,
            accessPermissionRequestPacket.segmentNo,
            accessPermissionRequestPacket.length,
            accessPermissionRequestPacket.technology,
            accessPermissionRequestPacket.sourceSubscriberNo,
            accessPermissionRequestPacket.endOfPacketId);

    if(sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&serverAddr, destinationStructLength) < 0)
    {
        printf("Unable to send the message\n");
        exit(EXIT_FAILURE);
    }

    printf("Sending access request with data:\nStartOfPacket ID : %X \n", accessPermissionRequestPacket.startOfPacketId);
    printf("Client ID : %hX \n", accessPermissionRequestPacket.clientId);
    printf("ACC Permission : %X \n", accessPermissionRequestPacket.Acc_Per);
    printf("Segment No : %hX \n",  accessPermissionRequestPacket.segmentNo);
    printf("Length : %d \n", accessPermissionRequestPacket.length);
    printf("Technology : %hhu \n", accessPermissionRequestPacket.technology);
    printf("Source Subscriber No : %u \n", accessPermissionRequestPacket.sourceSubscriberNo);
    printf("End of Packet ID : %X \n\n", accessPermissionRequestPacket.endOfPacketId);
}

//Function to receive server response
long receiveServerResponse(char buffer[], int size)
{
    long receiveReturnCode = recvfrom(sockfd, buffer, size, 0, (struct sockaddr *)&serverAddr, &destinationStructLength);
    return receiveReturnCode;
}

//Function to print specific server response
void printServerResponse(char buffer[])
{
    int startOfPacketId, serverResponse;
    short int clientId;

    sscanf(buffer,
           "%X %hX %X",
           &startOfPacketId,
           &clientId,
           &serverResponse);

    switch (serverResponse)
    {
    case 0XFFF9:
        printSubscriberHasNotPaidPacket(buffer);
        break;
    case 0XFFFA:
        printSubscriberDoesNotExistPacket(buffer);
        break;
    case 0XFFFB:
        printSubscriberPermittedToAccessNetworkPacket(buffer);
        break;
    }
}

//Function to print subscriber has not paid response packet
void printSubscriberHasNotPaidPacket(char buffer[])
{
    struct NotPaidMessagePacket packet;
    sscanf(buffer,
           "%X %hX %X %hX %d %hhu %u %X",
           &packet.startOfPacketId,
           &packet.clientId,
           &packet.notPaid,
           &packet.segmentNo,
           &packet.length,
           &packet.technology,
           &packet.sourceSubscriberNo,
           &packet.endOfPacketId);
    
    printf("Received: [Subscriber has not paid packet]\n");
    printf("Start Of Packet ID : %X \n", packet.startOfPacketId);
    printf("Client ID : %hX \n", packet.clientId);
    printf("Not Paid : %X \n", packet.notPaid);
    printf("Segment No : %hX \n",  packet.segmentNo);
    printf("Length : %d \n", packet.length);
    printf("Technology : %hhu \n", packet.technology);
    printf("Source Subscriber No : %u \n", packet.sourceSubscriberNo);
    printf("End of Packet ID : %X \n\n", packet.endOfPacketId);

}

//Function to print subscriber does not exist packet
void printSubscriberDoesNotExistPacket(char buffer[])
{
    struct SubsriberNotExistMessagePacket packet;
    sscanf(buffer,
           "%X %hX %X %hX %d %hhu %u %X",
           &packet.startOfPacketId,
           &packet.clientId,
           &packet.notExist,
           &packet.segmentNo,
           &packet.length,
           &packet.technology,
           &packet.sourceSubscriberNo,
           &packet.endOfPacketId);
    
    printf("Received: [Subscriber does not exist packet]\n");
    printf("Start Of Packet ID : %X \n", packet.startOfPacketId);
    printf("Client ID : %hX \n", packet.clientId);
    printf("Not exist : %X \n", packet.notExist);
    printf("Segment No : %hX \n",  packet.segmentNo);
    printf("Length : %d \n", packet.length);
    printf("Technology : %hhu \n", packet.technology);
    printf("Source Subscriber No : %u \n", packet.sourceSubscriberNo);
    printf("End of Packet ID : %X \n\n", packet.endOfPacketId);

}

//Function to print subscriber permitted access response packet
void printSubscriberPermittedToAccessNetworkPacket(char buffer[])
{
    struct PermittedAccessMessagePacket packet;
    sscanf(buffer,
           "%X %hX %X %hX %d %hhu %u %X",
           &packet.startOfPacketId,
           &packet.clientId,
           &packet.acc_OK,
           &packet.segmentNo,
           &packet.length,
           &packet.technology,
           &packet.sourceSubscriberNo,
           &packet.endOfPacketId);
    
    printf("Received: [Subscriber permitted to access the network message]\n");
    printf("Start Of Packet ID : %X \n", packet.startOfPacketId);
    printf("Client ID : %hX \n", packet.clientId);
    printf("Acc_Per : %X \n", packet.acc_OK);
    printf("Segment No : %hX \n",  packet.segmentNo);
    printf("Length : %d \n", packet.length);
    printf("Technology : %hhu \n", packet.technology);
    printf("Source Subscriber No : %u \n", packet.sourceSubscriberNo);
    printf("End of Packet ID : %X \n\n", packet.endOfPacketId);
}


