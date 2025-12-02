/**
 * @file ciot_util.h
 * @ingroup utils
 * @brief Header file for CIOT utility functions.
 * @version 0.1
 * @date 2023-12-08
 * @author Wesley Santos (wesleypro37@gmail.com)
 * @copyright Copyright (c) 2023
 */

#ifndef __CIOT_UTILS__H__
#define __CIOT_UTILS__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

#define CONSTRAIN(value, min, max) (((value) < (min)) ? (min) : ((value) > (max)) ? (max) : (value))

/**
 * @brief Convert bytes to hexadecimal representation.
 *
 * @param dst Destination buffer for the hexadecimal string.
 * @param src Source buffer containing bytes to convert.
 * @param size Size of the source buffer.
 * @param reverse Flag indicating whether to reverse byte order (little-endian).
 */
void bytes_to_hex(char *dst, uint8_t *src, size_t size, bool reverse);

/**
 * @brief Convert an hex char to byte
 * 
 * @param c hex char
 * @return unsigned char byte
 */
unsigned char hex_char_to_byte(char c);

/**
 * @brief convert an hex string to byte
 * 
 * @param hex_str hex string
 * @param byte_array output buffer
 * @param byte_array_len output buffer size
 * @return int 
 */
int hex_string_to_bytes(const char *hex_str, unsigned char *byte_array, size_t byte_array_len);

/**
 * @brief Compute CRC32 checksum for the given data.
 *
 * @param p_data Pointer to the data buffer.
 * @param size Size of the data buffer.
 * @param p_crc Pointer to the initial CRC32 value (optional, can be NULL).
 * @return Computed CRC32 checksum.
 */
uint32_t crc32_compute(uint8_t const *p_data, uint32_t size, uint32_t const *p_crc);

/**
 * @brief Copy data
 * 
 * @param destiny Pointer to destiny
 * @param source Pointer to source
 * @param size data size
 * @param reverse flag to resverse data order
 */
void ciot_copy_data(uint8_t *destiny, uint8_t *source, int size, bool reverse);

/**
 * @brief Maps a value from one range to another.
 * 
 * This function linearly maps the input value `in`, which is assumed to be in the range 
 * [`in_min`, `in_max`], to a corresponding value in the range [`out_min`, `out_max`].
 * If `in_max` is equal to `in_min`, the function returns `out_min` to avoid division by zero.
 * 
 * @param in       The input value to be mapped.
 * @param in_min   The lower bound of the input range.
 * @param in_max   The upper bound of the input range.
 * @param out_min  The lower bound of the output range.
 * @param out_max  The upper bound of the output range.
 * @return double  The mapped value in the output range.
 */
static inline double map_range(double in, double in_min, double in_max, double out_min, double out_max)
{
    if (in_max == in_min) return out_min;
    return (in - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_UTILS__H__
