/**
 * @file ciot_mbus.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ciot_mbus.h"

ciot_err_t ciot_mbus_get_error(nmbs_error error)
{
    switch (error)
    {
    case NMBS_ERROR_INVALID_REQUEST:
        return CIOT_ERR_INVALID_REQUEST;
    case NMBS_ERROR_INVALID_UNIT_ID:
        return CIOT_ERR_INVALID_ID;
    case NMBS_ERROR_INVALID_TCP_MBAP:
        return CIOT_ERR_INVALID_TCP_MBAP;
    case NMBS_ERROR_CRC:
        return CIOT_ERR_CRC;
    case NMBS_ERROR_TRANSPORT:
        return CIOT_ERR_TRANSPORT;
    case NMBS_ERROR_TIMEOUT:
        return CIOT_ERR_TIMEOUT;
    case NMBS_ERROR_INVALID_RESPONSE:
        return CIOT_ERR_INVALID_RESPONSE;
    case NMBS_ERROR_INVALID_ARGUMENT:
        return CIOT_ERR_INVALID_ARG;
    case NMBS_ERROR_NONE:
        return CIOT_ERR_OK;
    case NMBS_EXCEPTION_ILLEGAL_FUNCTION:
        return CIOT_ERR_MBUS_EXCEPTION_ILLEGAL_FUNCTION;
    case NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS:
        return CIOT_ERR_MBUS_EXCEPTION_ILLEGAL_DATA_ADDR;
    case NMBS_EXCEPTION_ILLEGAL_DATA_VALUE:
        return CIOT_ERR_MBUS_EXCEPTION_ILLEGAL_DATA_ADDR;
    case NMBS_EXCEPTION_SERVER_DEVICE_FAILURE:
        return CIOT_ERR_MBUS_EXCEPTION_SERVER_DEVICE_FAILURE;
    default:
        return CIOT_ERR_UNKNOWN;
    }
}
