/**
 * Daytime based sideboard light controller which should activate light when some drawer is open.
 *
 * @author Igor Martens
 * @since 30.12.2018
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

void setupADC() {
    ADMUX |=
        // left shift result
        (1 << ADLAR) |

        // AREF, Internal V ref turned off
        (0 << REFS1) |
        (1 << REFS0) |

        // use ADC0 for input (PC0), MUX3..0 = 0000
        (0 << MUX3)  |
        (0 << MUX2)  |
        (0 << MUX1)  |
        (0 << MUX0);

    ADCSRA =
        // Enable ADC
        (1 << ADEN)  |

        // no prescaler is required, we use only 8bit resolution
        (0 << ADPS2) |
        (0 << ADPS1) |
        (0 << ADPS0);
}

/** DEBUG ONLY **/
void setupRegisterPorts() {
    // Setup output ports
    DDRD |= (1<<DDD5);  // SER Port
    DDRD |= (1<<DDD6);  // SRCLK Port
    DDRD |= (1<<DDD7);  // RCLK Port
}

/**
 * DEBUG ONLY
 * Push the given byte to the register and finally execute latch.
 * The left bit will be pushed first.
 */
void pushByteAndLatch(uint8_t byte) {
    for (uint8_t i=0; i<8; ++i) {
        (byte & 128) ? PORTD |= (1 << PD5) : PORTD &= ~(1 << PD5);
        PORTD |= (1 << PD6);
        PORTD &= ~(1 << PD6);
        byte = byte << 1;
    }

    PORTD |= (1 << PD7);
    PORTD &= ~(1 << PD7);
}

void setup() {
    setupADC();
    setupRegisterPorts(); /** DEBUG ONLY **/
}

int main(void) {
    setup();

    while(1) {
        ADCSRA |= (1 << ADSC);         // start ADC measurement
        while (ADCSRA & (1 << ADSC));  // wait till conversion complete

        uint8_t val = ADCH;            // read value from ADC
        pushByteAndLatch(val);
    }

    return 0;
}
