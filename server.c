#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Calculations.h"

int main()
{

    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server, client; /*Structure describing internet socket address*/
    int port = 8888;                   /*Default*/

    int Distance = 0;
    float FuelConsumed = 0;
    int CurrentSpeed, NewSpeed = 0;
    int EngineState = 0; /*Initially engine is off*/
    int OdometerReading = INITIAL_ODO_READING;
    float FuelLevel = INITIAL_FUEL_LEVEL; /*Full tank at start*/

    /*Create socket*/
    socket_desc = socket(AF_INET, SOCK_STREAM, 0); /*Created socket file descriptor*/
    if (socket_desc == -1)
    {
        printf("Could not create socket");
        return 1;
    }

    /*Receive port number*/
    printf("Enter the port : \n");
    scanf("%d", &port);

    /*Prepare the sockaddr_in structure*/
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    /*Bind ->assigns  the address  specified  by  addr  to the socket referred to by the file descriptor*/
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    /*Listen*/
    listen(socket_desc, 3); /*The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow.*/

    /*Accept any incoming connection*/
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    /*When a client connects, accept returns a new socket file descriptor client_sock that can be used to communicate with the client.
    Second argument is pointer to a struct sockaddr object that will be filled in with the client's address information. Since accept takes a pointer to a struct sockaddr object, we need to cast the &client pointer to (struct sockaddr *).
    3rd arg specifies the size of the sockaddr structure*/
    if ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        puts("Connection accepted");

        while (1)
        {
            sleep(1); /*Defined in unistd.h*/
            /*Receive speed from python*/
            NewSpeed = Receive_Data_From_Client(client_sock);      /*speed now changes from 0-newspeed*/
            Distance = Distance_Travelled(NewSpeed);               /*Calculate distance travelled*/
            OdometerReading += Distance;                           /*Update odometer value*/
            EngineState = Get_Engine_Mode(CurrentSpeed, NewSpeed); /*Calculate current engine state*/
            FuelLevel = Fuel_Calculator(EngineState, FuelLevel);   /*Calculate fuel consumed*/

            CurrentSpeed = NewSpeed; /*Update current speed*/

            Send_Data_To_Socket(client_sock, OdometerReading, EngineState, (int)FuelLevel);
            /*Exit conditions*/
            if (EngineState == 0 || FuelLevel <= 0 || NewSpeed == -1)
                break;
        }

        if (read_size == 0)
        {
            puts("Client disconnected");
            fflush(stdout);
        }
        else if (read_size == -1)
        {
            perror("recv failed");
        }
    }

    return 0;
}
