/// @file color.h
/// @author namazso
/// @date 2017-10-09
/// @brief An RGBA 32 bit color.
///
/// Header only implementation of a simple 32 bit RGBA color struct.

#pragma once

/// @addtogroup color
/// @{

/// A 32 bit RGBA color.
struct color
{
	/// Red.
	uint8_t r;

	/// Green.
	uint8_t g;

	/// Blue.
	uint8_t b;

	/// Alpha.
	uint8_t a;
};

/// RGBA Color.
///
/// Generates a color struct from RGBA values.
///
/// @param[in] r Red.
/// @param[in] g Green.
/// @param[in] b Blue.
/// @param[in] a Alpha.
/// @return color struct
inline struct color color_rgba(
	uint8_t r,
	uint8_t g,
	uint8_t b,
	uint8_t a)
{
	struct color c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}

/// RGB Color.
///
/// Generates a color struct from RGB values, with 255 alpha.
///
/// @param[in] r Red.
/// @param[in] g Green.
/// @param[in] b Blue.
/// @return color struct
inline struct color color_rgb(
	uint8_t r,
	uint8_t g,
	uint8_t b)
{
	return color_rgba(r, g, b, 255);
}

/// Blend a foreground on a background with alpha.
///
/// Blends the foreground color onto the background color. Ignores
/// background alpha.
///
/// @param[in] bg Background color.
/// @param[in] fg Foreground color.
/// @return The blended color.
inline struct color color_alpha_blend(struct color bg, struct color fg)
{
	struct color r;
	const uint16_t alpha = fg.a + 1;
	const uint16_t inv_alpha = 256 - fg.a;
	r.r = (uint8_t)((alpha * fg.r + inv_alpha * bg.r) >> 8);
	r.g = (uint8_t)((alpha * fg.g + inv_alpha * bg.g) >> 8);
	r.b = (uint8_t)((alpha * fg.b + inv_alpha * bg.b) >> 8);
	r.a = 0xFF;
	return r;
}

/// @}