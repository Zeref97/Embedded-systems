#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <msp430.h>
#include <math.h>

/*************************store position*********************************/
struct position{
    double x;
    double y;
};

/*************************function***************************************/
void UART_send_char(unsigned char data){
    while (!(IFG2&UCA0TXIFG)); // Doi gui xong ky tu truoc
    UCA0TXBUF= data; // Moi cho phep gui ky tu tiep theo
}

void UART_send_string(char* string){
    while(*string){
        while (!(IFG2&UCA0TXIFG)); // Doi gui xong ky tu truoc
        UCA0TXBUF= *string;
        string++;
    }
}

void UART_send_int(unsigned long n)
{
     unsigned char buffer[16];
     unsigned char i,j;

     if(n == 0) {
         UART_send_char('0');
          return;
     }

     for (i = 15; i > 0 && n > 0; i--) {
          buffer[i] = (n%10)+'0';
          n /= 10;
     }

     for(j = i+1; j <= 15; j++) {
         UART_send_char(buffer[j]);
     }
}

void UART_send_float(double x, unsigned char coma)
{
    unsigned long temp;
    if(coma>4)coma=4;
    // de trong 1 ki tu o dau cho hien thi dau
    if(x<0)
    {
        UART_send_char('-');           //In so am
        x*=-1;
    }
    temp = (unsigned long)x;                                    // Chuyan thanh so nguyen.

    UART_send_int(temp);

    x=((float)x-temp);//*mysqr(10,coma);
    if(coma!=0)UART_send_char('.');    // In ra dau "."
    while(coma>0)
    {
        x*=10;
        coma--;
    }
    temp=(unsigned long)(x+0.5);    //Lam tron
    UART_send_int(temp);
}

char UART_read_char(){
    while (!(IFG2&UCA0RXIFG)); // Doi gui xong ky tu truoc
    return UCA0RXBUF; // Moi cho phep nhan ky tu tiep theo
}

void CLOCK_setup(){
    // If calibration constant erased
    if (CALBC1_1MHZ==0xFF){
        while(1);                             // do not load, trap CPU!!
    }

    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;
}

void UART_INIT(){
    P1SEL |= BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= BIT1 + BIT2;
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt ||||| bring this line to main.c directly
    _BIS_SR(GIE);
}

void send_cmd(char SSID[]){ // coi lai dum t ham nay nha
    UART_send_string("AT+CWLAP=\"");
    UART_send_string(SSID);
    UART_send_string("\"\r\n");
}

void ConnectAP(char SSID[],char password[]){
    UART_send_string("AT+CWJAP=\"");
    UART_send_string(SSID);
    UART_send_string("\",\"");
    UART_send_string(password);
    UART_send_string("\"\r\n");
}

void StartTCPServer(char IP[],char port[]){
    UART_send_string("AT+CIPSTART=\"TCP\",\"");
    UART_send_string(IP);
    UART_send_string("\",");
    UART_send_string(port);
    UART_send_string("\r\n");
}

double getDistance(double rssi_in, int freq ){
    return pow(10,(27.55 - ( 20 * log10(freq) ) - rssi_in) / 20.0);
}


struct position getPosition(double distance[], struct position ap1, struct position ap2, struct position ap3 ){
    struct position result;
    result.y= (distance[0]*distance[0] - distance[1]*distance[1] + ap2.y*ap2.y)/(2*ap2.y);
    result.x=  ( (distance[0]*distance[0] - distance[2]*distance[2] + ap3.y*ap3.y + ap3.x*ap3.x) / (2*ap3.x) ) - ( (ap3.y/ap3.x) / result.y);

    return result;
}


int get_RSSI(char s[]){   //+CWLAP:(-91)
    unsigned int j;
    for(j = 0;j< 150; j++){
        if(s[j] == '(' && s[j+1] == '-')
            return -(int)((s[j+2] - 48 )*10+(s[j+3]-48));
    }
}

#endif /*FUNCTION_H_*/
