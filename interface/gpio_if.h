/*
 * gpio_if.h
 *
 *  Created on: Aug 19, 2024
 *      Author: Admin
 */

#ifndef GPIO_IF_H_
#define GPIO_IF_H_

/**
 * GPIO config
**/
#define DEV_RST_PIN     18
#define DEV_CS_PIN      22
#define DEV_CS1_PIN     23
#define DEV_DRDY_PIN    17
#define GPIO_FILE 0

int gpio_pin_write(int fd,int pin,int level);
int gpio_pin_read(int fd,int pin);


#endif /* GPIO_IF_H_ */
