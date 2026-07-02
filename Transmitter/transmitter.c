#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define BTN PF0
#define TX  PF1

#define DOT_THRESHOLD 300

// ---------------- UART TX ----------------

void bit_delay(void)
{
    _delay_us(104);
}

void uart_tx_char(char data)
{
    uint8_t i;

    // Start bit
    PORTF &= ~(1 << TX);
    bit_delay();

    // Data bits (LSB First)
    for(i = 0; i < 8; i++)
    {
        if(data & (1 << i))
            PORTF |= (1 << TX);
        else
            PORTF &= ~(1 << TX);

        bit_delay();
    }

    // Stop bit
    PORTF |= (1 << TX);
    bit_delay();
}

// ---------------- MAIN ----------------

int main(void)
{
    // TX Output
    DDRF |= (1 << TX);

    // Button Input
    DDRF &= ~(1 << BTN);

    // Pullup Enable
    PORTF |= (1 << BTN);

    // UART Idle HIGH
    PORTF |= (1 << TX);

    uint16_t press_time = 0;
    uint16_t idle_time = 0;

    uint8_t char_sent = 0;
    uint8_t word_sent = 0;

    while(1)
    {
        // Button Pressed
        if(!(PINF & (1 << BTN)))
        {
            press_time = 0;

            while(!(PINF & (1 << BTN)))
            {
                _delay_ms(10);
                press_time += 10;
            }

            _delay_ms(20);   // debounce

            if(press_time < DOT_THRESHOLD)
            {
                uart_tx_char('.');
            }
            else
            {
                uart_tx_char('-');
            }

            idle_time = 0;
            char_sent = 0;
            word_sent = 0;
        }
else
{
    _delay_ms(10);
    idle_time += 10;

    // WORD END FIRST (higher priority)
    if(idle_time >= 2000 && !word_sent)
    {
        uart_tx_char('/');   // word end
        word_sent = 1;
        char_sent = 1;       // prevent '#'
    }

    // CHARACTER END
    else if(idle_time >= 1000 && !char_sent)
    {
        uart_tx_char('#');   // char end
        char_sent = 1;
    }
}
    }
}