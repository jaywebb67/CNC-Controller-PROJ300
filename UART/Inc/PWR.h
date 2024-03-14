/*
 * PWR.h
 *
 *  Created on: Feb 28, 2024
 *      Author: jaywe
 */

#ifndef PWR_H_
#define PWR_H_


#define Set_LPMode_Sleep() SCB->SCR&=~(SCB_SCR_SLEEPONEXIT_Msk|SCB_SCR_SLEEPDEEP_Msk)
#define Enter_LPMode __WFI()



#define Sleep() {Set_LPMode_Sleep(); Enter_LPMode;}


#endif /* PWR_H_ */
