/*
 *  ETA/OS - IRQ chip
 *  Copyright (C) 2014, 2015   Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @addtogroup irqchip
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/types.h>
#include <etaos/list.h>
#include <etaos/irq.h>
#include <etaos/error.h>

/**
 * @brief Add a new IRQ to an IRQ chip.
 * @param chip The IRQ chip where the IRQ will be added to.
 * @param irq IRQ data which has to be added.
 * @return Error code.
 * @retval -ENOTINITIALISED if chip or irq is set to NULL.
 * @retval 0 on success.
 */
int irq_chip_add_irq(struct irq_chip *chip, struct irq_data *irq)
{
	if(chip == NULL || irq == NULL)
		return -ENOTINITIALISED;

	irq->chip = chip;
	list_add(&irq->irq_list, &chip->irqs);
	return 0;
}

/**
 * @brief Initialise a new IRQ chip.
 * @param chip Chip which has to be initialised.
 * @param name Name of the IRQ chip.
 * @return Error code.
 * @retval -ENOTINITIALISED if chip == NULL.
 * @retval -EOK on success.
 */
int irq_chip_init(struct irq_chip *chip, const char *name)
{
	if(!chip)
		return -ENOTINITIALISED;

	chip->name = name;
	chip->sleep = NULL;
	chip->resume = NULL;
	chip->chip_handle = &irq_handle;
	return -EOK;
}

/* @} */
/* @} */

