//
// Created by root on 24-8-24.
//

#include "api_networks.h"
#include "cJSON.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <arpa/inet.h>
#include <ctype.h>

struct ip_trans_st {
    char trans_op;
    char *ori_str;
    char trans_result[INET_ADDRSTRLEN];
};

#define IP_TRANS_OP_ADDR_STR (1)
#define IP_TRANS_OP_ADDR_INT (2)
#define IP_TRANS_OP_ADDR_HEX (3)

static int atk_network_ip_trans_op(struct ip_trans_st *trans_node)
{
    unsigned int index = 0;
    if(NULL == trans_node || NULL == trans_node->ori_str) {
        return -1;
    }

    if (strncmp(trans_node->ori_str, "0x", 2) == 0 || strncmp(trans_node->ori_str, "0X", 2) == 0) {
        for (index = 2; index < strlen(trans_node->ori_str); index++) {
            if (!isxdigit(trans_node->ori_str[index])) {
                trans_node->trans_op = IP_TRANS_OP_ADDR_STR;
                return 0;
            }
        }
        trans_node->trans_op = IP_TRANS_OP_ADDR_HEX;
        return 0;
    } else {
        for (index = 0; index < strlen(trans_node->ori_str); index++) {
            if (!isdigit(trans_node->ori_str[index])) {
                trans_node->trans_op = IP_TRANS_OP_ADDR_STR;
                return 0;
            }
        }
        trans_node->trans_op = IP_TRANS_OP_ADDR_INT;
        return 0;
    }

    return -1;
}

static struct ip_trans_st * atk_network_ip_trans_deal(char *value)
{
    struct in_addr addr;
    struct ip_trans_st *trans_node = NULL;
    trans_node = calloc(1, sizeof(struct ip_trans_st));
    if(NULL == trans_node) {
        printf("内存分配失败");
        exit(-1);
    }

    unsigned int str_len = strlen(value);
    trans_node->ori_str = calloc(1, str_len + 1);
    strncpy(trans_node->ori_str, value, str_len);
    trans_node->ori_str[str_len] = '\0';
    atk_network_ip_trans_op(trans_node);

    if(trans_node->trans_op == IP_TRANS_OP_ADDR_INT
    || trans_node->trans_op == IP_TRANS_OP_ADDR_HEX) {
        unsigned long ip_num = 0;
        // 处理整型字符串表示的 IP 地址
        if(trans_node->trans_op == IP_TRANS_OP_ADDR_HEX)
            ip_num = strtoul(trans_node->ori_str, NULL, 16);
        else
            ip_num = strtoul(trans_node->ori_str, NULL, 10);

        // 将整型转换为 in_addr 结构
        addr.s_addr = htonl(ip_num);

        // 将 in_addr 结构转换为点分十进制字符串
        if (inet_ntop(AF_INET, &addr, trans_node->trans_result, INET_ADDRSTRLEN) != NULL) {
            printf("Converted IP: %s\n", trans_node->trans_result);
        } else {
            perror("inet_ntop failed");
        }
    } else {
        // 处理点分十进制字符串
        if (inet_pton(AF_INET, trans_node->ori_str, &addr) == 1) {
            printf("Input is already in dotted decimal format: %s\n", trans_node->ori_str);
            snprintf(trans_node->trans_result, sizeof(trans_node->trans_result), "%s", trans_node->ori_str);
        } else {
            printf("IP address format error\n");
            snprintf(trans_node->trans_result, sizeof(trans_node->trans_result), "Unknown Format");
        }
    }
    return trans_node;
}

void atk_api_network_ip_trans(struct evhttp_request *req, void *arg) {
    // 检查请求方法是否为 POST
    if (evhttp_request_get_command(req) != EVHTTP_REQ_POST) {
        evhttp_send_reply(req, HTTP_BADMETHOD, "Method Not Allowed", NULL);
        return;
    }

    // 获取请求体
    struct evbuffer *in_buf = evhttp_request_get_input_buffer(req);
    size_t data_len = evbuffer_get_length(in_buf);
    char *data = malloc(data_len + 1);
    evbuffer_copyout(in_buf, data, data_len);
    data[data_len] = '\0'; // 确保数据以空字符结尾

    // 解析 JSON 数据
    cJSON *json = cJSON_Parse(data);
    if (!json) {
        free(data);
        evhttp_send_reply(req, HTTP_BADREQUEST, "Bad Request", NULL);
        return;
    }

    // 处理业务逻辑，例如读取某个字段
    cJSON *name = cJSON_GetObjectItem(json, "trans_ori");
    if (name && cJSON_IsString(name)) {
        printf("Received trans_ori: %s\n", name->valuestring);
    }

    struct ip_trans_st *node = NULL;
    node = atk_network_ip_trans_deal(name->valuestring);
    cJSON_Delete(json);
    free(data);

    if(NULL == node) {
        evhttp_send_reply(req, HTTP_SERVUNAVAIL, "Service Unavail", NULL);
        return;
    }

    // 生成响应
    struct evbuffer *out_buf = evbuffer_new();
    cJSON *ret = cJSON_CreateObject();
    cJSON_AddItemToObject(ret, "trans_ori", cJSON_CreateString(node->ori_str));
    cJSON_AddItemToObject(ret, "trans_ret", cJSON_CreateString(node->trans_result));
    char *result = cJSON_PrintUnformatted(ret);
    evbuffer_add_printf(out_buf, "%s", result);
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "application/json");
    evhttp_send_reply(req, 201, "OK", out_buf);

    evbuffer_free(out_buf);
    free(node->ori_str);
    free(node);
    cJSON_free(result);
}
