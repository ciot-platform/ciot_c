/**
 * @file ciot_err.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_err.h"

const char *ciot_err_to_message(ciot_err_t error)
{
    switch (error)
    {
    case CIOT_ERR_OK:
        return "ERR_OK";
    case CIOT_ERR_NULL_ARG:
        return "ERR_NULL_ARG";
    case CIOT_ERR_INVALID_ID:
        return "ERR_INVALID_ID";
    case CIOT_ERR_INVALID_TYPE:
        return "ERR_INVALID_TYPE";
    case CIOT_ERR_OVERFLOW:
        return "ERR_OVERFLOW";
    case CIOT_ERR_NOT_IMPLEMENTED:
        return "ERR_NOT_IMPLEMENTED";
    case CIOT_ERR_NOT_SUPPORTED:
        return "ERR_NOT_SUPPORTED";
    case CIOT_ERR_BUSY:
        return "ERR_BUSY";
    case CIOT_ERR_INVALID_STATE:
        return "ERR_INVALID_STATE";
    case CIOT_ERR_SERIALIZATION:
        return "ERR_SERIALIZATION";
    case CIOT_ERR_DESERIALIZATION:
        return "ERR_DESERIALIZATION";
    case CIOT_ERR_SEND_DATA:
        return "ERR_SEND_DATA";
    case CIOT_ERR_RECV_DATA:
        return "ERR_RECV_DATA";
    case CIOT_ERR_INVALID_SIZE:
        return "ERR_INVALID_SIZE";
    case CIOT_ERR_CLOSED:
        return "ERR_CLOSED";
    case CIOT_ERR_NOT_FOUND:
        return "ERR_NOT_FOUND";
    case CIOT_ERR_VALIDATION_FAILED:
        return "ERR_VALIDATION_FAILED";
    case CIOT_ERR_CONNECTION:
        return "ERR_CONNECTION";
    case CIOT_ERR_DISCONNECTION:
        return "ERR_DISCONNECTION";
    case CIOT_ERR_EXCEPTION:
        return "ERR_EXCEPTION";
    case CIOT_ERR_INVALID_ARG:
        return "ERR_INVALID_ARG";
    case CIOT_ERR_NO_MEMORY:
        return "ERR_NO_MEMORY";
    case CIOT_ERR_TIMEOUT:
        return "ERR_TIMEOUT";
    case CIOT_ERR_MONGOOSE:
        return "ERR_MONGOOSE";
    case CIOT_ERR_IMPOSSIBLE_OP:
        return "ERR_IMPOSSIBLE_OP";
    case CIOT_ERR_CHECKSUM:
        return "ERR_CHECKSUM";
    case CIOT_ERR_PROTOCOL_VIOLATION:
        return "ERR_PROTOCOL_VIOLATION";
    case CIOT_ERR_DATA_LOSS:
        return "ERR_DATA_LOSS";
    case CIOT_ERR_INVALID_HEADER:
        return "ERR_INVALID_HEADER";
    case CIOT_ERR_DISABLED:
        return "ERR_DISABLED";
    case CIOT_ERR_FAIL:
        return "ERR_FAIL";
    case CIOT_ERR_INVALID_INDEX:
        return "ERR_INVALID_INDEX";
    case CIOT_ERR_INVALID_KEY_SIZE:
        return "ERR_INVALID_KEY_SIZE";
    case CIOT_ERR_FIFO_READ:
        return "ERR_FIFO_READ";
    case CIOT_ERR_FIFO_WRITE:
        return "ERR_FIFO_WRITE";
    case CIOT_ERR_INVALID_ADDR:
        return "ERR_INVALID_ADDR";
    case CIOT_ERR_RESOURCES:
        return "ERR_RESOURCES";
    case CIOT_ERR_UNKNOWN:
        return "ERR_UNKNOWN";
    case CIOT_ERR_READING:
        return "ERR_READING";
    case CIOT_ERR_ERASING:
        return "ERR_ERASING";
    case CIOT_ERR_WRITING:
        return "ERR_WRITING";
    case CIOT_ERR_INVALID_NAME:
        return "ERR_INVALID_NAME";
    case CIOT_ERR_DECODER_MISSING:
        return "ERR_DECODER_MISSING";
    case CIOT_ERR_SERIALIZER_MISSING:
        return "ERR_SERIALIZER_MISSING";
    case CIOT_ERR_SMALL_RAW:
        return "ERR_SMALL_RAW";
    case CIOT_ERR_INVALID_REQUEST:
        return "CIOT_ERR_INVALID_REQUEST";
    case CIOT_ERR_INVALID_TCP_MBAP:
        return "CIOT_ERR_INVALID_TCP_MBAP";
    case CIOT_ERR_CRC:
        return "CIOT_ERR_CRC";
    case CIOT_ERR_TRANSPORT:
        return "CIOT_ERR_TRANSPORT";
    case CIOT_ERR_INVALID_RESPONSE:
        return "CIOT_ERR_INVALID_RESPONSE";
    case CIOT_ERR_MBUS_EXCEPTION_ILLEGAL_FUNCTION:
        return "CIOT_ERR_MBUS_EXCEPTION_ILLEGAL_FUNCTION";
    case CIOT_ERR_MBUS_EXCEPTION_ILLEGAL_DATA_ADDR:
        return "CIOT_ERR_MBUS_EXCEPTION_ILLEGAL_DATA_ADDR";
    case CIOT_ERR_MBUS_EXCEPTION_ILLEGAL_DATA_VAL:
        return "CIOT_ERR_MBUS_EXCEPTION_ILLEGAL_DATA_VAL";
    case CIOT_ERR_MBUS_EXCEPTION_SERVER_DEVICE_FAILURE:
        return "CIOT_ERR_MBUS_EXCEPTION_SERVER_DEVICE_FAILURE";
    case CIOT_ERR_NULL_EVENT_HANDLER:
        return "CIOT_ERR_NULL_EVENT_HANDLER";
    default:
        return "ERR_UNSPECIFIED";
    }
}
