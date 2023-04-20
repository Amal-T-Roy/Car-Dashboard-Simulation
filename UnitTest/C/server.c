#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Calculations.h"




int main() {


    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server, client; /*Structure describing internet socket address*/
    char client_message[2000]; /*Array to hold*/

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    // Listen
    listen(socket_desc, 3);

    // Accept any incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
        puts("Connection accepted");


    int Distance = 0;
    float FuelConsumed = 0;
    int currentspeed,newspeed = 0;
    int EngineState = 0; /*Initially engine is off*/
    int OdometerReading = INITIAL_ODO_READING;
    float FuelLevel = INITIAL_FUEL_LEVEL; /*Full tank at start*/

    int count = 0;
    while(1) {
        sleep(1);
        //Receive speed from python
        newspeed = abs(Receive_From_Socket(client_sock)); /*speed now changes from 0-newspeed*/

       Distance = Distance_Travelled(newspeed); /*Calculate distance travelled*/
       OdometerReading += Distance;/*Update odometer value*/
       EngineState = Get_Engine_Mode(currentspeed,newspeed); /*Calculate current engine state*/
       FuelLevel = Fuel_Calculator(EngineState,FuelLevel); /*Calculate fuel consumed*/

        currentspeed = newspeed; /*Update current speed*/

        Send_Data_To_Socket(client_sock, OdometerReading ,EngineState, (int)FuelLevel);
        /*Exit conditions*/
        if(EngineState == 0 || FuelLevel <= 0 )
            break;
    }

    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if (read_size == -1) {
        perror("recv failed");
    }
}

    return 0;
}


