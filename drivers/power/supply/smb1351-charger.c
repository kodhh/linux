// SPDX-License-Identifier: GPL-2.0-only
/*
 * Driver for SMB1351/SMB1350 parallel charger
 *
 * Copyright (c) 2015-2017 The Linux Foundation. All rights reserved.
 * Copyright (c) 2025 Yureka 2 downstream
 *
 * Based on smb1351-charger.c from Qualcomm msm-3.18 kernel
 */

#define pr_fmt(fmt) "SMB1351 %s: " fmt, __func__

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/power_supply.h>
#include <linux/slab.h>

#define _SMB1351_MASK(BITS, POS) \
	((unsigned char)(((1 << (BITS)) - 1) << (POS)))
#define SMB1351_MASK(LEFT_BIT_POS, RIGHT_BIT_POS) \
		_SMB1351_MASK((LEFT_BIT_POS) - (RIGHT_BIT_POS) + 1, \
				(RIGHT_BIT_POS))

#define CHG_CURRENT_CTRL_REG			0x0
#define FAST_CHG_CURRENT_MASK			SMB1351_MASK(7, 4)
#define AC_INPUT_CURRENT_LIMIT_MASK		SMB1351_MASK(3, 0)

#define CHG_OTH_CURRENT_CTRL_REG		0x1
#define PRECHG_CURRENT_MASK			SMB1351_MASK(7, 5)
#define ITERM_MASK				SMB1351_MASK(4, 2)
#define USB_2_3_MODE_SEL_BIT			BIT(1)
#define USB_2_3_MODE_SEL_BY_I2C			0
#define USB_2_3_MODE_SEL_BY_PIN			0x2
#define USB_5_1_CMD_POLARITY_BIT		BIT(0)
#define USB_CMD_POLARITY_500_1_100_0		0
#define USB_CMD_POLARITY_500_0_100_1		0x1

#define VARIOUS_FUNC_REG			0x2
#define SUSPEND_MODE_CTRL_BIT			BIT(7)
#define SUSPEND_MODE_CTRL_BY_PIN		0
#define SUSPEND_MODE_CTRL_BY_I2C		0x80
#define AICL_EN_BIT				BIT(4)
#define APSD_EN_BIT				BIT(2)

#define VFLOAT_REG				0x3
#define VFLOAT_MASK				SMB1351_MASK(5, 0)

#define CHG_CTRL_REG				0x4
#define AUTO_RECHG_BIT				BIT(7)
#define AUTO_RECHG_ENABLE			0
#define AUTO_RECHG_DISABLE			0x80
#define ITERM_EN_BIT				BIT(6)
#define ITERM_ENABLE				0
#define ITERM_DISABLE				0x40
#define AUTO_RECHG_TH_BIT			BIT(3)
#define AUTO_RECHG_TH_50MV			0
#define AUTO_RECHG_TH_100MV			0x8

#define CHG_PIN_EN_CTRL_REG			0x6
#define EN_PIN_CTRL_MASK			SMB1351_MASK(6, 5)
#define EN_BY_I2C_0_DISABLE			0
#define EN_BY_I2C_0_ENABLE			0x20
#define EN_BY_PIN_HIGH_ENABLE			0x40
#define EN_BY_PIN_LOW_ENABLE			0x60
#define USBCS_CTRL_BIT				BIT(4)
#define USBCS_CTRL_BY_I2C			0
#define USBCS_CTRL_BY_PIN			0x10
#define CHG_ERR_BIT				BIT(2)
#define APSD_DONE_BIT				BIT(1)
#define LED_BLINK_FUNC_BIT			BIT(7)

#define THERM_A_CTRL_REG			0x7
#define THERM_MONITOR_BIT			BIT(4)
#define THERM_MONITOR_EN			0

#define OTG_TLIM_CTRL_REG			0xA
#define SWITCH_FREQ_MASK			SMB1351_MASK(7, 6)
#define SWITCH_FREQ_SHIFT			6

#define VERSION_REG				0x2E
#define VERSION_MASK				BIT(1)

#define CMD_I2C_REG				0x30
#define CMD_BQ_CFG_ACCESS_BIT			BIT(6)

#define CMD_INPUT_LIMIT_REG			0x31
#define CMD_SUSPEND_MODE_BIT			BIT(6)
#define CMD_INPUT_CURRENT_MODE_BIT		BIT(3)
#define CMD_INPUT_CURRENT_MODE_CMD		0x08
#define CMD_USB_2_3_SEL_BIT			BIT(2)
#define CMD_USB_2_MODE				0
#define CMD_USB_3_MODE				0x4
#define CMD_USB_1_5_AC_CTRL_MASK		SMB1351_MASK(1, 0)
#define CMD_USB_100_MODE			0
#define CMD_USB_500_MODE			0x2
#define CMD_USB_HC_MODE				0x1

#define CMD_CHG_REG				0x32


#define CMD_HVDCP_REG				0x34
#define CMD_HVDCP_MODE_MASK			SMB1351_MASK(5, 0)

#define STATUS_0_REG				0x36
#define STATUS_INPUT_MODE_MASK			SMB1351_MASK(6, 5)
#define STATUS_INPUT_SUSPEND			BIT(4)
#define STATUS_INPUT_CURRENT_LIMIT_MASK		SMB1351_MASK(3, 0)

#define STATUS_1_REG				0x37
#define STATUS_INPUT_RANGE_MASK			SMB1351_MASK(7, 4)
#define STATUS_INPUT_RANGE_5V			BIT(4)
#define STATUS_INPUT_RANGE_5V_9V		BIT(6)
#define STATUS_INPUT_RANGE_9V			BIT(5)
#define STATUS_INPUT_RANGE_12V			BIT(7)

#define CHG_REVISION_REG			0x3F

#define HVDCP_BATT_MISSING_CTRL_REG		0x12
#define HVDCP_EN_BIT				BIT(5)
#define BATT_MISSING_THERM_PIN_SOURCE_BIT	BIT(0)

#define PON_OPTIONS_REG				0x13
#define SYSOK_INOK_POLARITY_BIT			BIT(7)
#define SYSOK_INOK_POLARITY_INVERT		BIT(7)
#define SYSOK_OPTIONS_MASK			SMB1351_MASK(6, 4)
#define SYSOK_INOK_OPTION1			0x00

#define CHG_STAT_TIMERS_CTRL_REG		0x5
#define STAT_OUTPUT_POLARITY_BIT		BIT(7)
#define STAT_OUTPUT_MODE_BIT			BIT(6)
#define STAT_OUTPUT_CTRL_BIT			BIT(5)

#define VARIOUS_FUNC_2_REG			0xE
#define PRECHG_TO_FASTCHG_BIT			BIT(1)

#define USB2_MIN_CURRENT_MA			100
#define USB2_MAX_CURRENT_MA			500
#define USB3_MIN_CURRENT_MA			150
#define USB3_MAX_CURRENT_MA			900
#define SMB1351_CHG_PRE_MIN_MA			100
#define SMB1351_CHG_FAST_MIN_MA			1000
#define SMB1351_CHG_FAST_MAX_MA			4500
#define SMB1351_CHG_PRE_SHIFT			5
#define SMB1351_CHG_FAST_SHIFT			4
#define SUSPEND_CURRENT_MA			2
#define MIN_FLOAT_MV				3500
#define MAX_FLOAT_MV				4500
#define VFLOAT_STEP_MV				20

#define CHG_ITERM_200MA				0x0
#define CHG_ITERM_300MA				0x04
#define CHG_ITERM_400MA				0x08
#define CHG_ITERM_500MA				0x0C
#define CHG_ITERM_600MA				0x10
#define CHG_ITERM_700MA				0x14

enum chip_version {
	SMB_UNKNOWN = 0,
	SMB1350,
	SMB1351,
	SMB_MAX_TYPE,
};

struct smb1351_charger {
	struct i2c_client	*client;
	struct device		*dev;

	int			vfloat_mv;
	int			recharge_mv;
	int			parallel_pin_polarity_setting;
	int			switch_freq;
	bool			charging_disabled;
	bool			disable_apsd;

	int			parallel_charger_present;
	int			usb_suspended_status;
	int			battchg_disabled_status;
	int			usb_psy_ma;
	int			target_fastchg_current_max_ma;
	int			fastchg_current_max_ma;
	bool			chg_autonomous_mode;

	enum chip_version	version;

	struct power_supply	*parallel_psy;

	struct mutex		config_lock;
	struct mutex		fcc_lock;
};

static int usb_chg_current[] = {
	500, 685, 1000, 1100, 1200, 1300, 1500, 1600,
	1700, 1800, 2000, 2200, 2500, 3000,
};

static int fast_chg_current[] = {
	1000, 1200, 1400, 1600, 1800, 2000, 2200,
	2400, 2600, 2800, 3000, 3400, 3600, 3800,
	4000, 4640,
};

static int pre_chg_current[] = {
	200, 300, 400, 500, 600, 700,
};

static int smb1351_read_reg(struct smb1351_charger *chip, int reg, u8 *val)
{
	s32 ret;

	ret = i2c_smbus_read_byte_data(chip->client, reg);
	if (ret < 0) {
		dev_err(chip->dev, "i2c read fail: can't read from %02x: %d\n",
			reg, ret);
		return ret;
	}
	*val = ret;
	return 0;
}

static int smb1351_write_reg(struct smb1351_charger *chip, int reg, u8 val)
{
	s32 ret;

	ret = i2c_smbus_write_byte_data(chip->client, reg, val);
	if (ret < 0) {
		dev_err(chip->dev, "i2c write fail: can't write %02x to %02x: %d\n",
			val, reg, ret);
		return ret;
	}
	return 0;
}

static int smb1351_masked_write(struct smb1351_charger *chip, int reg,
				u8 mask, u8 val)
{
	u8 temp;
	int rc;

	rc = smb1351_read_reg(chip, reg, &temp);
	if (rc)
		return rc;

	temp &= ~mask;
	temp |= val & mask;

	return smb1351_write_reg(chip, reg, temp);
}

static int smb1351_enable_volatile_writes(struct smb1351_charger *chip)
{
	return smb1351_masked_write(chip, CMD_I2C_REG,
				    CMD_BQ_CFG_ACCESS_BIT,
				    CMD_BQ_CFG_ACCESS_BIT);
}

static int smb_chip_get_version(struct smb1351_charger *chip)
{
	u8 ver;
	int rc;

	if (chip->version != SMB_UNKNOWN)
		return 0;

	rc = smb1351_read_reg(chip, VERSION_REG, &ver);
	if (rc)
		return rc;

	if (ver & VERSION_MASK)
		chip->version = SMB1350;
	else
		chip->version = SMB1351;

	return 0;
}

static int smb1351_usb_suspend(struct smb1351_charger *chip, int reason,
			       bool suspend)
{
	int suspended;

	suspended = chip->usb_suspended_status;

	if (!suspend)
		suspended &= ~reason;
	else
		suspended |= reason;

	smb1351_masked_write(chip, CMD_INPUT_LIMIT_REG,
			     CMD_SUSPEND_MODE_BIT,
			     suspended ? CMD_SUSPEND_MODE_BIT : 0);

	chip->usb_suspended_status = suspended;

	return 0;
}

static int smb1351_get_usb_chg_current(struct smb1351_charger *chip,
				       int *icl_ma)
{
	u8 icl_status, usb_mode;
	bool is_usb3;
	int rc, i;

	rc = smb1351_read_reg(chip, STATUS_0_REG, &icl_status);
	if (rc)
		return rc;

	if (icl_status & STATUS_INPUT_SUSPEND) {
		*icl_ma = 0;
		return 0;
	}

	rc = smb1351_read_reg(chip, CMD_INPUT_LIMIT_REG, &usb_mode);
	if (rc)
		return rc;

	is_usb3 = !!(usb_mode & CMD_USB_2_3_SEL_BIT);

	switch (icl_status & STATUS_INPUT_MODE_MASK) {
	case 0x20:
		if (is_usb3)
			*icl_ma = USB3_MIN_CURRENT_MA;
		else
			*icl_ma = USB2_MIN_CURRENT_MA;
		break;
	case 0x40:
		if (is_usb3)
			*icl_ma = USB3_MAX_CURRENT_MA;
		else
			*icl_ma = USB2_MAX_CURRENT_MA;
		break;
	case 0x00:
		i = icl_status & STATUS_INPUT_CURRENT_LIMIT_MASK;
		if (i >= ARRAY_SIZE(usb_chg_current))
			i = ARRAY_SIZE(usb_chg_current) - 1;
		*icl_ma = usb_chg_current[i];
		break;
	default:
		break;
	}

	return 0;
}

static int smb1351_set_usb_chg_current(struct smb1351_charger *chip,
				       int current_ma)
{
	int i, rc = 0, icl_result_ma = 0;
	u8 reg = 0, mask;

	if (chip->chg_autonomous_mode)
		return 0;

	if (current_ma <= SUSPEND_CURRENT_MA) {
		smb1351_usb_suspend(chip, 4, true);
		return 0;
	}

	if (current_ma > SUSPEND_CURRENT_MA &&
	    current_ma < USB2_MIN_CURRENT_MA)
		current_ma = USB2_MIN_CURRENT_MA;

	if (current_ma == USB2_MIN_CURRENT_MA)
		reg = CMD_USB_2_MODE | CMD_USB_100_MODE;
	else if (current_ma == USB3_MIN_CURRENT_MA)
		reg = CMD_USB_3_MODE | CMD_USB_100_MODE;
	else if (current_ma == USB2_MAX_CURRENT_MA)
		reg = CMD_USB_2_MODE | CMD_USB_500_MODE;
	else if (current_ma == USB3_MAX_CURRENT_MA)
		reg = CMD_USB_3_MODE | CMD_USB_500_MODE;
	else if (current_ma > USB2_MAX_CURRENT_MA) {
		reg = CMD_USB_HC_MODE;

		smb1351_get_usb_chg_current(chip, &icl_result_ma);

		for (i = ARRAY_SIZE(usb_chg_current) - 1; i >= 0; i--) {
			if (usb_chg_current[i] <= current_ma)
				break;
		}
		if (i < 0)
			i = 0;

		rc = smb1351_masked_write(chip, CHG_CURRENT_CTRL_REG,
					  AC_INPUT_CURRENT_LIMIT_MASK, i);
		if (rc)
			return rc;

		current_ma = usb_chg_current[i];
	}

	reg |= CMD_INPUT_CURRENT_MODE_CMD;
	mask = CMD_INPUT_CURRENT_MODE_BIT | CMD_USB_2_3_SEL_BIT |
	       CMD_USB_1_5_AC_CTRL_MASK;

	rc = smb1351_masked_write(chip, CMD_INPUT_LIMIT_REG, mask, reg);
	if (rc)
		return rc;

	smb1351_usb_suspend(chip, 4, false);

	if (icl_result_ma < current_ma && (reg & CMD_USB_HC_MODE)) {
		smb1351_masked_write(chip, CMD_INPUT_LIMIT_REG, mask,
				     CMD_USB_2_MODE | CMD_USB_500_MODE |
				     CMD_INPUT_CURRENT_MODE_CMD);
		smb1351_masked_write(chip, CMD_INPUT_LIMIT_REG, mask, reg);
	}

	return 0;
}

static int smb1351_fastchg_current_set(struct smb1351_charger *chip,
				       unsigned int fastchg_current)
{
	int i, rc;
	bool is_pre_chg = false;

	mutex_lock(&chip->fcc_lock);

	if (fastchg_current < SMB1351_CHG_PRE_MIN_MA)
		fastchg_current = SMB1351_CHG_PRE_MIN_MA;
	if (fastchg_current > SMB1351_CHG_FAST_MAX_MA)
		fastchg_current = SMB1351_CHG_FAST_MAX_MA;

	if (fastchg_current < SMB1351_CHG_FAST_MIN_MA)
		is_pre_chg = true;

	if (is_pre_chg) {
		for (i = ARRAY_SIZE(pre_chg_current) - 1; i >= 0; i--) {
			if (pre_chg_current[i] <= fastchg_current)
				break;
		}
		if (i < 0)
			i = 0;
		chip->fastchg_current_max_ma = pre_chg_current[i];

		i = i << SMB1351_CHG_PRE_SHIFT;
		rc = smb1351_masked_write(chip, CHG_OTH_CURRENT_CTRL_REG,
					  PRECHG_CURRENT_MASK, i);
		if (rc)
			goto done;

		rc = smb1351_masked_write(chip, VARIOUS_FUNC_2_REG,
					  PRECHG_TO_FASTCHG_BIT,
					  PRECHG_TO_FASTCHG_BIT);
	} else {
		if (chip->version == SMB_UNKNOWN) {
			rc = -EINVAL;
			goto done;
		}

		if (chip->version == SMB1350 && fastchg_current > 2600)
			fastchg_current = 2600;

		for (i = ARRAY_SIZE(fast_chg_current) - 1; i >= 0; i--) {
			if (fast_chg_current[i] <= fastchg_current)
				break;
		}
		if (i < 0)
			i = 0;
		chip->fastchg_current_max_ma = fast_chg_current[i];

		i = i << SMB1351_CHG_FAST_SHIFT;

		rc = smb1351_masked_write(chip, VARIOUS_FUNC_2_REG,
					  PRECHG_TO_FASTCHG_BIT, 0);
		if (rc)
			goto done;

		rc = smb1351_masked_write(chip, CHG_CURRENT_CTRL_REG,
					  FAST_CHG_CURRENT_MASK, i);
	}
done:
	mutex_unlock(&chip->fcc_lock);
	return rc;
}

static int smb1351_float_voltage_set(struct smb1351_charger *chip, int vfloat_mv)
{
	u8 temp;

	if (vfloat_mv < MIN_FLOAT_MV || vfloat_mv > MAX_FLOAT_MV)
		return -EINVAL;

	temp = (vfloat_mv - MIN_FLOAT_MV) / VFLOAT_STEP_MV;

	return smb1351_masked_write(chip, VFLOAT_REG, VFLOAT_MASK, temp);
}

static int smb1351_parallel_set_present(struct smb1351_charger *chip,
					int present)
{
	u8 reg, mask;

	if (present == chip->parallel_charger_present)
		return 0;

	chip->parallel_charger_present = present;

	if (present) {
		int rc;

		rc = smb1351_read_reg(chip, CHG_REVISION_REG, &reg);
		if (rc) {
			dev_dbg(chip->dev, "Failed to detect smb1351\n");
			return -ENODEV;
		}

		smb_chip_get_version(chip);
		smb1351_enable_volatile_writes(chip);

		if (chip->vfloat_mv != -EINVAL)
			smb1351_float_voltage_set(chip, chip->vfloat_mv);

		if (chip->recharge_mv != -EINVAL) {
			reg = AUTO_RECHG_ENABLE;
			if (chip->recharge_mv > 50)
				reg |= AUTO_RECHG_TH_100MV;
			else
				reg |= AUTO_RECHG_TH_50MV;

			smb1351_masked_write(chip, CHG_CTRL_REG,
					     AUTO_RECHG_BIT |
					     AUTO_RECHG_TH_BIT, reg);
		}

		reg = chip->parallel_pin_polarity_setting | USBCS_CTRL_BY_I2C;
		smb1351_masked_write(chip, CHG_PIN_EN_CTRL_REG,
				     EN_PIN_CTRL_MASK | USBCS_CTRL_BIT, reg);

		smb1351_masked_write(chip, VARIOUS_FUNC_REG,
				     SUSPEND_MODE_CTRL_BIT,
				     SUSPEND_MODE_CTRL_BY_I2C);

		reg = USB_2_3_MODE_SEL_BY_I2C | USB_CMD_POLARITY_500_1_100_0;
		mask = USB_2_3_MODE_SEL_BIT | USB_5_1_CMD_POLARITY_BIT;
		smb1351_masked_write(chip, CHG_OTH_CURRENT_CTRL_REG, mask, reg);

		chip->target_fastchg_current_max_ma = SMB1351_CHG_FAST_MIN_MA;
		smb1351_fastchg_current_set(chip,
					    chip->target_fastchg_current_max_ma);

		chip->usb_psy_ma = SUSPEND_CURRENT_MA;
		smb1351_usb_suspend(chip, 4, true);
		smb1351_usb_suspend(chip, 16, false);
	} else {
		smb1351_usb_suspend(chip, 16, true);
	}

	return 0;
}

static int smb1351_parallel_set_property(struct power_supply *psy,
					 enum power_supply_property prop,
					 const union power_supply_propval *val)
{
	struct smb1351_charger *chip = power_supply_get_drvdata(psy);
	int rc = 0;

	switch (prop) {
	case POWER_SUPPLY_PROP_ONLINE:
		if (chip->parallel_charger_present)
			rc = smb1351_usb_suspend(chip, 1, !val->intval);
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		mutex_lock(&chip->config_lock);
		rc = smb1351_parallel_set_present(chip, val->intval);
		mutex_unlock(&chip->config_lock);
		break;
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX:
		if (chip->parallel_charger_present) {
			chip->target_fastchg_current_max_ma = val->intval / 1000;
			rc = smb1351_fastchg_current_set(chip,
					chip->target_fastchg_current_max_ma);
		}
		break;
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		if (chip->parallel_charger_present)
			rc = smb1351_set_usb_chg_current(chip, val->intval / 1000);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
		if (chip->parallel_charger_present)
			rc = smb1351_float_voltage_set(chip, val->intval / 1000);
		break;
	default:
		return -EINVAL;
	}

	return rc;
}

static int smb1351_parallel_get_property(struct power_supply *psy,
					 enum power_supply_property prop,
					 union power_supply_propval *val)
{
	struct smb1351_charger *chip = power_supply_get_drvdata(psy);

	switch (prop) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = chip->parallel_charger_present;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = chip->parallel_charger_present;
		break;
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX:
		val->intval = chip->fastchg_current_max_ma * 1000;
		break;
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		smb1351_get_usb_chg_current(chip, &val->intval);
		val->intval *= 1000;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
		val->intval = chip->vfloat_mv * 1000;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int smb1351_parallel_is_writeable(struct power_supply *psy,
					 enum power_supply_property prop)
{
	switch (prop) {
	case POWER_SUPPLY_PROP_ONLINE:
	case POWER_SUPPLY_PROP_PRESENT:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX:
	case POWER_SUPPLY_PROP_CURRENT_MAX:
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
		return 1;
	default:
		return 0;
	}
}

static enum power_supply_property smb1351_parallel_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
};

static const struct power_supply_desc smb1351_parallel_psy_desc = {
	.name			= "usb-parallel",
	.type			= POWER_SUPPLY_TYPE_USB,
	.properties		= smb1351_parallel_properties,
	.num_properties		= ARRAY_SIZE(smb1351_parallel_properties),
	.get_property		= smb1351_parallel_get_property,
	.set_property		= smb1351_parallel_set_property,
	.property_is_writeable	= smb1351_parallel_is_writeable,
};

static int smb1351_charger_probe(struct i2c_client *client)
{
	struct smb1351_charger *chip;
	struct power_supply_config psy_cfg = {};
	struct device *dev = &client->dev;
	struct device_node *node = dev->of_node;
	int rc;

	chip = devm_kzalloc(dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->client = client;
	chip->dev = dev;
	i2c_set_clientdata(client, chip);

	chip->charging_disabled = of_property_read_bool(node,
					"qcom,charging-disabled");
	chip->chg_autonomous_mode = of_property_read_bool(node,
					"qcom,chg-autonomous-mode");

	rc = of_property_read_u32(node, "qcom,float-voltage-mv",
				  &chip->vfloat_mv);
	if (rc)
		chip->vfloat_mv = -EINVAL;

	rc = of_property_read_u32(node, "qcom,recharge-mv", &chip->recharge_mv);
	if (rc)
		chip->recharge_mv = -EINVAL;

	rc = of_property_read_u32(node, "qcom,parallel-en-pin-polarity",
				  &chip->parallel_pin_polarity_setting);
	if (rc)
		chip->parallel_pin_polarity_setting = 0;
	else
		chip->parallel_pin_polarity_setting =
			chip->parallel_pin_polarity_setting ?
			EN_BY_PIN_HIGH_ENABLE : EN_BY_PIN_LOW_ENABLE;

	chip->switch_freq = -EINVAL;
	of_property_read_u32(node, "qcom,switch-freq", &chip->switch_freq);

	chip->disable_apsd = of_property_read_bool(node, "qcom,disable-apsd");

	mutex_init(&chip->config_lock);
	mutex_init(&chip->fcc_lock);

	psy_cfg.drv_data = chip;
	psy_cfg.fwnode = of_fwnode_handle(node);

	chip->parallel_psy = devm_power_supply_register(dev,
					&smb1351_parallel_psy_desc, &psy_cfg);
	if (IS_ERR(chip->parallel_psy)) {
		dev_err(dev, "Failed to register parallel power supply: %pe\n",
			chip->parallel_psy);
		return PTR_ERR(chip->parallel_psy);
	}

	return 0;
}

static void smb1351_charger_remove(struct i2c_client *client)
{
}

static const struct of_device_id smb1351_match_table[] = {
	{ .compatible = "qcom,smb1351-charger" },
	{ },
};
MODULE_DEVICE_TABLE(of, smb1351_match_table);

static const struct i2c_device_id smb1351_charger_id[] = {
	{ "smb1351-charger", 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, smb1351_charger_id);

static struct i2c_driver smb1351_charger_driver = {
	.driver = {
		.name		= "smb1351-charger",
		.of_match_table	= smb1351_match_table,
	},
	.probe		= smb1351_charger_probe,
	.remove		= smb1351_charger_remove,
	.id_table	= smb1351_charger_id,
};

module_i2c_driver(smb1351_charger_driver);

MODULE_DESCRIPTION("SMB1351/SMB1350 parallel charger driver");
MODULE_LICENSE("GPL v2");
