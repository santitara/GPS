/* 
 * File:   delay.h
 * Author: casees
 *
 * Created on 16 de febrero de 2016, 9:40
 */

#ifndef DELAY_H
#define	DELAY_H

#ifdef	__cplusplus
extern "C" {
#endif

    
void delay_10us_x(unsigned int n);  //function to make a 10 us delay
void delay_ms(unsigned int msec);    //function to make a msec ms delay



#ifdef	__cplusplus
}
#endif

#endif	/* DELAY_H */

