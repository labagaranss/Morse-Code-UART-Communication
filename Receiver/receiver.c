#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

//CONFIG 
#define RX PF2
#define LCD_ADDR 0x27

//  TIME BASE
volatile uint32_t ms = 0;
volatile uint32_t last_rx_time = 0;

//  TIMER0 (1ms) 
void timer0_init(void)
{
    TCCR0A = (1<<WGM01);                 // CTC mode
    OCR0A = 249;                         // 1ms @ 16MHz, prescaler 64
    TCCR0B = (1<<CS01) | (1<<CS00);      // prescaler 64
    TIMSK0 = (1<<OCIE0A);                // enable interrupt
}

ISR(TIMER0_COMPA_vect)
{
    ms++;
}

// ---------- TWI ----------
void twi_init(void)
{
    TWSR = 0x00;
    TWBR = 72;
    TWCR = (1<<TWEN);
}

void twi_start(void)
{
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR&(1<<TWINT)));
}

void twi_stop(void)
{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    _delay_ms(1);
}

void twi_write(unsigned char data)
{
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while(!(TWCR&(1<<TWINT)));
}

// LCD 
void lcd_send(unsigned char data)
{
    twi_start();
    twi_write(LCD_ADDR << 1);
    twi_write(data);
    twi_stop();
}

void lcd_pulse(unsigned char data)
{
    lcd_send(data | 0x04);
    _delay_us(1);

    lcd_send(data & ~0x04);
    _delay_us(100);
}

void lcd_write4(unsigned char nibble, unsigned char rs)
{
    unsigned char data = (nibble << 4);

    if(rs) data |= 0x01;

    data |= 0x08; // backlight

    lcd_pulse(data);
}

void lcd_cmd(unsigned char cmd)
{
    lcd_write4(cmd >> 4, 0);
    lcd_write4(cmd & 0x0F, 0);
}

void lcd_data(unsigned char data)
{
    lcd_write4(data >> 4, 1);
    lcd_write4(data & 0x0F, 1);
}

void lcd_init(void)
{
    _delay_ms(50);

    lcd_write4(0x03,0); _delay_ms(5);
    lcd_write4(0x03,0); _delay_ms(5);
    lcd_write4(0x03,0); _delay_ms(5);
    lcd_write4(0x02,0);

    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);

    _delay_ms(5);
}
uint8_t uart_rx_available(void)
{
    return !(PINF & (1<<RX)); // start bit detected (LOW)
}

// UART BIT BANG RX 
void bit_delay(void)
{
    _delay_us(104); // ~9600 baud
}

char uart_rx_char(void)
{
    char data = 0;

    while(PINF & (1<<RX));   // wait start bit

    _delay_us(156);

    for(uint8_t i=0;i<8;i++)
    {
        if(PINF & (1<<RX))
            data |= (1<<i);

        bit_delay();
    }

    bit_delay();
    return data;
}

// MORSE DECODER 
char decode_morse(char *m)
{
    if(!strcmp(m,".-")) return 'A';
    if(!strcmp(m,"-...")) return 'B';
    if(!strcmp(m,"-.-.")) return 'C';
    if(!strcmp(m,"-..")) return 'D';
    if(!strcmp(m,".")) return 'E';
    if(!strcmp(m,"..-.")) return 'F';
    if(!strcmp(m,"--.")) return 'G';
    if(!strcmp(m,"....")) return 'H';
    if(!strcmp(m,"..")) return 'I';
    if(!strcmp(m,".---")) return 'J';
    if(!strcmp(m,"-.-")) return 'K';
    if(!strcmp(m,".-..")) return 'L';
    if(!strcmp(m,"--")) return 'M';
    if(!strcmp(m,"-.")) return 'N';
    if(!strcmp(m,"---")) return 'O';
    if(!strcmp(m,".--.")) return 'P';
    if(!strcmp(m,"--.-")) return 'Q';
    if(!strcmp(m,".-.")) return 'R';
    if(!strcmp(m,"...")) return 'S';
    if(!strcmp(m,"-")) return 'T';
    if(!strcmp(m,"..-")) return 'U';
    if(!strcmp(m,"...-")) return 'V';
    if(!strcmp(m,".--")) return 'W';
    if(!strcmp(m,"-..-")) return 'X';
    if(!strcmp(m,"-.--")) return 'Y';
    if(!strcmp(m,"--..")) return 'Z';
    if(!strcmp(m,".----")) return '1';
    if(!strcmp(m,"..---")) return '2';
    if(!strcmp(m,"...--")) return '3';
    if(!strcmp(m,"....-")) return '4';
    if(!strcmp(m,".....")) return '5';
    if(!strcmp(m,"-....")) return '6';
    if(!strcmp(m,"--...")) return '7';
    if(!strcmp(m,"---..")) return '8';
    if(!strcmp(m,"----.")) return '9';
    if(!strcmp(m,"-----")) return '0';

    return '?';
}

// MAIN 
int main(void)
{
    DDRF &= ~(1<<RX);
    PORTF |= (1<<RX);   // pull-up

    twi_init();
    lcd_init();
    timer0_init();
    sei();

    lcd_cmd(0x01);
    lcd_cmd(0x80);

    char morse[10] = {0};
    uint8_t idx = 0;

    last_rx_time = ms;

    while(1)
    {
        char c;

    if(uart_rx_available())
   {
    c = uart_rx_char();
    last_rx_time = ms;
   }
   else
   {
    c = 0; // no data
   }
        last_rx_time = ms;   // update activity time

        if(c == '.' || c == '-')
        {
            if(idx < 9)
            {
                morse[idx++] = c;
                morse[idx] = '\0';
            }
        }
        else if(c == '#')
        {
            char letter = decode_morse(morse);
            lcd_data(letter);

            idx = 0;
            morse[0] = '\0';
        }
        else if(c == '/')
        {
            lcd_data(' ');
        }

        //INACTIVITY CHECK 
        if((ms - last_rx_time) >= 60000)
        {
            lcd_cmd(0x01);   // clear LCD
            lcd_cmd(0x80);   // home
            idx = 0;
            morse[0] = '\0';
            last_rx_time = ms;
        }
    }
}