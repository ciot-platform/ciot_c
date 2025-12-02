/**
 * @file ciot_mbus_server_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_MBUS_SERVER == 1

#include "ciot_mbus_server.h"
#include "ciot_uart.h"
#include "ciot_types.h"

static ciot_err_t ciot_mbus_server_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_mbus_server_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_mbus_server_send_data(ciot_iface_t *iface, uint8_t *data, int size);
static ciot_err_t ciot_mbus_server_read_data(ciot_iface_t *iface, uint8_t *data, int size);
static ciot_err_t ciot_mbus_server_process_func(ciot_mbus_server_t self, ciot_mbus_function_req_t *func);

ciot_err_t ciot_mbus_server_init(ciot_mbus_server_t self)
{
    ciot_mbus_server_base_t *base = (ciot_mbus_server_base_t *)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_mbus_server_process_data;
    base->iface.get_data = ciot_mbus_server_get_data;
    base->iface.send_data = ciot_mbus_server_send_data;
    base->iface.read_data = ciot_mbus_server_read_data;
    base->iface.info.type = CIOT_IFACE_TYPE_MBUS_SERVER;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mbus_server_process_req(ciot_mbus_server_t self, ciot_mbus_server_req_t *req)
{
    ciot_mbus_server_base_t *base = (ciot_mbus_server_base_t *)self;
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    switch (req->which_type)
    {
    case CIOT_MBUS_SERVER_REQ_FUNCTION_TAG:
        base->iface.req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
        return ciot_mbus_server_process_func(self, &req->function);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}

static ciot_err_t ciot_mbus_server_process_func(ciot_mbus_server_t self, ciot_mbus_function_req_t *func)
{
    switch (func->code)
    {
    case CIOT_MBUS_FUNC_CODE_NONE:
    case CIOT_MBUS_FUNC_CODE_READ_COILS:
    case CIOT_MBUS_FUNC_CODE_READ_DI:
    case CIOT_MBUS_FUNC_CODE_READ_HR:
        for (size_t i = func->address; i < func->read_count; i++)
        {
            CIOT_ERR_RETURN(ciot_mbus_server_get_reg(self, func->address + i, &func->data[i], 2));
            func->data_count = i + 1;
        }
        return CIOT_ERR_OK;
    case CIOT_MBUS_FUNC_CODE_READ_IR:
    case CIOT_MBUS_FUNC_CODE_WRITE_SINGLE_COIL:
    case CIOT_MBUS_FUNC_CODE_WRITE_SINGLE_HR:
    case CIOT_MBUS_FUNC_CODE_WRITE_MULTIPLE_COILS:
        return CIOT_ERR_NOT_IMPLEMENTED;
    case CIOT_MBUS_FUNC_CODE_WRITE_MULTIPLE_HR:
        for (size_t i = func->address; i < func->data_count; i++)
        {
            CIOT_ERR_RETURN(ciot_mbus_server_set_reg(self, func->address + i, &func->data[i], 2));
        }
        return CIOT_ERR_OK;
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}

static ciot_err_t ciot_mbus_server_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_MBUS_SERVER_TAG);

    ciot_mbus_server_t self = iface->ptr;
    ciot_mbus_server_data_t *mbus_server = &data->mbus_server;

    switch (mbus_server->which_type)
    {
    case CIOT_MBUS_SERVER_DATA_STOP_TAG:
        return ciot_mbus_server_stop(self);
    case CIOT_MBUS_SERVER_DATA_CONFIG_TAG:
        return ciot_mbus_server_start(self, &mbus_server->config);
    case CIOT_MBUS_SERVER_DATA_REQUEST_TAG:
        return ciot_mbus_server_process_req(self, &mbus_server->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_mbus_server_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_mbus_server_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_MBUS_SERVER_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->mbus_server.which_type = CIOT_MBUS_SERVER_DATA_CONFIG_TAG;
        data->mbus_server.config = self->cfg;
        if(self->cfg.which_type == CIOT_MBUS_SERVER_CFG_RTU_TAG) {
            ciot_uart_base_t *uart = (ciot_uart_base_t*)self->conn;
            data->mbus_server.config.rtu.uart = uart->cfg;
            data->mbus_server.config.rtu.has_uart = true;
        }
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->mbus_server.which_type = CIOT_MBUS_SERVER_DATA_STATUS_TAG;
        data->mbus_server.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mbus_server_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_mbus_server_read_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif // CIOT_CONFIG_FEATURE_MBUS_SERVER == 1