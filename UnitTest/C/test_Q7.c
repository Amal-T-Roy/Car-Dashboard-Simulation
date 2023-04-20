#include<check.h>
#include "Calculations.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1



/*Test 1*/
START_TEST(test_Distance_Travelled) {
    float  distance = 0; 
    distance = Distance_Travelled(36);
    ck_assert_int_eq(distance, 10);
} END_TEST

/*Test 2*/
START_TEST(test_Get_Engine_Mode) {
    int mode = 0;
    mode = Get_Engine_Mode(10,20);
    ck_assert_int_eq(mode, 3); /*Accelerating*/
} END_TEST

/*Test 3*/
START_TEST(test_Fuel_Calculator) {
    int EngineState = 1;
    float FuelLevel = 50;
    FuelLevel = Fuel_Calculator(EngineState,FuelLevel);
    ck_assert_int_eq((int)FuelLevel, 49); /*Accelerating*/
} END_TEST

/*Test 4*/
START_TEST(test_Send_Data_To_Socket)
{
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Set up the server address
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_addr.sin_port = htons(8080);
    
    // Connect to the server
    int status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    ck_assert_int_eq(status, -1);
    
    // Call the function being tested
    int val1 = 1, val2 = 2, val3 = 3;
    int result = Send_Data_To_Socket(sockfd, val1, val2, val3);
    ck_assert(result == 0 || result == -1);
    
    // Close the socket
    close(sockfd);
}
END_TEST


Suite *my_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("MySuite");

    /* Core test case */
    tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_Distance_Travelled);
    tcase_add_test(tc_core, test_Get_Engine_Mode);
    tcase_add_test(tc_core, test_Fuel_Calculator);
    tcase_add_test(tc_core, test_Send_Data_To_Socket);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = my_suite();
    sr = srunner_create(s);

    //srunner_run_all(sr, CK_NORMAL);
    srunner_run_all(sr, CK_VERBOSE); /*Verbose gives more info about errors and warnings*/
    number_failed = srunner_ntests_failed(sr); /*Returns number of failed tests*/
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
