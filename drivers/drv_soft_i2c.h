/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-01-07     tyustli   first version
 */

#ifndef __DRV_I2C__
#define __DRV_I2C__

#include <rtthread.h>
#include <rtdevice.h>

/* stm32 config class */
struct stm32_soft_i2c_config
{
    rt_uint8_t scl;
    rt_uint8_t sda;
    const char *bus_name;
};

/* stm32 i2c dirver class */
struct stm32_i2c
{
    struct rt_i2c_bit_ops ops;
    struct rt_i2c_bus_device i2c2_bus;
};

int rt_hw_i2c_init(char *name, rt_uint8_t scl, rt_uint8_t sda);

#endif

/****************** end of file ***************/
