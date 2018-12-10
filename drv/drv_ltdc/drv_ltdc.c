/*
 * memwa2 display driver
 *
 * Copyright (c) 2016 Mathias Edman <mail@dicetec.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 * USA
 *
 */


/**
 * This driver handles all graphics and configuration.
 */

#include "drv_ltdc.h"
#include "drv_led.h"
#include "stm32h7xx_hal_ltdc.h"
#include "stm32h7xx_hal_dma2d.h"
#include <string.h>

#define VGA_WIDTH  ((uint16_t)640)
#define VGA_HEIGHT ((uint16_t)480)
#define VGA_HSYNC  ((uint16_t)96)
#define VGA_HBP    ((uint16_t)48)
#define VGA_HFP    ((uint16_t)16)
#define VGA_VSYNC  ((uint16_t)2)
#define VGA_VBP    ((uint16_t)31)
#define VGA_VFP    ((uint16_t)11)

#define SVGA_WIDTH  ((uint16_t)800)
#define SVGA_HEIGHT ((uint16_t)600)
#define SVGA_HSYNC  ((uint16_t)128)
#define SVGA_HBP    ((uint16_t)88)
#define SVGA_HFP    ((uint16_t)40)
#define SVGA_VSYNC  ((uint16_t)4)
#define SVGA_VBP    ((uint16_t)23)
#define SVGA_VFP    ((uint16_t)1)

#define DISP_MAIN_LAYER       0
#define DISP_EMU_LAYER        1

#define CLUT_TEXT_BG_POS      16
#define CLUT_TEXT_FG_POS      17
#define CLUT_MARKER_POS       18
#define CLUT_MAX              19

LTDC_HandleTypeDef g_ltdc_handle; /* used by irq.c */
static LTDC_LayerCfgTypeDef g_ltdc_layer_cfg_a[2];
static uint32_t g_memory_addr_a[3];

static uint32_t clut_a[CLUT_MAX] =
{
    0x000000, /* black */
    0xFFFFFF, /* white */
    0x354374, /* red */
    0xBAAC7C, /* cyan */
    0x90487B, /* violet */
    0x4F9764, /* green */
    0x853240, /* blue */
    0x7ACDBF, /* yellow */
    0x2F5B7B, /* orange */
    0x00454f, /* brown */
    0x6572a3, /* light red */
    0x505050, /* grey 1 */
    0x787878, /* grey 2 */
    0x8ed7a4, /* light green */
    0xbd6a78, /* light blue */
    0x9f9f9f, /* grey 3 */
    0x110201, /* text background */
    0xed9f8f, /* text forground */
    0xfb1b15, /* marker */
};

void drv_ltdc_init(ltdc_mode_t ltdc_mode)
{
    drv_led_toggle_limit_green(0);

    switch(ltdc_mode)
    {
        case LTDC_MODE_VGA:
            /* Timing configuration */
            g_ltdc_handle.Init.HorizontalSync = (VGA_HSYNC - 1);
            g_ltdc_handle.Init.VerticalSync = (VGA_VSYNC - 1);
            g_ltdc_handle.Init.AccumulatedHBP = (VGA_HSYNC + VGA_HBP - 1);
            g_ltdc_handle.Init.AccumulatedVBP = (VGA_VSYNC + VGA_VBP - 1);  
            g_ltdc_handle.Init.AccumulatedActiveH = (VGA_HEIGHT + VGA_VSYNC + VGA_VBP - 1);
            g_ltdc_handle.Init.AccumulatedActiveW = (VGA_WIDTH + VGA_HSYNC + VGA_HBP - 1);
            g_ltdc_handle.Init.TotalHeigh = (VGA_HEIGHT + VGA_VSYNC + VGA_VBP + VGA_VFP - 1);
            g_ltdc_handle.Init.TotalWidth = (VGA_WIDTH + VGA_HSYNC + VGA_HBP + VGA_HFP - 1);

            /* Background value */
            g_ltdc_handle.Init.Backcolor.Blue = 0;
            g_ltdc_handle.Init.Backcolor.Green = 0;
            g_ltdc_handle.Init.Backcolor.Red = 0;

            /* Polarity */
            g_ltdc_handle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
            g_ltdc_handle.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
            g_ltdc_handle.Init.DEPolarity = LTDC_DEPOLARITY_AL;  
            g_ltdc_handle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
            g_ltdc_handle.Instance = LTDC;
            break;
        case LTDC_MODE_SVGA:
            /* Timing configuration */
            g_ltdc_handle.Init.HorizontalSync = (SVGA_HSYNC - 1);
            g_ltdc_handle.Init.VerticalSync = (SVGA_VSYNC - 1);
            g_ltdc_handle.Init.AccumulatedHBP = (SVGA_HSYNC + SVGA_HBP - 1);
            g_ltdc_handle.Init.AccumulatedVBP = (SVGA_VSYNC + SVGA_VBP - 1);
            g_ltdc_handle.Init.AccumulatedActiveH = (SVGA_HEIGHT + SVGA_VSYNC + SVGA_VBP - 1);
            g_ltdc_handle.Init.AccumulatedActiveW = (SVGA_WIDTH + SVGA_HSYNC + SVGA_HBP - 1);
            g_ltdc_handle.Init.TotalHeigh = (SVGA_HEIGHT + SVGA_VSYNC + SVGA_VBP + SVGA_VFP - 1);
            g_ltdc_handle.Init.TotalWidth = (SVGA_WIDTH + SVGA_HSYNC + SVGA_HBP + SVGA_HFP - 1);

            /* Background value */
            g_ltdc_handle.Init.Backcolor.Blue = 0;
            g_ltdc_handle.Init.Backcolor.Green = 0;
            g_ltdc_handle.Init.Backcolor.Red = 0;

            /* Polarity */
            g_ltdc_handle.Init.HSPolarity = LTDC_HSPOLARITY_AH;
            g_ltdc_handle.Init.VSPolarity = LTDC_VSPOLARITY_AH;
            g_ltdc_handle.Init.DEPolarity = LTDC_DEPOLARITY_AH;
            g_ltdc_handle.Init.PCPolarity = LTDC_PCPOLARITY_IIPC;
            g_ltdc_handle.Instance = LTDC;
    }

    HAL_LTDC_Init(&g_ltdc_handle);

    HAL_LTDC_ConfigCLUT(&g_ltdc_handle, clut_a, CLUT_MAX, 0);
    HAL_LTDC_ConfigCLUT(&g_ltdc_handle, clut_a, CLUT_MAX, 1);

    HAL_LTDC_EnableCLUT(&g_ltdc_handle, 0);
    HAL_LTDC_EnableCLUT(&g_ltdc_handle, 1);

    HAL_LTDC_ProgramLineEvent(&g_ltdc_handle, 200);
    __HAL_LTDC_DISABLE_IT(&g_ltdc_handle, LTDC_IT_LI); /* Disable line events for now */
}

void *drv_ltdc_get_layer(uint8_t layer)
{
    return (void *)(g_ltdc_handle.LayerCfg[layer].FBStartAdress);
}

void drv_ltdc_set_memory(uint8_t layer, uint32_t memory)
{
    g_memory_addr_a[layer] = memory;
}

void drv_ltdc_set_layer(uint8_t layer,
                    uint32_t window_x0,
                    uint32_t window_x1,
                    uint32_t window_y0,
                    uint32_t window_y1,
                    uint32_t buffer_x,
                    uint32_t buffer_y,
                    uint8_t alpha,
                    uint32_t pixel_format)
{
    /* Layer Init */
    g_ltdc_layer_cfg_a[layer].WindowX0 = window_x0;
    g_ltdc_layer_cfg_a[layer].WindowX1 = window_x1;
    g_ltdc_layer_cfg_a[layer].WindowY0 = window_y0;
    g_ltdc_layer_cfg_a[layer].WindowY1 = window_y1;
    g_ltdc_layer_cfg_a[layer].Alpha = alpha;
    g_ltdc_layer_cfg_a[layer].Alpha0 = 0;
    g_ltdc_layer_cfg_a[layer].Backcolor.Blue = 0;
    g_ltdc_layer_cfg_a[layer].Backcolor.Green = 0;
    g_ltdc_layer_cfg_a[layer].Backcolor.Red = 0;
    g_ltdc_layer_cfg_a[layer].BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    g_ltdc_layer_cfg_a[layer].BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    g_ltdc_layer_cfg_a[layer].ImageWidth = buffer_x;
    g_ltdc_layer_cfg_a[layer].ImageHeight = buffer_y;
    g_ltdc_layer_cfg_a[layer].PixelFormat = pixel_format;
    g_ltdc_layer_cfg_a[layer].FBStartAdress = g_memory_addr_a[layer];

    HAL_LTDC_ConfigLayer(&g_ltdc_handle, &g_ltdc_layer_cfg_a[layer], layer);
}

void drv_ltdc_activate_layer(uint8_t layer)
{
    __HAL_LTDC_LAYER_ENABLE(&g_ltdc_handle, layer);
    __HAL_LTDC_RELOAD_CONFIG(&g_ltdc_handle);
}

void drv_ltdc_deactivate_layer(uint8_t layer)
{
    __HAL_LTDC_LAYER_DISABLE(&g_ltdc_handle, layer);
    __HAL_LTDC_RELOAD_CONFIG(&g_ltdc_handle);
}

void drv_ltdc_fill_layer(uint8_t layer, uint32_t color)
{
    uint32_t i;

    for(i = 0; i < g_ltdc_layer_cfg_a[layer].ImageWidth * g_ltdc_layer_cfg_a[layer].ImageHeight; i++)
    {
        if(g_ltdc_layer_cfg_a[layer].PixelFormat == LTDC_PIXEL_FORMAT_L8)
        {
            *((uint8_t *)g_ltdc_layer_cfg_a[layer].FBStartAdress + i) = color;
        }
        else if(g_ltdc_layer_cfg_a[layer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
        {
            *((uint16_t *)g_ltdc_layer_cfg_a[layer].FBStartAdress + i) = color;
        }
        else if(g_ltdc_layer_cfg_a[layer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
        {
            *((uint32_t *)g_ltdc_layer_cfg_a[layer].FBStartAdress + i) = color;
        }
    }
}

void drv_ltdc_move_layer(uint8_t layer, uint32_t x, uint32_t y)
{
    HAL_LTDC_SetWindowPosition(&g_ltdc_handle, x, y, layer);
}

void drv_ltdc_flip_buffer(uint8_t **done_buffer_pp)
{
    /* Set layer to be displayed */
    HAL_LTDC_SetAddress(&g_ltdc_handle, (uint32_t)*done_buffer_pp, DISP_EMU_LAYER);

    /* And shift buffer */
    if((uint32_t)*done_buffer_pp == g_memory_addr_a[DRV_LTDC_ADDR_BUFFER1])
    {
        *done_buffer_pp = (uint8_t *)g_memory_addr_a[DRV_LTDC_ADDR_BUFFER2];
    }
    else
    {
        *done_buffer_pp = (uint8_t *)g_memory_addr_a[DRV_LTDC_ADDR_BUFFER1];
    }

    drv_led_toggle_green();
}

void drv_ltdc_set_clut_table(uint32_t *clut_p)
{
    memcpy(clut_a, clut_p, CLUT_MAX * sizeof(uint32_t));
    HAL_LTDC_ConfigCLUT(&g_ltdc_handle, clut_a, CLUT_MAX, 0);
    HAL_LTDC_ConfigCLUT(&g_ltdc_handle, clut_a, CLUT_MAX, 1);
}

void drv_ltdc_enable_clut(uint8_t layer)
{
    HAL_LTDC_EnableCLUT(&g_ltdc_handle, layer);
}

void drv_ltdc_disable_clut(uint8_t layer)
{
    HAL_LTDC_DisableCLUT(&g_ltdc_handle, layer);
}

void HAL_LTDC_ErrorCallback(LTDC_HandleTypeDef *hltdc)
{
    switch((uint32_t)hltdc->ErrorCode)
    {
        case HAL_LTDC_ERROR_NONE:
            break;
        case HAL_LTDC_ERROR_TE:
            serv_term_printf(SERV_TERM_PRINT_TYPE_ERROR, "HAL LTDC error (transfer error)");
            break;
        case HAL_LTDC_ERROR_FU:
            serv_term_printf(SERV_TERM_PRINT_TYPE_ERROR, "HAL LTDC error (fifo underrun)");
            break;
        case HAL_LTDC_ERROR_TIMEOUT:
            serv_term_printf(SERV_TERM_PRINT_TYPE_ERROR, "HAL LTDC error (timeout error)");
            break;
    }

    __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_TE);
    __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_FU);
    hltdc->State = HAL_LTDC_STATE_READY;
    hltdc->ErrorCode = HAL_LTDC_ERROR_NONE;
}

void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc)
{
    serv_term_printf(SERV_TERM_PRINT_TYPE_ERROR, "HAL LTDC line callback!");
    //HAL_LTDC_ProgramLineEvent(hltdc, 200);
}
