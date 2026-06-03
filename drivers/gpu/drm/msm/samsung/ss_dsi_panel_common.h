/*
 * =================================================================
 *
 *
 *	Description:  samsung display common file
 *
 *	Author: jb09.kim
 *	Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
Copyright (C) 2012, Samsung Electronics. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 */

#ifndef SS_DSI_PANEL_COMMON_H
#define SS_DSI_PANEL_COMMON_H

#ifndef SUPPORT_PANEL_REVISION
#define SUPPORT_PANEL_REVISION 4  /* Extins la 4 pentru a acoperi reviziile noi */
#endif

/* REPARARE: Mărit de la 32 la 128 pentru a nu face overflow cu noile enum-uri */
#ifndef SS_DSI_CMD_SET_MAX
#define SS_DSI_CMD_SET_MAX     128 
#define SS_DSI_CMD_SET_START   20
#endif

#ifndef RX_CMD_START
#define RX_CMD_START           0
#define RX_CMD_END             100
#define RX_SELF_DISP_DEBUG     101
#define RX_SELF_MASK_CHECK     102
#endif

/* REPARARE: Mutat totul într-un singur Enum curat pentru a evita suprapunerile de ID-uri */
enum samsung_custom_cmd_sets {
	TX_AID_SUBDIVISION = SS_DSI_CMD_SET_START,
	TX_PAC_AID_SUBDIVISION,
	TX_IRC_SUBDIVISION,
	TX_PAC_IRC_SUBDIVISION,

	TX_GAMMA_MODE1_INTERPOLATION,
	TX_COPR_ENABLE,
	TX_DYNAMIC_HLPM_ENABLE,
	TX_DYNAMIC_HLPM_DISABLE,
	TX_SELF_IDLE_AOD_ENTER,
	TX_SELF_IDLE_TIMER_ON,
	TX_SELF_IDLE_MOVE_ON_PATTERN1,
	TX_SELF_IDLE_TIMER_OFF,
	TX_SELF_IDLE_MOVE_OFF,
	TX_SELF_IDLE_AOD_EXIT,
	TX_CABC_OFF_DUTY,
	TX_CABC_ON,
	TX_CABC_ON_DUTY,
	TX_CABC_OFF,

	/* REPARARE: Adăugare comenzi MCD și Teste scurtcircuit sub-identificatori lipsă */
	TX_MCD_ON,
	TX_MCD_OFF,
	TX_MICRO_SHORT_TEST_ON,
	TX_MICRO_SHORT_TEST_OFF,
	TX_GRAY_SPOT_TEST_ON,
	TX_GRAY_SPOT_TEST_OFF,
	TX_ISC_DEFECT_TEST_ON,
	TX_ISC_DEFECT_TEST_OFF,

	/* REPARARE: Adăugare identificatori de securitate POC */
	RX_POC_CHECKSUM,
	RX_POC_STATUS,

	/* REPARARE: Adăugare comenzi pentru Flash Gamma (Bliț Cameră) */
	TX_FLASH_GAMMA_PRE1,
	TX_FLASH_GAMMA_PRE2,
	TX_FLASH_GAMMA,
	TX_FLASH_GAMMA_POST,
	RX_FLASH_GAMMA
};

#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/leds.h>
#include <linux/err.h>
#include <linux/lcd.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/ctype.h>
#include <asm/div64.h>
#include <linux/interrupt.h>
#include <linux/msm-bus.h>
#include <linux/sched.h>
#include <linux/dma-buf.h>
#include <linux/debugfs.h>
#include <linux/wakelock.h>
#include <linux/miscdevice.h>
#include <linux/reboot.h>
#include <video/mipi_display.h>
#include <linux/dev_ril_bridge.h>
#include <linux/regulator/consumer.h>
#include <linux/self_display/self_display.h>

#include "dsi_display.h"
#include "dsi_panel.h"
#include "sde_kms.h"
#include "sde_connector.h"
#include "sde_encoder.h"
#include "sde_encoder_phys.h"

#include "ss_ddi_spi_common.h"
#include "ss_dpui_common.h"
#include "ss_dsi_panel_sysfs.h"
#include "ss_dsi_panel_debug.h"
#include "ss_ddi_poc_common.h"
#include "ss_copr_common.h"
#include "ss_interpolation_common.h"
#include "ss_flash_table_data_common.h"
#include <linux/panel_notify.h>

#if defined(CONFIG_SEC_DEBUG)
#include <linux/sec_debug.h>
#endif
#if defined(CONFIG_SEC_BSP)
#include <linux/sec_param.h>
#endif

/* REPARARE: Structură extinsă local pentru a introduce suportul de revizii cerut la linia 2054 */
struct ss_dsi_panel_cmd_set_extended {
	enum dsi_cmd_set_type type;
	int count;
	struct dsi_cmd_desc *cmds;
	void *cmd_set_rev[SUPPORT_PANEL_REVISION]; 
};

extern bool enable_pr_debug;
#define LOG_KEYWORD "[SDE]"
#define LCD_DEBUG(X, ...)	\
		do {	\
			if (enable_pr_debug)	\
				pr_info("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__);\
			else \
				pr_debug("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__);\
		} while (0)	\

#define LCD_INFO(X, ...) pr_info("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__)
#define LCD_INFO_ONCE(X, ...) pr_info_once("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__)
#define LCD_ERR(X, ...) pr_err("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__)

#define MAX_PANEL_NAME_SIZE 100
#define PARSE_STRING 64
#define MAX_EXTRA_POWER_GPIO 4
#define MAX_BACKLIGHT_TFT_GPIO 4
#define OSC_TE_FITTING_LUT_MAX 2
#define MAX_INTF_NUM 2
#define MAX_CELL_ID 11
#define MAX_OCTA_ID 20
#define PBA_ID 0xFFFFFF
#define ELVSS_INTERPOLATION_TEMPERATURE -20
#define ENTER_HBM_CE_LUX 40000
#define MAX_ESD_GPIO 2
#define BASIC_FB_PANLE_TYPE 0x01
#define NEW_FB_PANLE_TYPE 0x00
#define OTHERLINE_WORKQ_DEALY 900
#define OTHERLINE_WORKQ_CNT 70
#define MDNIE_TUNE_MAX_SIZE 6

extern int poweroff_charging;
#define USE_CURRENT_BL_LEVEL 0xFFFFFF

enum PANEL_LEVEL_KEY {
	LEVEL_KEY_NONE = 0,
	LEVEL0_KEY = BIT(0),
	LEVEL1_KEY = BIT(1),
	LEVEL2_KEY = BIT(2),
	POC_KEY = BIT(3),
};

enum backlight_origin {
	BACKLIGHT_NORMAL,
	BACKLIGHT_FINGERMASK_ON,
	BACKLIGHT_FINGERMASK_OFF,
};

enum mipi_samsung_cmd_map_list {
	PANEL_CMD_MAP_NULL,
	PANEL_CMD_MAP_MAX,
};

enum { MIPI_RESUME_STATE, MIPI_SUSPEND_STATE };
enum { ACL_OFF, ACL_30, ACL_15, ACL_50 };
enum {
	TE_FITTING_DONE = BIT(0),
	TE_CHECK_ENABLE = BIT(1),
	TE_FITTING_REQUEST_IRQ = BIT(3),
	TE_FITTING_STEP1 = BIT(4),
	TE_FITTING_STEP2 = BIT(5),
};

#define SAMSUNG_DISPLAY_PINCTRL0_STATE_DEFAULT "samsung_display_gpio_control0_default"
#define SAMSUNG_DISPLAY_PINCTRL0_STATE_SLEEP  "samsung_display_gpio_control0_sleep"
#define SAMSUNG_DISPLAY_PINCTRL1_STATE_DEFAULT "samsung_display_gpio_control0_default"
#define SAMSUNG_DISPLAY_PINCTRL1_STATE_SLEEP  "samsung_display_gpio_control0_sleep"

enum { SAMSUNG_GPIO_CONTROL0, SAMSUNG_GPIO_CONTROL1 };
enum { HALL_IC_OPEN, HALL_IC_CLOSE, HALL_IC_UNDEFINED };
#define LCD_FLIP_NOT_REFRESH	BIT(8)

enum IRC_MODE {
	IRC_MODERATO_MODE = 0,
	IRC_FLAT_GAMMA_MODE = 1,
	IRC_MAX_MODE,
};

enum { VDDM_ORG = 0, VDDM_LV, VDDM_HV, MAX_VDDM };
enum { MIPI_TX_TYPE_GRAM, MIPI_TX_TYPE_SIDERAM };

struct te_fitting_lut { int te_duration; int value; };
struct osc_te_fitting_info {
	unsigned int status;
	long long te_duration;
	long long *te_time;
	int sampling_rate;
	struct completion te_check_comp;
	struct work_struct work;
	struct te_fitting_lut *lut[OSC_TE_FITTING_LUT_MAX];
};

struct lpm_pwr_ctrl {
	bool support_lpm_pwr_ctrl;
	char lpm_pwr_ctrl_supply_name[32];
	int lpm_pwr_ctrl_supply_min_v;
	int lpm_pwr_ctrl_supply_max_v;
	char lpm_pwr_ctrl_elvss_name[32];
	int lpm_pwr_ctrl_elvss_lpm_v;
	int lpm_pwr_ctrl_elvss_normal_v;
};

struct lpm_info {
	u8 origin_mode; u8 ver; u8 mode; u8 hz;
	int lpm_bl_level; bool esd_recovery;
	struct mutex lpm_lock;
	struct lpm_pwr_ctrl lpm_pwr;
};

struct clk_timing_table { int tab_size; int *clk_rate; };
struct clk_sel_table { int tab_size; int *rat; int *band; int *from; int *end; int *target_clk_idx; };
struct rf_info { u8 rat; u32 band; u32 arfcn; } __packed;

struct dyn_mipi_clk {
	struct notifier_block notifier;
	struct mutex dyn_mipi_lock;
	struct workqueue_struct *change_clk_wq;
	struct work_struct change_clk_work;
	struct clk_sel_table clk_sel_table;
	struct clk_timing_table clk_timing_table;
	struct rf_info rf_info;
	int is_support; int force_idx;
};

struct cmd_map { int *bl_level; int *cmd_idx; int size; };
enum CD_MAP_TABLE_LIST { NORMAL, PAC_NORMAL, HBM, PAC_HBM, AOD, HMT, GAMMA_MODE2_NORMAL, MULTI_TO_ONE_NORMAL, CD_MAP_TABLE_MAX };

struct candela_map_table {
	int tab_size; int *scaled_idx; int *idx; int *from; int *end;
	int *cd; int *interpolation_cd; int *gamma_mode2_cd;
	int min_lv; int max_lv; int *auto_level;
};

struct samsung_display_dtsi_data {
	bool samsung_lp11_init; bool samsung_tcon_clk_on_support; bool samsung_esc_clk_128M; bool samsung_osc_te_fitting;
	bool samsung_support_factory_panel_swap; u32 samsung_power_on_reset_delay; u32 samsung_dsi_off_reset_delay;
	u32 samsung_lpm_init_delay; u8 samsung_delayed_display_on; bool samsung_anapass_power_seq; int samsung_tcon_rdy_gpio;
	bool samsung_cmds_unicast; u32 samsung_reduce_display_on_time; u32 samsung_dsi_force_clock_lane_hs;
	u32 samsung_wait_after_reset_delay; u32 samsung_wait_after_sleep_out_delay; u32 samsung_finger_print_irq_num;
	u32 samsung_home_key_irq_num; int samsung_osc_te_fitting_cmd_index[2]; int backlight_tft_gpio[MAX_BACKLIGHT_TFT_GPIO];

	/* REPARARE: Folosim structura extinsă local pentru a mapa corect array-ul de comenzi */
	struct ss_dsi_panel_cmd_set_extended cmd_sets[SS_DSI_CMD_SET_MAX];

	struct candela_map_table candela_map_table[CD_MAP_TABLE_MAX][SUPPORT_PANEL_REVISION];
	struct cmd_map aid_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map vint_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map acl_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map elvss_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map smart_acl_elvss_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map caps_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map hmt_reverse_aid_map_table[SUPPORT_PANEL_REVISION];

	bool disp_en_gpio_use; bool panel_lpm_enable; bool hmt_enabled;
	int tft_common_support; int backlight_gpio_config; int pwm_ap_support;
	const char *tft_module_name; const char *panel_vendor; const char *disp_model;
	int hbm_ce_text_mode_support; int blic_discharging_delay_tft; int cabc_delay;
	int hbm_brightness_step; int normal_brightness_step; int hmd_brightness_step;
	int gamma_size; int aor_size; int vint_size; int elvss_size; int irc_size;
	int flash_table_hbm_aor_offset; int flash_table_hbm_vint_offset; int flash_table_hbm_elvss_offset; int flash_table_hbm_irc_offset;
	int flash_table_normal_gamma_offset; int flash_table_normal_aor_offset; int flash_table_normal_vint_offset; int flash_table_normal_elvss_offset; int flash_table_normal_irc_offset;
	int flash_table_hmd_gamma_offset; int flash_table_hmd_aor_offset;

	bool flash_gamma_support; char flash_read_intf[10]; int flash_gamma_data_read_addr_len; int *flash_gamma_data_read_addr;
	int flash_gamma_write_check_address; int flash_gamma_bank_start_len; int *flash_gamma_bank_start; int flash_gamma_bank_end_len; int *flash_gamma_bank_end;
	int flash_gamma_check_sum_start_offset; int flash_gamma_check_sum_end_offset;
	int flash_gamma_0xc8_start_offset; int flash_gamma_0xc8_end_offset; int flash_gamma_0xc8_size; int flash_gamma_0xc8_check_sum_start_offset; int flash_gamma_0xc8_check_sum_end_offset;
	int flash_MCD1_R_address; int flash_MCD2_R_address; int flash_MCD1_L_address; int flash_MCD2_L_address;

	int num_of_data_lanes; bool is_jpn;
};

struct display_status { bool wait_disp_on; int wait_actual_disp_on; int aod_delay; int hbm_mode; int disp_on_pre; };
struct hmt_status {
	unsigned int hmt_on; unsigned int hmt_reverse; unsigned int hmt_is_first;
	int hmt_bl_level; int candela_level_hmt; int cmd_idx_hmt;
	int (*hmt_enable)(struct samsung_display_driver_data *vdd);
	int (*hmt_reverse_update)(struct samsung_display_driver_data *vdd, int enable);
	int (*hmt_bright_update)(struct samsung_display_driver_data *vdd);
};

struct esd_recovery {
	spinlock_t irq_lock; bool esd_recovery_init; bool is_enabled_esd_recovery; bool is_wakeup_source;
	int esd_gpio[MAX_ESD_GPIO]; u8 num_of_gpio; unsigned long irqflags[MAX_ESD_GPIO];
	void (*esd_irq_enable)(bool enable, bool nosync, void *data); int send_esd_recovery;
};

enum { 
	LPM_MODE_OFF = 0, 
	ALPM_MODE_ON, 
	HLPM_MODE_ON, 
	MAX_LPM_MODE 
};

enum { 
	ALPM_MODE_ON_2NIT = 1, 
	HLPM_MODE_ON_2NIT = 2, 
	ALPM_MODE_ON_60NIT = 3, 
	HLPM_MODE_ON_60NIT_DUMMY = 4,
	HLPM_MODE_ON_60NIT = 5 /* Definit explicit ca identificator unic cerut la linia 1953 */
};

struct panel_func {
	int (*samsung_panel_on_pre)(struct samsung_display_driver_data *vdd);
	int (*samsung_panel_on_post)(struct samsung_display_driver_data *vdd);
	int (*samsung_panel_off_pre)(struct samsung_display_driver_data *vdd);
	int (*samsung_panel_off_post)(struct samsung_display_driver_data *vdd);
	void (*samsung_backlight_late_on)(struct samsung_display_driver_data *vdd);
	void (*samsung_panel_init)(struct samsung_display_driver_data *vdd);
	char (*samsung_panel_revision)(struct samsung_display_driver_data *vdd);
	int (*samsung_module_info_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_manufacture_date_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_ddi_id_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_cell_id_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_octa_id_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_hbm_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_elvss_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_irc_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_mdnie_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_smart_dimming_init)(struct samsung_display_driver_data *vdd);
	int (*samsung_flash_gamma_support)(struct samsung_display_driver_data *vdd);
	int (*samsung_interpolation_init)(struct samsung_display_driver_data *vdd, enum INTERPOLATION_MODE mode);
	struct smartdim_conf *(*samsung_smart_get_conf)(void);
	struct dsi_panel_cmd_set * (*samsung_brightness_hbm_off)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_aid)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_acl_on)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_pre_acl_percent)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_acl_percent)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_acl_off)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_pre_elvss)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_elvss)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_pre_caps)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_caps)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_elvss_temperature1)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_elvss_temperature2)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_vint)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_irc)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_gamma)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_hbm_gamma)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_hbm_etc)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_hbm_irc)(struct samsung_display_driver_data *vdd, int *level_key);
	int (*get_hbm_candela_value)(int level);
	void (*ss_event_frame_update)(struct samsung_display_driver_data *vdd, int event, void *arg);
	void (*ss_event_fb_event_callback)(struct samsung_display_driver_data *vdd, int event, void *arg);
	void (*ss_event_osc_te_fitting)(struct samsung_display_driver_data *vdd, int event, void *arg);
	void (*ss_event_esd_recovery_init)(struct samsung_display_driver_data *vdd, int event, void *arg);
	int (*samsung_osc_te_fitting)(struct samsung_display_driver_data *vdd);
	int (*samsung_change_ldi_fps)(struct samsung_display_driver_data *vdd, unsigned int input_fps);
	struct dsi_panel_cmd_set * (*samsung_brightness_gamma_hmt)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_aid_hmt)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_elvss_hmt)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_vint_hmt)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_hmt)(struct samsung_display_driver_data *vdd, int *level_key);
	int (*samsung_smart_dimming_hmt_init)(struct samsung_display_driver_data *vdd);
	struct smartdim_conf *(*samsung_smart_get_conf_hmt)(void);
	void (*samsung_tft_blic_init)(struct samsung_display_driver_data *vdd);
	void (*samsung_brightness_tft_pwm)(struct samsung_display_driver_data *vdd, int level);
	struct dsi_panel_cmd_set * (*samsung_brightness_tft_pwm_ldi)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_gamma_mode2)(struct samsung_display_driver_data *vdd, int *level_key);
	void (*samsung_bl_ic_pwm_en)(int enable);
	void (*samsung_bl_ic_i2c_ctrl)(int scaled_level);
	void (*samsung_bl_ic_outdoor)(int enable);
	void (*samsung_ql_lvds_register_set)(struct samsung_display_driver_data *vdd);
	int (*samsung_lvds_write_reg)(u16 addr, u32 data);
	void (*samsung_update_lpm_ctrl_cmd)(struct samsung_display_driver_data *vdd);
	void (*samsung_set_lpm_brightness)(struct samsung_display_driver_data *vdd);
	int (*parsing_otherline_pdata)(struct file *f, struct samsung_display_driver_data *vdd, char *src, int len);
	void (*set_panel_fab_type)(int type); int (*get_panel_fab_type)(void);
	void (*color_weakness_ccb_on_off)(struct samsung_display_driver_data *vdd, int mode);
	int (*ddi_hw_cursor)(struct samsung_display_driver_data *vdd, int *input);
	void (*samsung_cover_control)(struct samsung_display_driver_data *vdd);
	int (*samsung_poc_ctrl)(struct samsung_display_driver_data *vdd, u32 cmd, const char *buf);
	int (*samsung_gct_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_gct_write)(struct samsung_display_driver_data *vdd);
	void (*samsung_pba_config)(struct samsung_display_driver_data *vdd, void *arg);
	void (*get_aor_data)(struct samsung_display_driver_data *vdd);
	void (*gen_hbm_interpolation_gamma)(struct samsung_display_driver_data *vdd, struct ss_interpolation_brightness_table *normal_table, int normal_table_size);
	void (*gen_hbm_interpolation_irc)(struct samsung_display_driver_data *vdd, struct ss_interpolation_brightness_table *hbml_table, int hbm_table_size);
	void (*gen_normal_interpolation_irc)(struct samsung_display_driver_data *vdd, struct ss_interpolation_brightness_table *normal_table, int normal_table_size);
};

struct samsung_register_info { size_t virtual_addr; };
struct samsung_register_dump_info { struct samsung_register_info dsi_pll; struct samsung_register_info dsi_ctrl; struct samsung_register_info dsi_phy; };
struct samsung_display_debug_data { struct dentry *root; struct dentry *dump; struct dentry *hw_info; struct dentry *display_status; struct dentry *display_ltp; bool print_cmds; bool *is_factory_mode; bool panic_on_pptimeout; struct miscdevice dev; bool report_once; };

struct self_display {
	struct miscdevice dev; int is_support; int factory_support; int on; int file_open; int time_set;
	struct self_time_info st_info; struct self_icon_info si_info; struct self_grid_info sg_info; struct self_analog_clk_info sa_info; struct self_digital_clk_info sd_info; struct self_partial_hlpm_scan sphs_info;
	struct mutex vdd_self_move_lock; struct mutex vdd_self_mask_lock; struct mutex vdd_self_aclock_lock; struct mutex vdd_self_dclock_lock; struct mutex vdd_self_icon_grid_lock;
	struct self_display_op operation[FLAG_SELF_DISP_MAX]; struct self_display_debug debug; u8 *mask_crc_pass_data; u8 *mask_crc_read_data; int mask_crc_size;
	int (*init)(struct samsung_display_driver_data *vdd); int (*data_init)(struct samsung_display_driver_data *vdd); int (*aod_enter)(struct samsung_display_driver_data *vdd); int (*aod_exit)(struct samsung_display_driver_data *vdd);
	void (*self_mask_img_write)(struct samsung_display_driver_data *vdd); void (*self_mask_on)(struct samsung_display_driver_data *vdd, int enable); int (*self_mask_check)(struct samsung_display_driver_data *vdd);
	void (*self_blinking_on)(struct samsung_display_driver_data *vdd, int enable); int (*self_display_debug)(struct samsung_display_driver_data *vdd); void (*self_move_set)(struct samsung_display_driver_data *vdd, int ctrl);
	int (*self_icon_set)(struct samsung_display_driver_data *vdd); int (*self_grid_set)(struct samsung_display_driver_data *vdd); int (*self_aclock_set)(struct samsung_display_driver_data *vdd); int (*self_dclock_set)(struct samsung_display_driver_data *vdd);
	int (*self_time_set)(struct samsung_display_driver_data *vdd, int from_self_move); int (*self_partial_hlpm_scan_set)(struct samsung_display_driver_data *vdd);
};

enum mdss_cpufreq_cluster { CPUFREQ_CLUSTER_BIG, CPUFREQ_CLUSTER_LITTLE, CPUFREQ_CLUSTER_ALL };
enum ss_panel_pwr_state { PANEL_PWR_OFF, PANEL_PWR_ON_READY, PANEL_PWR_ON, PANEL_PWR_LPM, MAX_PANEL_PWR };
enum ss_display_ndx { PRIMARY_DISPLAY_NDX = 0, SECONDARY_DISPLAY_NDX, MAX_DISPLAY_NDX };

enum { POC_OP_NONE = 0, POC_OP_ERASE = 1, POC_OP_WRITE = 2, POC_OP_READ = 3, POC_OP_ERASE_WRITE_IMG = 4, POC_OP_ERASE_WRITE_TEST = 5, POC_OP_BACKUP = 6, POC_OP_ERASE_SECTOR = 7, POC_OP_CHECKSUM, POC_OP_CHECK_FLASH, POC_OP_SET_FLASH_WRITE, POC_OP_SET_FLASH_EMPTY, MAX_POC_OP };
enum poc_state { POC_STATE_NONE, POC_STATE_FLASH_EMPTY, POC_STATE_FLASH_FILLED, POC_STATE_ER_START, POC_STATE_ER_PROGRESS, POC_STATE_ER_COMPLETE, POC_STATE_ER_FAILED, POC_STATE_WR_START, POC_STATE_WR_PROGRESS, POC_STATE_WR_COMPLETE, POC_STATE_WR_FAILED, MAX_POC_STATE };

#define IOC_GET_POC_STATUS	_IOR('A', 100, __u32)
#define IOC_GET_POC_CHKSUM	_IOR('A', 101, __u32)
#define IOC_GET_POC_CSDATA	_IOR('A', 102, __u32)
#define IOC_GET_POC_ERASED	_IOR('A', 103, __u32)
#define IOC_GET_POC_FLASHED	_IOR('A', 104, __u32)
#define IOC_SET_POC_ERASE	_IOR('A', 110, __u32)
#define IOC_SET_POC_TEST	_IOR('A', 112, __u32)

struct POC {
	bool is_support; int poc_operation; u32 file_opend; struct miscdevice dev; bool erased; atomic_t cancel; struct notifier_block dpui_notif;
	u8 chksum_data[4]; u8 chksum_res; u8 *wbuf; u32 wpos; u32 wsize; u8 *rbuf; u32 rpos; u32 rsize;
	int start_addr; int image_size; int er_try_cnt; int er_fail_cnt; u32 erase_delay_us; int erase_sector_addr_idx[3];
	int wr_try_cnt; int wr_fail_cnt; u32 write_delay_us; int write_loop_cnt; int write_data_size; int write_addr_idx[3];
	int rd_try_cnt; int rd_fail_cnt; u32 read_delay_us; int read_addr_idx[3]; u8 *mca_data; int mca_size;
	int (*poc_write)(struct samsung_display_driver_data *vdd, u8 *data, u32 pos, u32 size); int (*poc_read)(struct samsung_display_driver_data *vdd, u8 *buf, u32 pos, u32 size); int (*poc_erase)(struct samsung_display_driver_data *vdd, u32 erase_pos, u32 erase_size, u32 target_pos);
	int (*poc_open)(struct samsung_display_driver_data *vdd); int (*poc_release)(struct samsung_display_driver_data *vdd); void (*poc_comp)(struct samsung_display_driver_data *vdd); int (*check_read_case)(struct samsung_display_driver_data *vdd);
	int read_case; bool need_sleep_in;
};

#define GCT_RES_CHECKSUM_PASS	(1)
#define GCT_RES_CHECKSUM_NG	(0)
#define GCT_RES_CHECKSUM_OFF	(-2)
#define GCT_RES_CHECKSUM_NOT_SUPPORT	(-3)

struct gram_checksum_test { bool is_support; int on; u8 checksum[4]; };
struct ss_exclusive_mipi_tx { struct mutex ex_tx_lock; int enable; wait_queue_head_t ex_tx_waitq; int permit_frame_update; };

struct mdnie_info {
	int support_mdnie; int support_trans_dimming; int disable_trans_dimming; int enter_hbm_ce_lux; bool tuning_enable_tft; int lcd_on_notifiy; int mdnie_x; int mdnie_y; int mdnie_tune_size[MDNIE_TUNE_MAX_SIZE];
	struct mdnie_lite_tun_type *mdnie_tune_state_dsi; struct mdnie_lite_tune_data *mdnie_data;
};

struct brightness_info {
	int pac; int elvss_interpolation_temperature; int auto_level; int bl_level; int cd_level; int interpolation_cd; int gamma_mode2_cd; int gamma_mode2_support; int multi_to_one_support;
	int finger_mask_bl_level; int finger_mask_hbm_on; int cd_idx; int pac_cd_idx; int elvss_value1; int elvss_value2; u8 *irc_otp; int aor_data;
	enum IRC_MODE irc_mode; int support_irc; struct workqueue_struct *br_wq; struct work_struct br_work;
};

struct STM_CMD { int STM_CTRL_EN; int STM_MAX_OPT; int	STM_DEFAULT_OPT; int STM_DIM_STEP; int STM_FRAME_PERIOD; int STM_MIN_SECT; int STM_PIXEL_PERIOD; int STM_LINE_PERIOD; int STM_MIN_MOVE; int STM_M_THRES; int STM_V_THRES; };
struct STM_REG_OSSET{ const char *name; int offset; };
struct STM { int stm_on; struct STM_CMD orig_cmd; struct STM_CMD cur_cmd; };
struct ub_con_detect { spinlock_t irq_lock; int gpio; unsigned long irqflag; bool enabled; int ub_con_cnt; };

struct samsung_display_driver_data {
	void *msm_private; int cmd_type; bool panel_dead; int read_panel_status_from_lk; bool is_factory_mode; bool panel_attach_status; int panel_revision; char *panel_vendor;
	bool support_optical_fingerprint; bool finger_mask_updated; int finger_mask; int panel_hbm_entry_delay; struct lcd_device *lcd_dev; bool remove_self_move;
	struct display_status display_status_dsi; struct mutex vdd_lock; struct mutex cmd_lock; struct mutex bl_lock; struct mutex ss_spi_lock;
	struct samsung_display_debug_data *debug_data; struct ss_exclusive_mipi_tx exclusive_tx; struct list_head vdd_list; int temperature; int lux; int acl_status; int siop_status;
	struct panel_func panel_func; struct samsung_display_dtsi_data dtsi_data;
	int manufacture_id_dsi; int module_info_loaded_dsi; int manufacture_date_loaded_dsi; int manufacture_date_dsi; int manufacture_time_dsi; int ddi_id_loaded_dsi; int ddi_id_dsi[5];
	int cell_id_loaded_dsi; int cell_id_dsi[MAX_CELL_ID]; int octa_id_loaded_dsi; u8 octa_id_dsi[MAX_OCTA_ID]; int hbm_loaded_dsi; int elvss_loaded_dsi; int irc_loaded_dsi;
	int gradual_acl_val; int cover_control; int select_panel_gpio; bool select_panel_use_expander_gpio; int xtalk_mode; s64 reset_time_64; s64 sleep_out_time_64; int gpara;
	enum ss_panel_pwr_state panel_state; enum ss_display_ndx ndx; struct samsung_register_dump_info dump_info[MAX_INTF_NUM];
	int smart_dimming_loaded_dsi; struct smartdim_conf *smart_dimming_dsi; struct osc_te_fitting_info te_fitting_info;
	int smart_dimming_hmt_loaded_dsi; struct hmt_status hmt_stat; struct smartdim_conf *smart_dimming_dsi_hmt;
	int support_cabc; int scaled_level; int (*backlight_tft_config)(struct samsung_display_driver_data *vdd, int enable); void (*backlight_tft_pwm_control)(struct samsung_display_driver_data *vdd, int bl_lvl); void (*ss_panel_tft_outdoormode_update)(struct samsung_display_driver_data *vdd);
	struct esd_recovery esd_recovery; int panel_recovery_cnt; struct workqueue_struct *image_dump_workqueue; struct work_struct image_dump_work; struct workqueue_struct *other_line_panel_support_workq; struct delayed_work other_line_panel_support_work; int other_line_panel_work_cnt;
	struct lpm_info panel_lpm; struct notifier_block dpui_notif; struct notifier_block dpci_notif; u64 dsi_errors; struct COPR copr; int copr_load_init_cmd;
	bool samsung_support_ddi_spi; struct spi_device *spi_dev; struct spi_driver spi_driver; struct notifier_block spi_notif; int ddi_spi_status; struct ddi_spi_cmd_set *spi_cmd_set; int ddi_spi_cs_high_gpio_for_gpara;
	struct POC poc_driver; struct dyn_mipi_clk dyn_mipi_clk; int ffc_cmds_line_position; struct gram_checksum_test gct;
	bool support_hall_ic; int hall_ic_status; int hall_ic_mode_change_trigger; bool hall_ic_status_pending; bool hall_ic_status_unhandled; struct notifier_block hall_ic_notifier_display; bool lcd_flip_not_refresh; u32 lcd_flip_delay_ms; struct delayed_work delay_disp_on_work;
	struct ss_smmu_debug ss_debug_smmu[SMMU_MAX_DEBUG]; struct kmem_cache *ss_debug_smmu_cache; struct self_display self_disp; int self_display_loaded_dsi;
	struct workqueue_struct *flash_br_workqueue; struct delayed_work flash_br_work; struct brightness_data_info panel_br_info; struct ss_interpolation flash_itp; struct ss_interpolation table_itp; int table_interpolation_loaded; bool spi_no_dev;
	struct brightness_info br; struct mdnie_info mdnie; int mdnie_loaded_dsi; struct STM stm; int stm_load_init_cmd; int grayspot; int ccd_pass_val; int ccd_fail_val; int samsung_splash_enabled; int pll_ssc_disabled; struct ub_con_detect ub_con_det; bool support_partial_disp; int partial_disp_val; bool samsung_enable_splash_pba; char window_color[2]; bool support_window_color; int force_white_flush;
};

extern struct list_head vdds_list;

/* COMMON FUNCTIONS */
void ss_panel_init(struct dsi_panel *panel);
void ss_set_max_cpufreq(struct samsung_display_driver_data *vdd, int enable, enum mdss_cpufreq_cluster cluster);
void ss_set_exclusive_tx_packet(struct samsung_display_driver_data *vdd, enum dsi_cmd_set_type cmd, int pass);
void ss_set_exclusive_tx_lock_from_qct(struct samsung_display_driver_data *vdd, bool lock);
int ss_send_cmd(struct samsung_display_driver_data *vdd, enum dsi_cmd_set_type cmd);
int ss_write_ddi_ram(struct samsung_display_driver_data *vdd, int target, u8 *buffer, int len);
int ss_panel_on_pre(struct samsung_display_driver_data *vdd);
int ss_panel_on_post(struct samsung_display_driver_data *vdd);
int ss_panel_off_pre(struct samsung_display_driver_data *vdd);
int ss_panel_off_post(struct samsung_display_driver_data *vdd);
int ss_backlight_tft_gpio_config(struct samsung_display_driver_data *vdd, int enable);
int ss_backlight_tft_request_gpios(struct samsung_display_driver_data *vdd);
int ss_panel_data_read(struct samsung_display_driver_data *vdd, enum dsi_cmd_set_type type, u8 *buffer, int level_key);
void ss_panel_low_power_config(struct samsung_display_driver_data *vdd, int enable);
int ss_panel_attached(int ndx);
int get_lcd_attached(char *mode);
int get_lcd_attached_secondary(char *mode);
struct samsung_display_driver_data *check_valid_ctrl(struct dsi_panel *panel);
char ss_panel_id0_get(struct samsung_display_driver_data *vdd);
char ss_panel_id1_get(struct samsung_display_driver_data *vdd);
char ss_panel_id2_get(struct samsung_display_driver_data *vdd);
char ss_panel_rev_get(struct samsung_display_driver_data *vdd);
int ss_panel_attach_get(struct samsung_display_driver_data *vdd);
int ss_panel_attach_set(struct samsung_display_driver_data *vdd, bool attach);
int ss_read_loading_detection(void);

#ifdef CONFIG_FOLDER_HALL
extern void hall_ic_register_notify(struct notifier_block *nb);
#endif

extern struct dsi_status_data *pstatus_data;
int hmt_enable(struct samsung_display_driver_data *vdd);
int hmt_reverse_update(struct samsung_display_driver_data *vdd, int enable);
int samsung_display_hall_ic_status(struct notifier_block *nb, unsigned long hall_ic, void *data);
void ss_copr_calc_work(struct work_struct *work);
void ss_copr_calc_delayed_work(struct delayed_work *work);
int ss_find_reg_offset(int (*reg_list)[2], struct dsi_panel_cmd_set *cmd_list[], int list_size);
void ss_panel_lpm_ctrl(struct samsung_display_driver_data *vdd, int enable);
int ss_panel_lpm_power_ctrl(struct samsung_display_driver_data *vdd, int enable);
int ss_change_dyn_mipi_clk_timing(struct samsung_display_driver_data *vdd);
int ss_dyn_mipi_clk_tx_ffc(struct samsung_display_driver_data *vdd);
void ss_read_mtp(struct samsung_display_driver_data *vdd, int addr, int len, int pos, u8 *buf);
void ss_write_mtp(struct samsung_display_driver_data *vdd, int len, u8 *buf);
#define MAX_READ_LINE_SIZE 256
int read_line(char *src, char *buf, int *pos, int len);
int ss_stm_set_cmd_offset(struct STM_CMD *cmd, char* p);
void print_stm_cmd(struct STM_CMD cmd);
int ss_get_stm_orig_cmd(struct samsung_display_driver_data *vdd);
void ss_stm_set_cmd(struct samsung_display_driver_data *vdd, struct STM_CMD *cmd);
void ss_send_ub_uevent(struct samsung_display_driver_data *vdd);

#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_BRIGHTNESS_PAC3 25500
#define BRIGHTNESS_MAX_PACKET 50
#define HBM_MODE 6

int ss_brightness_dcs(struct samsung_display_driver_data *vdd, int level, int backlight_origin);
void ss_brightness_tft_pwm(struct samsung_display_driver_data *vdd, int level);
void update_packet_level_key_enable(struct samsung_display_driver_data *vdd, struct dsi_cmd_desc *packet, int *cmd_cnt, int level_key);
void update_packet_level_key_disable(struct samsung_display_driver_data *vdd, struct dsi_cmd_desc *packet, int *cmd_cnt, int level_key);
int ss_single_transmission_packet(struct dsi_panel_cmd_set *cmds);
int ss_set_backlight(struct samsung_display_driver_data *vdd, u32 bl_lvl);
bool is_hbm_level(struct samsung_display_driver_data *vdd);
void ss_send_hbm_fingermask_image_tx(struct samsung_display_driver_data *vdd, bool on);
int ss_brightness_dcs_hmt(struct samsung_display_driver_data *vdd, int level);
int hmt_bright_update(struct samsung_display_driver_data *vdd);
int get_scaled_level(struct samsung_display_driver_data *vdd, int ndx);
void ss_tft_autobrightness_cabc_update(struct samsung_display_driver_data *vdd);

#include "ss_dsi_smart_dimming_common.h"
#include "ss_dsi_mdnie_lite_common.h"

extern struct dsi_display_boot_param boot_displays[MAX_DSI_ACTIVE_DISPLAY];
extern char dsi_display_primary[MAX_CMDLINE_PARAM_LEN];
extern char dsi_display_secondary[MAX_CMDLINE_PARAM_LEN];

static inline void ss_get_primary_panel_name_cmdline(char *panel_name) {
	char *pos = NULL; size_t len;
	pos = strnstr(dsi_display_primary, ":", sizeof(dsi_display_primary));
	if (!pos) return;
	len = (size_t) (pos - dsi_display_primary) + 1;
	strlcpy(panel_name, dsi_display_primary, len);
}

static inline void ss_get_secondary_panel_name_cmdline(char *panel_name) {
	char *pos = NULL; size_t len;
	pos = strnstr(dsi_display_secondary, ":", sizeof(dsi_display_secondary));
	if (!pos) return;
	len = (size_t) (pos - dsi_display_secondary) + 1;
	strlcpy(panel_name, dsi_display_secondary, len);
}

struct samsung_display_driver_data *ss_get_vdd(enum ss_display_ndx ndx);
#define GET_DSI_PANEL(vdd)	((struct dsi_panel *) (vdd)->msm_private)

static inline struct dsi_display *GET_DSI_DISPLAY(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	return dev_get_drvdata(panel->parent);
}

static inline struct drm_device *GET_DRM_DEV(struct samsung_display_driver_data *vdd) {
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	return display->drm_dev;
}

static inline struct msm_kms *GET_MSM_KMS(struct samsung_display_driver_data *vdd) {
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct msm_drm_private *priv = ddev->dev_private;
	return priv->kms;
}
#define GET_SDE_KMS(vdd)	to_sde_kms(GET_MSM_KMS(vdd))

static inline struct drm_crtc *GET_DRM_CRTC(struct samsung_display_driver_data *vdd) {
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct msm_drm_private *priv = ddev->dev_private;
	return priv->crtcs[0];
}

static inline struct drm_encoder *GET_DRM_ENCODER(struct samsung_display_driver_data *vdd) {
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct msm_drm_private *priv = ddev->dev_private;
	return priv->encoders[0];
}

static inline struct drm_connector *GET_DRM_CONNECTORS(struct samsung_display_driver_data *vdd) {
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct list_head *connector_list; struct drm_connector *conn = NULL, *conn_iter; struct sde_connector *c_conn;
	connector_list = &ddev->mode_config.connector_list;
	list_for_each_entry(conn_iter, connector_list, head) {
		c_conn = to_sde_connector(conn_iter);
		if (c_conn->display == display) { conn = conn_iter; break; }
	}
	if (!conn) pr_err("[SDE] failed to find drm_connector\n");
	return conn;
}

#define GET_SDE_CONNECTOR(vdd)	to_sde_connector(GET_DRM_CONNECTORS(vdd))

static inline struct backlight_device *GET_SDE_BACKLIGHT_DEVICE(struct samsung_display_driver_data *vdd) {
	struct backlight_device *bd = NULL; struct sde_connector *conn = NULL;
	if (IS_ERR_OR_NULL(vdd)) goto end;
	conn = GET_SDE_CONNECTOR(vdd);
	if (IS_ERR_OR_NULL(conn)) goto end;
	bd = conn->bl_device;
end:
	return bd;
}

static inline enum ss_display_ndx ss_get_display_ndx(struct samsung_display_driver_data *vdd) { return vdd->ndx; }
static inline void ss_set_display_ndx(struct samsung_display_driver_data *vdd, int ndx) { vdd->ndx = ndx; }
extern struct samsung_display_driver_data vdd_data[MAX_DISPLAY_NDX];

static inline struct samsung_display_driver_data *ss_check_hall_ic_get_vdd(struct samsung_display_driver_data *vdd) {
	if (!vdd->support_hall_ic) return vdd;
	return ss_get_vdd(PRIMARY_DISPLAY_NDX);
}

static inline const char *ss_get_panel_name(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	return panel->name;
}

static inline u32 ss_get_xres(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	if (!panel->cur_mode) return 0;
	return panel->cur_mode->timing.h_active;
}

static inline u32 ss_get_yres(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	if (!panel->cur_mode) return 0;
	return panel->cur_mode->timing.v_active;
}

static inline bool ss_is_dual_dsi(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	if (!panel->cur_mode || !panel->cur_mode->priv_info) return false;
	return (panel->cur_mode->priv_info->topology.num_intf == 2);
}

static inline bool ss_is_single_dsi(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	if (!panel->cur_mode) return true;
	return (panel->cur_mode->priv_info->topology.num_intf == 1);
}

static inline bool ss_is_cmd_mode(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	return (panel->panel_mode == DSI_OP_CMD_MODE);
}

static inline bool ss_is_video_mode(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	return (panel->panel_mode == DSI_OP_VIDEO_MODE);
}

static inline bool ss_is_bl_dcs(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	return (panel->bl_config.type == DSI_BACKLIGHT_DCS);
}

static inline bool ss_is_panel_on(struct samsung_display_driver_data *vdd) { return (vdd->panel_state == PANEL_PWR_ON); }
static inline bool ss_is_panel_on_ready(struct samsung_display_driver_data *vdd) { return (vdd->panel_state == PANEL_PWR_ON_READY); }
static inline bool ss_is_ready_to_send_cmd(struct samsung_display_driver_data *vdd) { return ((vdd->panel_state == PANEL_PWR_ON) || (vdd->panel_state == PANEL_PWR_LPM)); }
static inline bool ss_is_panel_off(struct samsung_display_driver_data *vdd) { return (vdd->panel_state == PANEL_PWR_OFF); }
static inline bool ss_is_panel_lpm(struct samsung_display_driver_data *vdd) { return (vdd->panel_state == PANEL_PWR_LPM); }

static inline int ss_is_read_cmd(enum dsi_cmd_set_type type) {
	return ((type > RX_CMD_START && type < RX_CMD_END) || (type == RX_SELF_DISP_DEBUG || type == RX_SELF_MASK_CHECK));
}

static inline bool ss_is_seamless_mode(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	if (!panel->cur_mode) return false;
	return (panel->cur_mode->dsi_mode_flags & DSI_MODE_FLAG_SEAMLESS);
}

static inline void ss_set_seamless_mode(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	if (!panel->cur_mode) return;
	panel->cur_mode->dsi_mode_flags |= DSI_MODE_FLAG_SEAMLESS;
}

static inline unsigned int ss_get_te_gpio(struct samsung_display_driver_data *vdd) {
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	return display->disp_te_gpio;
}

static inline bool ss_is_vsync_enabled(struct samsung_display_driver_data *vdd) {
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *dev = display->drm_dev;
	int pipe = drm_crtc_index(GET_DRM_CRTC(vdd));
	return dev->vblank[pipe].enabled;
}

static inline unsigned int ss_is_esd_check_enabled(struct samsung_display_driver_data *vdd) { return true; }

extern char *cmd_set_prop_map[SS_DSI_CMD_SET_MAX];
static inline char *ss_get_cmd_name(enum dsi_cmd_set_type type) { return cmd_set_prop_map[type]; }
static inline bool __must_check SS_IS_CMDS_NULL(struct dsi_panel_cmd_set *set) { return unlikely(!set) || unlikely(!set->cmds); }

static inline struct dsi_panel_cmd_set *ss_get_cmds(
		struct samsung_display_driver_data *vdd,
		enum dsi_cmd_set_type type)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	struct dsi_display_mode_priv_info *priv_info;
	struct dsi_panel_cmd_set *set;
	int rev = vdd->panel_revision;

	vdd->cmd_type = type;

	if (type < SS_DSI_CMD_SET_START) {
		if (!panel->cur_mode || !panel->cur_mode->priv_info) return NULL;
		priv_info = panel->cur_mode->priv_info;
		return &priv_info->cmd_sets[type];
	}

	if (type == TX_AID_SUBDIVISION && vdd->br.pac) type = TX_PAC_AID_SUBDIVISION;
	if (type == TX_IRC_SUBDIVISION && vdd->br.pac) type = TX_PAC_IRC_SUBDIVISION;

	/* REPARARE: Facem safe-cast structurii noastre extinse pentru a asigura validitatea codului */
	struct ss_dsi_panel_cmd_set_extended *ext_set = (struct ss_dsi_panel_cmd_set_extended *)&vdd->dtsi_data.cmd_sets[type];

	if (rev >= 0 && rev < SUPPORT_PANEL_REVISION && ext_set->cmd_set_rev[rev])
		return (struct dsi_panel_cmd_set *)ext_set->cmd_set_rev[rev];

	return (struct dsi_panel_cmd_set *)ext_set;
}

static inline struct device_node *ss_get_panel_of(struct samsung_display_driver_data *vdd) {
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	return panel->panel_of_node;
}

#endif /* SS_DSI_PANEL_COMMON_H */
