#include <function.h>

/**
 * main.c
 */

/*********************global variables***********************************/
//char* SSID[3] = {"ESP_ACA5B3", "WATERFOUNTAINS", "AI-THINKER_ACA782"};
char* SSID[3] = {"Overlord", "Minh Ha", "HoangPhuc"};
char str_received[150];     //static string, receive rssi
unsigned int flag = 0;      //check for finishing receive string
unsigned int i = 0;         //positon at array str_received
int RSSI[3];                //store strenght of wifi
double distance[3];         //store distance between msp and wifi
const int FREQUENCY = 2400; //MHz, 2,4 GHz
struct position pos;

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    CLOCK_setup();
    UART_INIT();

    UART_send_string("AT+CWLAPOPT=1,4\r\n"); // get rssi only
    _delay_cycles(1000000); //1 sec

    send_cmd(SSID[0]);
    while(flag==0);
    RSSI[0] = get_RSSI(str_received);
    flag=0;

    send_cmd(SSID[1]);
    while(flag==0);
    RSSI[1] = get_RSSI(str_received);
    flag=0;

    send_cmd(SSID[2]);
    while(flag==0);
    RSSI[2] = get_RSSI(str_received);
    flag=0;

    _delay_cycles(1000000);

    distance[0]=getDistance(RSSI[0],FREQUENCY);
    distance[1]=getDistance(RSSI[1],FREQUENCY);
    distance[2]=getDistance(RSSI[2],FREQUENCY);

    _delay_cycles(1000000);

    struct position ap[3];

    ap[0].x =0;
    ap[0].y =0;

    ap[1].x=0;
    ap[1].y=940;

    ap[2].x=540;
    ap[2].y=940;

    _delay_cycles(1000000);

    pos = getPosition(distance, ap[0],ap[1],ap[2]);

    while(1){

    }
    return 0;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    if(!flag){ // co hieu ket thuc chuoi
        str_received[i]=UART_read_char();
        if(str_received[i]==')'){
            flag=1;
            i=0;
        }
        else {
            i=i+1;
        }
    }
}
