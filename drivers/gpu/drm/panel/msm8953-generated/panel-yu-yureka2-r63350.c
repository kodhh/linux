// SPDX-License-Identifier: GPL-2.0-only
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved.

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct r63350 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct r63350 *to_r63350(struct drm_panel *panel)
{
	return container_of(panel, struct r63350, panel);
}

#define dsi_generic_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_generic_write(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void r63350_reset(struct r63350 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int r63350_on(struct r63350 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_generic_write_seq(dsi, 0xb0, 0x04);
	dsi_generic_write_seq(dsi, 0xb3, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xb4, 0x0c, 0x00);
	dsi_generic_write_seq(dsi, 0xb6, 0x4b, 0xdb, 0x16);
	dsi_generic_write_seq(dsi, 0xb8,
			      0x57, 0x3d, 0x19, 0x1e, 0x0a, 0x50, 0x50);
	dsi_generic_write_seq(dsi, 0xb9,
			      0x6f, 0x3d, 0x28, 0x3c, 0x14, 0xc8, 0xc8);
	dsi_generic_write_seq(dsi, 0xba,
			      0xb5, 0x33, 0x41, 0x64, 0x23, 0xa0, 0xa0);
	dsi_generic_write_seq(dsi, 0xbb, 0x14, 0x14);
	dsi_generic_write_seq(dsi, 0xbc, 0x37, 0x32);
	dsi_generic_write_seq(dsi, 0xbd, 0x64, 0x32);
	dsi_generic_write_seq(dsi, 0xc0, 0x00);
	dsi_generic_write_seq(dsi, 0xc1,
			      0x04, 0x60, 0x00, 0x40, 0x10, 0x00, 0x58, 0x03,
			      0x00, 0x00, 0x00, 0x64, 0x84, 0x30, 0x4a, 0x00,
			      0x00, 0x9d, 0xc1, 0x01, 0x00, 0xca, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x02, 0x20,
			      0x03, 0x98);
	dsi_generic_write_seq(dsi, 0xc2,
			      0x31, 0xf7, 0x80, 0x06, 0x08, 0x00, 0x00, 0x08);
	dsi_generic_write_seq(dsi, 0xc3, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xc4,
			      0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x01, 0x06);
	dsi_generic_write_seq(dsi, 0xc6,
			      0x77, 0x01, 0x6e, 0x02, 0x67, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x04, 0x19, 0x0a, 0x77);
	dsi_generic_write_seq(dsi, 0xc7,
			      0x00, 0x16, 0x1d, 0x25, 0x32, 0x3e, 0x48, 0x56,
			      0x39, 0x40, 0x4b, 0x57, 0x61, 0x68, 0x77, 0x00,
			      0x16, 0x1d, 0x25, 0x32, 0x3e, 0x48, 0x56, 0x39,
			      0x40, 0x4b, 0x57, 0x61, 0x68, 0x77);
	dsi_generic_write_seq(dsi, 0xcb,
			      0xe6, 0xe0, 0xc7, 0x63, 0x00, 0x00, 0x00, 0x00,
			      0x20, 0xe0, 0x87, 0x00, 0xe8, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xcc, 0x06);
	dsi_generic_write_seq(dsi, 0xce,
			      0x55, 0x40, 0x49, 0x53, 0x59, 0x5e, 0x63, 0x68,
			      0x6e, 0x74, 0x7e, 0x8a, 0x98, 0xa8, 0xbb, 0xd0,
			      0xff, 0x04, 0x00, 0x04, 0x04, 0x42, 0x00, 0x69,
			      0x5a);
	dsi_generic_write_seq(dsi, 0xd0,
			      0x11, 0x00, 0x00, 0x56, 0xcb, 0x40, 0x19, 0x19,
			      0x09, 0x00);
	dsi_generic_write_seq(dsi, 0xd1, 0x00, 0x48, 0x16, 0x0f);
	dsi_generic_write_seq(dsi, 0xd3,
			      0x1b, 0x33, 0x99, 0xbb, 0xb3, 0x33, 0x33, 0x33,
			      0x11, 0x00, 0x01, 0x00, 0x00, 0xe8, 0xa0, 0x02,
			      0x2f, 0x27, 0x33, 0x33, 0x72, 0x12, 0x8a, 0x57,
			      0x3d, 0xbc);
	dsi_generic_write_seq(dsi, 0xd4, 0x41, 0x04, 0x00, 0xd5);
	dsi_generic_write_seq(dsi, 0xd6, 0x01);
	dsi_generic_write_seq(dsi, 0xd7,
			      0xbf, 0xf8, 0x7f, 0xa8, 0xce, 0x3e, 0xfc, 0xc1,
			      0xe1, 0xef, 0x83, 0x07, 0x3f, 0x10, 0x7f, 0xc0,
			      0x01, 0xe7, 0x40, 0x3c, 0x00, 0xc0, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xd9, 0x20, 0x00, 0x14);
	dsi_generic_write_seq(dsi, 0xdd, 0x30, 0x06, 0x23, 0x65);
	dsi_generic_write_seq(dsi, 0xde, 0x00, 0x3f, 0xff, 0x10);
	dsi_dcs_write_seq(dsi, 0x29, 0x00);
	msleep(32);
	dsi_dcs_write_seq(dsi, 0x11, 0x00);
	msleep(128);

	return 0;
}

static int r63350_off(struct r63350 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	usleep_range(16000, 17000);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	dsi_generic_write_seq(dsi, 0xb0, 0x04);
	dsi_generic_write_seq(dsi, 0xb1, 0x01);

	return 0;
}

static int r63350_prepare(struct drm_panel *panel)
{
	struct r63350 *ctx = to_r63350(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	r63350_reset(ctx);

	ret = r63350_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int r63350_unprepare(struct drm_panel *panel)
{
	struct r63350 *ctx = to_r63350(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = r63350_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode r63350_mode = {
	.clock = (1080 + 320 + 12 + 36) * (1920 + 4 + 2 + 4) * 59 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 320,
	.hsync_end = 1080 + 320 + 12,
	.htotal = 1080 + 320 + 12 + 36,
	.vdisplay = 1920,
	.vsync_start = 1920 + 4,
	.vsync_end = 1920 + 4 + 2,
	.vtotal = 1920 + 4 + 2 + 4,
	.width_mm = 68,
	.height_mm = 120,
};

static int r63350_get_modes(struct drm_panel *panel,
			    struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &r63350_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static int r63350_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

static const struct backlight_ops r63350_bl_ops = {
	.update_status = r63350_bl_update_status,
};

static struct backlight_device *
r63350_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &r63350_bl_ops, &props);
}

static const struct drm_panel_funcs r63350_panel_funcs = {
	.prepare = r63350_prepare,
	.unprepare = r63350_unprepare,
	.get_modes = r63350_get_modes,
};

static int r63350_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct r63350 *ctx;
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
			  MIPI_DSI_MODE_VIDEO_HSE |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &r63350_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ctx->panel.backlight = r63350_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		return ret;
	}

	return 0;
}

static void r63350_remove(struct mipi_dsi_device *dsi)
{
	struct r63350 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id r63350_of_match[] = {
	{ .compatible = "yu,yureka2-r63350" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, r63350_of_match);

static struct mipi_dsi_driver r63350_driver = {
	.probe = r63350_probe,
	.remove = r63350_remove,
	.driver = {
		.name = "panel-r63350",
		.of_match_table = r63350_of_match,
	},
};
module_mipi_dsi_driver(r63350_driver);

MODULE_AUTHOR("YU Yureka 2 contributors");
MODULE_DESCRIPTION("DRM driver for r63350_1080p_video_Tcl");
MODULE_LICENSE("GPL v2");
