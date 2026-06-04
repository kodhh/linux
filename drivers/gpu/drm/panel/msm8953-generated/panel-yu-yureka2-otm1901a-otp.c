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

struct otm1901a_dj_otp {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct otm1901a_dj_otp *to_otm1901a_dj_otp(struct drm_panel *panel)
{
	return container_of(panel, struct otm1901a_dj_otp, panel);
}

#define dsi_generic_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_generic_write(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void otm1901a_dj_otp_reset(struct otm1901a_dj_otp *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
}

static int otm1901a_dj_otp_on(struct otm1901a_dj_otp *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0x19, 0x01, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xff, 0x19, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x35, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc4, 0x13);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0xff, 0xff, 0xff);

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

	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x99, 0x11, 0x11);

	return 0;
}

static int otm1901a_dj_otp_off(struct otm1901a_dj_otp *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x99, 0x95, 0x27);

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
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x99, 0x11, 0x11);

	return 0;
}

static int otm1901a_dj_otp_prepare(struct drm_panel *panel)
{
	struct otm1901a_dj_otp *ctx = to_otm1901a_dj_otp(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	otm1901a_dj_otp_reset(ctx);

	ret = otm1901a_dj_otp_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int otm1901a_dj_otp_unprepare(struct drm_panel *panel)
{
	struct otm1901a_dj_otp *ctx = to_otm1901a_dj_otp(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = otm1901a_dj_otp_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode otm1901a_dj_otp_mode = {
	.clock = (1080 + 320 + 4 + 32) * (1920 + 14 + 2 + 9) * 59 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 320,
	.hsync_end = 1080 + 320 + 4,
	.htotal = 1080 + 320 + 4 + 32,
	.vdisplay = 1920,
	.vsync_start = 1920 + 14,
	.vsync_end = 1920 + 14 + 2,
	.vtotal = 1920 + 14 + 2 + 9,
	.width_mm = 68,
	.height_mm = 120,
};

static int otm1901a_dj_otp_get_modes(struct drm_panel *panel,
				     struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &otm1901a_dj_otp_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs otm1901a_dj_otp_panel_funcs = {
	.prepare = otm1901a_dj_otp_prepare,
	.unprepare = otm1901a_dj_otp_unprepare,
	.get_modes = otm1901a_dj_otp_get_modes,
};

static int otm1901a_dj_otp_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct otm1901a_dj_otp *ctx;
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

	drm_panel_init(&ctx->panel, dev, &otm1901a_dj_otp_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		return ret;
	}

	return 0;
}

static int otm1901a_dj_otp_remove(struct mipi_dsi_device *dsi)
{
	struct otm1901a_dj_otp *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id otm1901a_dj_otp_of_match[] = {
	{ .compatible = "yu,yureka2-otm1901a-otp" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, otm1901a_dj_otp_of_match);

static struct mipi_dsi_driver otm1901a_dj_otp_driver = {
	.probe = otm1901a_dj_otp_probe,
	.remove = otm1901a_dj_otp_remove,
	.driver = {
		.name = "panel-otm1901a-dj-otp",
		.of_match_table = otm1901a_dj_otp_of_match,
	},
};
module_mipi_dsi_driver(otm1901a_dj_otp_driver);

MODULE_AUTHOR("YU Yureka 2 contributors");
MODULE_DESCRIPTION("DRM driver for otm1901a_1080p_video_DJ_otp");
MODULE_LICENSE("GPL v2");
