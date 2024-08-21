/*
 * spi_if.h
 *
 *  Created on: Aug 19, 2024
 *      Author: Admin
 */

#ifndef SPI_IF_H_
#define SPI_IF_H_

#include <stdint.h>

int spi_write(int fd,unsigned char *bytes,int length);
int spi_read(int fd,unsigned char *bytes,int length);
int spi_write_byte(int fd,uint8_t bytes);
uint8_t spi_read_byte(int fd);

#endif /* SPI_IF_H_ */
