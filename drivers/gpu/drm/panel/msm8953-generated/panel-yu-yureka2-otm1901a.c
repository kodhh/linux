// SPDX-License-Identifier: GPL-2.0-only
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved.

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct otm1901a_dj {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct otm1901a_dj *to_otm1901a_dj(struct drm_panel *panel)
{
	return container_of(panel, struct otm1901a_dj, panel);
}

#define dsi_generic_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_generic_write(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void otm1901a_dj_reset(struct otm1901a_dj *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(50);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(50);
}

static int otm1901a_dj_on(struct otm1901a_dj *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0x19, 0x01, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xff, 0x19, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x1c, 0x33);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xc1, 0xe8);
	dsi_generic_write_seq(dsi, 0x00, 0xa7);
	dsi_generic_write_seq(dsi, 0xc1, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xc0, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xc0, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0x9a);
	dsi_generic_write_seq(dsi, 0xc0, 0x1e);
	dsi_generic_write_seq(dsi, 0x00, 0xac);
	dsi_generic_write_seq(dsi, 0xc0, 0x06);
	dsi_generic_write_seq(dsi, 0x00, 0xdc);
	dsi_generic_write_seq(dsi, 0xc0, 0x06);
	dsi_generic_write_seq(dsi, 0x00, 0x81);
	dsi_generic_write_seq(dsi, 0xa5, 0x04);
	dsi_generic_write_seq(dsi, 0x00, 0x84);
	dsi_generic_write_seq(dsi, 0xc4, 0x20);
	dsi_generic_write_seq(dsi, 0x00, 0xa5);
	dsi_generic_write_seq(dsi, 0xb3, 0x1d);
	dsi_generic_write_seq(dsi, 0x00, 0x92);
	dsi_generic_write_seq(dsi, 0xe9, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xf3, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0xb4);
	dsi_generic_write_seq(dsi, 0xc0, 0xd0);
	dsi_generic_write_seq(dsi, 0x00, 0x93);
	dsi_generic_write_seq(dsi, 0xc5, 0x19);
	dsi_generic_write_seq(dsi, 0x00, 0x95);
	dsi_generic_write_seq(dsi, 0xc5, 0x2d);
	dsi_generic_write_seq(dsi, 0x00, 0x97);
	dsi_generic_write_seq(dsi, 0xc5, 0x14);
	dsi_generic_write_seq(dsi, 0x00, 0x99);
	dsi_generic_write_seq(dsi, 0xc5, 0x29);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xd8, 0x1d, 0x1d);
	dsi_generic_write_seq(dsi, 0x00, 0xb3);
	dsi_generic_write_seq(dsi, 0xc0, 0xcc);
	dsi_generic_write_seq(dsi, 0x00, 0xb4);
	dsi_generic_write_seq(dsi, 0xc0, 0x80);
	dsi_generic_write_seq(dsi, 0x00, 0xbc);
	dsi_generic_write_seq(dsi, 0xc0, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xf7);
	dsi_generic_write_seq(dsi, 0xc3, 0x04, 0x18, 0x04, 0x04);
	dsi_generic_write_seq(dsi, 0x00, 0x81);
	dsi_generic_write_seq(dsi, 0xa5, 0x07);
	dsi_generic_write_seq(dsi, 0x00, 0x9d);
	dsi_generic_write_seq(dsi, 0xc5, 0x77);
	dsi_generic_write_seq(dsi, 0x00, 0x9b);
	dsi_generic_write_seq(dsi, 0xc5, 0x55, 0x55);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc4, 0x15);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc0,
			      0x00, 0x87, 0x00, 0x0a, 0x0a, 0x00, 0x87, 0x0a,
			      0x0a, 0x00, 0x87, 0x00, 0x0a, 0x0a);
	dsi_generic_write_seq(dsi, 0x00, 0xf0);
	dsi_generic_write_seq(dsi, 0xc3, 0x22, 0x02, 0x00, 0x00, 0x00, 0x0c);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xc0,
			      0x00, 0x00, 0x00, 0x00, 0x03, 0x22, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xc0,
			      0x00, 0x00, 0x00, 0x00, 0x03, 0x22, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xc2,
			      0x83, 0x01, 0x00, 0x00, 0x82, 0x01, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc3,
			      0x82, 0x02, 0x03, 0x00, 0x03, 0x84, 0x81, 0x03,
			      0x03, 0x00, 0x03, 0x84);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xc3,
			      0x00, 0x01, 0x03, 0x00, 0x03, 0x84, 0x01, 0x02,
			      0x03, 0x00, 0x03, 0x84);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x09, 0x0a, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
			      0x17, 0x18, 0x28, 0x28, 0x28, 0x28, 0x28);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x0a, 0x09, 0x14, 0x13, 0x12, 0x11, 0x15, 0x16,
			      0x17, 0x18, 0x28, 0x28, 0x28, 0x28, 0x28);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x1d, 0x1e, 0x1f, 0x19, 0x1a, 0x1b, 0x1c, 0x20,
			      0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x04, 0x08);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x77);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x77);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x01, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x77);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x77);
	dsi_generic_write_seq(dsi, 0x00, 0xe0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x77);
	dsi_generic_write_seq(dsi, 0x00, 0xf0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x77, 0x77);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xcd,
			      0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
			      0x3f, 0x02, 0x12, 0x11, 0x3f, 0x04, 0x3f);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xcd,
			      0x06, 0x3f, 0x3f, 0x26, 0x26, 0x26, 0x21, 0x20,
			      0x1f, 0x26, 0x26);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xcd,
			      0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
			      0x3f, 0x01, 0x12, 0x11, 0x3f, 0x03, 0x3f);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xcd,
			      0x05, 0x3f, 0x3f, 0x26, 0x26, 0x26, 0x21, 0x20,
			      0x1f, 0x26, 0x26);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe1,
			      0x30, 0x35, 0x36, 0x3a, 0x40, 0x44, 0x46, 0x52,
			      0x58, 0x66, 0x6c, 0x72, 0x88, 0x81, 0x77, 0x60,
			      0x4d, 0x39, 0x2c, 0x24, 0x12, 0x09, 0x06, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe2,
			      0x30, 0x35, 0x36, 0x3a, 0x40, 0x44, 0x46, 0x52,
			      0x58, 0x66, 0x6c, 0x72, 0x88, 0x81, 0x77, 0x60,
			      0x4d, 0x39, 0x2c, 0x24, 0x12, 0x09, 0x06, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe3,
			      0x1c, 0x1d, 0x20, 0x25, 0x28, 0x2c, 0x36, 0x43,
			      0x48, 0x59, 0x63, 0x6c, 0x8d, 0x84, 0x7a, 0x63,
			      0x4d, 0x39, 0x2c, 0x24, 0x1d, 0x11, 0x07, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe4,
			      0x1c, 0x1d, 0x20, 0x25, 0x28, 0x2c, 0x36, 0x43,
			      0x48, 0x59, 0x63, 0x6c, 0x8d, 0x84, 0x7a, 0x63,
			      0x4d, 0x39, 0x2c, 0x24, 0x1d, 0x11, 0x07, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe5,
			      0x00, 0x07, 0x08, 0x0d, 0x13, 0x1b, 0x25, 0x36,
			      0x40, 0x52, 0x5f, 0x69, 0x8f, 0x86, 0x7b, 0x63,
			      0x4d, 0x39, 0x2c, 0x24, 0x1e, 0x0d, 0x04, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe6,
			      0x00, 0x07, 0x08, 0x0d, 0x13, 0x1b, 0x25, 0x36,
			      0x40, 0x52, 0x5f, 0x69, 0x8f, 0x86, 0x7b, 0x63,
			      0x4c, 0x38, 0x30, 0x20, 0x1e, 0x0d, 0x04, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x81);
	dsi_generic_write_seq(dsi, 0xa5, 0x07);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc4, 0x1a);
	dsi_generic_write_seq(dsi, 0x00, 0x9b);
	dsi_generic_write_seq(dsi, 0xc5, 0x55, 0x50);
	dsi_generic_write_seq(dsi, 0x00, 0xf7);
	dsi_generic_write_seq(dsi, 0xc3, 0x04, 0x16, 0x04, 0x04);
	dsi_generic_write_seq(dsi, 0x00, 0xf2);
	dsi_generic_write_seq(dsi, 0xc1, 0x80, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x9d);
	dsi_generic_write_seq(dsi, 0xc5, 0x77);
	dsi_generic_write_seq(dsi, 0x00, 0xf7);
	dsi_generic_write_seq(dsi, 0xc3, 0x00, 0xff, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xf2);
	dsi_generic_write_seq(dsi, 0xc1, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xf5, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0xff, 0xff, 0xff);
	dsi_generic_write_seq(dsi, 0x51, 0x00);
	dsi_generic_write_seq(dsi, 0x53, 0x24);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}

	return 0;
}

static int otm1901a_dj_off(struct otm1901a_dj *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(50);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xf7, 0x5a, 0xa5, 0x19, 0x01);

	return 0;
}

static int otm1901a_dj_prepare(struct drm_panel *panel)
{
	struct otm1901a_dj *ctx = to_otm1901a_dj(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	otm1901a_dj_reset(ctx);

	ret = otm1901a_dj_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int otm1901a_dj_unprepare(struct drm_panel *panel)
{
	struct otm1901a_dj *ctx = to_otm1901a_dj(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = otm1901a_dj_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode otm1901a_dj_mode = {
	.clock = (1080 + 52 + 4 + 60) * (1920 + 12 + 2 + 14) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 52,
	.hsync_end = 1080 + 52 + 4,
	.htotal = 1080 + 52 + 4 + 60,
	.vdisplay = 1920,
	.vsync_start = 1920 + 12,
	.vsync_end = 1920 + 12 + 2,
	.vtotal = 1920 + 12 + 2 + 14,
	.width_mm = 68,
	.height_mm = 120,
};

static int otm1901a_dj_get_modes(struct drm_panel *panel,
				 struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &otm1901a_dj_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs otm1901a_dj_panel_funcs = {
	.prepare = otm1901a_dj_prepare,
	.unprepare = otm1901a_dj_unprepare,
	.get_modes = otm1901a_dj_get_modes,
};

static int otm1901a_dj_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct otm1901a_dj *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &otm1901a_dj_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		return ret;
	}

	return 0;
}

static int otm1901a_dj_remove(struct mipi_dsi_device *dsi)
{
	struct otm1901a_dj *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id otm1901a_dj_of_match[] = {
	{ .compatible = "yu,yureka2-otm1901a" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, otm1901a_dj_of_match);

static struct mipi_dsi_driver otm1901a_dj_driver = {
	.probe = otm1901a_dj_probe,
	.remove = otm1901a_dj_remove,
	.driver = {
		.name = "panel-otm1901a-dj",
		.of_match_table = otm1901a_dj_of_match,
	},
};
module_mipi_dsi_driver(otm1901a_dj_driver);

MODULE_AUTHOR("YU Yureka 2 contributors");
MODULE_DESCRIPTION("DRM driver for otm1901a_1080p_video_DJ");
MODULE_LICENSE("GPL v2");
