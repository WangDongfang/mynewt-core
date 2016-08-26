/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <hal/hal_bsp.h>
#include <mcu/nrf51_hal.h>
#include "bsp/bsp.h"
#include <nrf_adc.h>
#include <nrf_drv_adc.h>
#include <os/os_dev.h>
#include <uart/uart.h>
#include <uart_hal/uart_hal.h>

static const struct nrf51_uart_cfg uart_cfg = {
    .suc_pin_tx = 9,
    .suc_pin_rx = 11,
    .suc_pin_rts = 8,
    .suc_pin_cts = 10
};

static struct uart_dev hal_uart0;

/*
 * What memory to include in coredump.
 */
static const struct bsp_mem_dump dump_cfg[] = {
    [0] = {
        .bmd_start = &_ram_start,
        .bmd_size = RAM_SIZE
    }
};

const struct hal_flash *
bsp_flash_dev(uint8_t id)
{
    /*
     * Internal flash mapped to id 0.
     */
    if (id != 0) {
        return NULL;
    }
    return &nrf51_flash_dev;
}

const struct bsp_mem_dump *
bsp_core_dump(int *area_cnt)
{
    *area_cnt = sizeof(dump_cfg) / sizeof(dump_cfg[0]);
    return dump_cfg;
}

uint16_t
bsp_get_refmv(void *cfgdata)
{
    uint16_t refmv;
    nrf_drv_adc_channel_config_t *cc;
    nrf_adc_config_reference_t cref;

    cc = (nrf_drv_adc_channel_config_t *) cfgdata;
    cref = cc->reference | (cc->external_reference << ADC_CONFIG_EXTREFSEL_Pos);
    switch (cref) {
        case NRF_ADC_CONFIG_REF_VBG:
            refmv = 1200;
            break;
/* XXX: fill these out if using an external reference */
#if 0
        case NRF_ADC_CONFIG_REF_EXT_REF0:
            refmv = XXX;
            break;
        case NRF_ADC_CONFIG_REF_EXT_REF1:
            refmv = YYY;
            break;
#endif
        case NRF_ADC_CONFIG_REF_SUPPLY_ONE_HALF:
            refmv = 2800 / 2;
            break;
        case NRF_ADC_CONFIG_REF_SUPPLY_ONE_THIRD:
            refmv = 2800 / 3;
            break;
        default:
            refmv = 0;
            break;
    }

    return refmv;
}

void
bsp_hal_init(void)
{
    int rc;

    rc = os_dev_create((struct os_dev *)&hal_uart0, "uart0",
      OS_DEV_INIT_PRIMARY, 0, uart_hal_init, (void *)&uart_cfg);
    assert(rc == 0);
}
