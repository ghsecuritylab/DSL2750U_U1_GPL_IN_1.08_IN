/* Copyright (c) 2005 freescale semiconductor
 * Copyright (c) 2005 MontaVista Software
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef _USB_RTL8652_H
#define _USB_RTL8652_H
#define PADDR(addr)  ((addr) & 0x1FFFFFFF)
#define REG32(reg)   (*((volatile unsigned int *)(reg)))
#define OHCI_RTL8652_USB_BASE 0xb8020000
#define OHCI_RTL8652_USB_REG_SIZE sizeof(struct ohci_regs)
/* offsets for the non-ehci registers in the FSL SOC USB controller */
#define EHCI_RTL8652_USB_BASE 0xb8021000
#define EHCI_RTL8652_USB_REG_SIZE sizeof(struct ehci_regs)
#define RTL8652_USB_IRQ 9
#endif				/* _USB_RTL8652_H_ */
