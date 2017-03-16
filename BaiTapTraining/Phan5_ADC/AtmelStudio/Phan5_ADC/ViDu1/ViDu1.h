/*
* ViDu1.cpp
*
* Created: 03/07/2016 11:17:38 PM
*  Author: ThaoNguyen
*/

#define F_CPU 8000000UL
#include <avr/io.h>

#define cbi(port, bit) (port) &= ~(1 << bit)
#define sbi(port, bit) (port) |= (1 << bit)

void display(uint16_t value)
{
    uint8_t pc = 0, pd = 0;
    
    uint16_t temp = value / 1000;
    uint16_t remain = value % 1000;
    pd |= (temp & 0x0f) << 4;
    
    temp = remain / 100;
    remain = remain % 100;
    pd |= temp & 0x0f;
    
    temp = remain / 10;
    remain = remain % 10;
    pc |= ((temp & 0x0f) << 4) | (remain & 0x0f);
    
    PORTC = pc;
    PORTD = pd;
}

uint16_t adc_read(uint8_t channel)
{
    ADMUX = channel | (ADMUX & 0xf0);
    ADCSRA|=(1<<ADSC);
    while(!(ADCSRA & (1<<ADIF)));
    return ADCW;
}

int main(void)
{
    // Khởi tạo chân ra Led7
    DDRC = 0xff;
    PORTC = 0;
    DDRD = 0xff;
    PORTD = 0;
    
    // Khởi tạo ADC
    ADMUX |= (1 << REFS0);                                              // Chọn điện áp tham chiếu từ chân AREF
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Cho phép ADC hoạt động, Prescale = 128
    
    while(1)
    {
        display(adc_read(1));
    }
}