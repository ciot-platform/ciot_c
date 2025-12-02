/**
 * @file ciot_err.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __CIOT_ERR__H__
#define __CIOT_ERR__H__

#include "ciot_types.h"
#include "ciot_log.h"

#define CIOT_ERR_LOG(TAG, x) CIOT_LOGE(TAG, "%s", ciot_err_to_message(x))

#define CIOT_ERR_NULL_CHECK(x)                       \
    if (x == NULL)                                   \
    {                                                \
        CIOT_ERR_LOG("ciot_err", CIOT_ERR_NULL_ARG); \
        return CIOT_ERR_NULL_ARG;                    \
    }

#define CIOT_ERR_TYPE_CHECK(current, expected)                                 \
    if (current != expected)                                                   \
    {                                                                          \
        CIOT_ERR_LOG("ciot_err", CIOT_ERR_INVALID_TYPE);                       \
        CIOT_LOGE("ciot_err", "current: %d, expected: %d", current, expected); \
        return CIOT_ERR_INVALID_TYPE;                                          \
    }

#define CIOT_ERR_MSG_DATA_TAG_CHECK(current, expected)                         \
    if (current != expected && current != CIOT_MSG_DATA_COMMON_TAG)            \
    {                                                                          \
        CIOT_ERR_LOG("ciot_err", CIOT_ERR_INVALID_TYPE);                       \
        CIOT_LOGE("ciot_err", "current: %d, expected: %d", current, expected); \
        return CIOT_ERR_INVALID_TYPE;                                          \
    }

#define CIOT_ERR_SIZE_CHECK(size, min) \
    if (size <= min)                   \
        return CIOT_ERR_INVALID_SIZE;

#define CIOT_ERR_INDEX_CHECK(index, min, max) \
    if (index < min)                          \
        return CIOT_ERR_INVALID_INDEX;        \
    if (index > max)                          \
        return CIOT_ERR_INVALID_INDEX;

#define CIOT_ERR_UINDEX_CHECK(index, max) \
    if (index > max)                      \
        return CIOT_ERR_INVALID_INDEX;

#define CIOT_ERR_ID_CHECK(id, size) \
    if (id < 0)                     \
        return CIOT_ERR_INVALID_ID; \
    if (id >= size)                 \
    return CIOT_ERR_NO_MEMORY

#define CIOT_ERR_CAPACITY_CHECK(index, size) \
    if (index < 0)                           \
        return CIOT_ERR_INVALID_INDEX;       \
    if (index >= size)                       \
        return CIOT_ERR_NO_MEMORY;

#define CIOT_ERR_EXISTENCE_CHECK(id, size) \
    if (id < 0)                            \
        return CIOT_ERR_INVALID_ID;        \
    if (id >= size)                        \
        return CIOT_ERR_NOT_FOUND;

#define CIOT_ERR_STATE_CHECK(current, expected)           \
    if (current != expected)                              \
    {                                                     \
        CIOT_ERR_LOG("ciot_err", CIOT_ERR_INVALID_STATE); \
        return CIOT_ERR_INVALID_STATE;                    \
    }

#define CIOT_ERR_VALUE_CHECK(current, expected, err) \
    if (current != expected)                         \
    {                                                \
        CIOT_ERR_LOG("ciot_err", err);               \
        return err;                                  \
    }

#define CIOT_ERR_EMPTY_STRING_CHECK(x)                      \
    if (x[0] == '\0')                                       \
    {                                                       \
        /*CIOT_ERR_LOG("ciot_err", CIOT_ERR_INVALID_ARG);*/ \
        return CIOT_ERR_INVALID_ARG;                        \
    }

#define CIOT_ERR_RETURN(x)                      \
    do                                          \
    {                                           \
        ciot_err_t ciot_err = x;                \
        if (ciot_err != CIOT_ERR_OK)            \
        {                                       \
            CIOT_ERR_LOG("ciot_err", ciot_err); \
            return ciot_err;                    \
        }                                       \
    } while (0)

#define CIOT_ERR_PRINT(TAG, x)           \
    do                                   \
    {                                    \
        ciot_err_t ciot_err = x;         \
        if (ciot_err != CIOT_ERR_OK)     \
        {                                \
            CIOT_ERR_LOG(TAG, ciot_err); \
        }                                \
    } while (0)

const char *ciot_err_to_message(ciot_err_t error);

#endif //!__CIOT_ERR__H__