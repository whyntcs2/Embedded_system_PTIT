#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "button.h"


int main(){
    button_cfg();
    led_cfg();
    while(1){
        button_debouce();
    }
}
