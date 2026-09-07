#include "delay.h"

void delay(uint16_t time){
    while(time){
        SysTick -> CTRL = 5;
        SysTick -> LOAD = 71999;
        SysTick -> VAL = 0;
        while(!(SysTick->CTRL & (1 << 16))){}
        --time;
    }
}
