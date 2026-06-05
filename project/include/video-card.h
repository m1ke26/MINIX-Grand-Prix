#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

/**
 * @brief Sets the video mode to the specified mode using VBE.
 * @param mode The VBE mode to set (e.g. 0x105 for 800x600).
 * @return 0 on success, -1 on failure.
 */
int set_video_mode(uint16_t mode);

/**
 * @brief Maps the video memory for the current mode and initializes necessary variables.
 * @param mode The VBE mode that was set (used to determine resolution and color depth
 * @return 0 on success, -1 on failure.
 */
int map_video_memory(uint16_t mode);

/**
 * @brief Draws a line at the specified coordinates with the given color.
 * @param x The x coordinate of the starting point of the line.
 * @param y The y coordinate of the starting point of the line.
 * @param len The length of the line in pixels.
 * @param color The color of the line in 0xRRGGBB format.
 * @return 0 on success, -1 on failure.
 */
int vg_draw_line(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * @brief Draws a pixel at the specified coordinates with the given color.
 * @param x The x coordinate of the pixel.
 * @param y The y coordinate of the pixel.
 * @param color The color of the pixel in 0xRRGGBB format.
 * @return 0 on success, -1 on failure.
 */
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief Draws a filled rectangle at the specified coordinates with the given color.
 * @param x The x coordinate of the top-left corner of the rectangle.
 * @param y The y coordinate of the top-left corner of the rectangle.
 * @param width The width of the rectangle in pixels.
 * @param height The height of the rectangle in pixels.
 * @param color The color of the rectangle in 0xRRGGBB format.
 * @return 0 on success, -1 on failure.
 */
int vg_draw_rect(uint16_t x, uint16_t y, uint16_t width,
                      uint16_t height, uint32_t color);

/**
 * @brief Draws an XPM image at the specified coordinates.
 * @param xpm The XPM image to draw.
 * @param x The x coordinate of the top-left corner of the image.
 * @param y The y coordinate of the top-left corner of the image.
 * @return 0 on success, -1 on failure.
 */
int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

/* ---- Double buffering ---- */

/**
 * @brief Initializes the back buffer for double buffering.
 * @param width The width of the back buffer (should match screen width).
 * @param height The height of the back buffer (should match screen height).
 * @param bytes_per_pixel The number of bytes per pixel (e.g. 3 for 24-bit color).
 * @note The back buffer is used for off-screen drawing to prevent flickering. After drawing to the back buffer, call vg_buf_swap() to copy it to the screen.
 * @return 0 on success, -1 on failure.
 */
int  vg_init_double_buffer(uint16_t width, uint16_t height, uint8_t bytes_per_pixel);

/**
 * @brief Draws a pixel to the back buffer at the specified coordinates with the given color.
 * @param x The x coordinate of the pixel.
 * @param y The y coordinate of the pixel.
 * @param color The color of the pixel in 0xRRGGBB format.
 */
void vg_buf_draw_pixel(int x, int y, uint32_t color);

/**
 * @brief Draws a filled rectangle to the back buffer at the specified coordinates with the given color.
 * @param x The x coordinate of the top-left corner of the rectangle.
 * @param y The y coordinate of the top-left corner of the rectangle.
 * @param w The width of the rectangle in pixels.
 * @param h The height of the rectangle in pixels.
 * @param color The color of the rectangle in 0xRRGGBB format.
 */
void vg_buf_draw_rect(int x, int y, int w, int h, uint32_t color);

/**
 * @brief Clear the back buffer to black.
 */
void vg_buf_clear(void);

/**
 * @brief Copy back buffer to framebuffer (one memcpy).
 */
void vg_buf_swap(void);

/**
 * @brief Free back buffer memory.
 */
void vg_free_double_buffer(void);

/**
 * @brief Converts every pixel in the back buffer to greyscale in-place.
 * @note Call after drawing the game world and before drawing the pause menu.
 */
void vg_buf_desaturate(void);
