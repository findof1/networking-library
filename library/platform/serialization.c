#include "serialization.h"

int sendInt(socket_t socket, int value)
{
  uint8_t type = TYPE_INT;
  uint32_t size = htonl(sizeof(value));
  uint32_t number = htonl(value);
  if (sendData(socket, &type, sizeof(uint8_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendData(socket, &size, sizeof(uint32_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendData(socket, &number, sizeof(uint32_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  return PLATFORM_SUCCESS;
}

int recvInt(socket_t socket, int *out)
{
  uint8_t type;
  uint32_t size, netValue;

  int result = recvData(socket, &type, 1, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (type != TYPE_INT)
  {
    return PLATFORM_FAILURE;
  }

  result = recvData(socket, &size, 4, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (ntohl(size) != sizeof(uint32_t))
  {
    return PLATFORM_FAILURE;
  }

  result = recvData(socket, &netValue, 4, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  *out = ntohl(netValue);
  return PLATFORM_SUCCESS;
}

int sendFloat(socket_t socket, float value)
{
  uint8_t type = TYPE_FLOAT;
  uint32_t size = htonl(sizeof(value));

  uint32_t number;
  memcpy(&number, &value, sizeof(float));
  number = htonl(number);

  if (sendData(socket, &type, sizeof(uint8_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendData(socket, &size, sizeof(uint32_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendData(socket, &number, sizeof(uint32_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  return PLATFORM_SUCCESS;
}

int recvFloat(socket_t socket, float *out)
{
  uint8_t type;
  uint32_t size, netValue;

  int result = recvData(socket, &type, 1, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (type != TYPE_FLOAT)
  {
    return PLATFORM_FAILURE;
  }

  result = recvData(socket, &size, 4, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (ntohl(size) != sizeof(float))
  {
    return PLATFORM_FAILURE;
  }

  result = recvData(socket, &netValue, 4, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  netValue = ntohl(netValue);
  float number;
  memcpy(out, &netValue, sizeof(uint32_t));

  return PLATFORM_SUCCESS;
}

int sendString(socket_t socket, const char *str)
{
  uint8_t type = TYPE_STRING;
  uint32_t length = strlen(str);
  uint32_t size = htonl(length);

  if (sendData(socket, &type, sizeof(uint8_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendData(socket, &size, sizeof(uint32_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendData(socket, str, length, 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }

  return PLATFORM_SUCCESS;
}

int recvString(socket_t socket, char **out)
{
  uint8_t type;
  uint32_t size;
  char *str;

  int result = recvData(socket, &type, 1, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (type != TYPE_STRING)
  {
    return PLATFORM_FAILURE;
  }

  result = recvData(socket, &size, 4, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  size = ntohl(size);

  char *buf = (char *)malloc(size + 1);
  if (buf == NULL)
  {
    return PLATFORM_FAILURE;
  }

  result = recvAll(socket, buf, size, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    free(buf);
    return result;
  }

  (buf)[size] = '\0';
  *out = buf;

  return PLATFORM_SUCCESS;
}

int sendJSON(socket_t socket, const cJSON *json)
{
  uint8_t type = TYPE_JSON;
  char *str = cJSON_PrintUnformatted(json);
  uint32_t length = strlen(str);
  uint32_t size = htonl(length);
  if (sendData(socket, &type, sizeof(uint8_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendData(socket, &size, sizeof(uint32_t), 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendData(socket, str, length, 0) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  return PLATFORM_SUCCESS;
}

int recvJSON(socket_t socket, cJSON **json)
{
  uint8_t type;
  uint32_t size;
  char *str;

  int result = recvData(socket, &type, 1, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (type != TYPE_JSON)
  {
    return PLATFORM_FAILURE;
  }

  result = recvData(socket, &size, 4, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }
  size = ntohl(size);

  char *buf = (char *)malloc(size + 1);
  if (buf == NULL)
  {
    return PLATFORM_FAILURE;
  }

  result = recvAll(socket, buf, size, 0);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    free(buf);
    return result;
  }

  (buf)[size] = '\0';
  *json = cJSON_Parse(buf);
  free(buf);

  if (*json == NULL)
  {
    return PLATFORM_FAILURE;
  }

  return PLATFORM_SUCCESS;
}

int recvAny(socket_t socket, Data *data)
{
  uint8_t rawType;
  int result = recvData(socket, &rawType, 1, 0);
  data->type = (NetworkedType)rawType;
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (data->type == TYPE_INT)
  {
    uint32_t size, netValue;

    result = recvData(socket, &size, 4, 0);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }

    if (ntohl(size) != sizeof(uint32_t))
    {
      return PLATFORM_FAILURE;
    }

    result = recvData(socket, &netValue, 4, 0);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }

    data->data.i = ntohl(netValue);
    return PLATFORM_SUCCESS;
  }

  if (data->type == TYPE_FLOAT)
  {
    uint32_t size, netValue;

    result = recvData(socket, &size, 4, 0);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }

    if (ntohl(size) != sizeof(float))
    {
      return PLATFORM_FAILURE;
    }

    result = recvData(socket, &netValue, 4, 0);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }
    netValue = ntohl(netValue);
    memcpy(&data->data.f, &netValue, sizeof(uint32_t));
    return PLATFORM_SUCCESS;
  }

  if (data->type == TYPE_STRING)
  {
    uint32_t size;

    result = recvData(socket, &size, 4, 0);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }
    size = ntohl(size);

    char *buf = (char *)malloc(size + 1);
    if (buf == NULL)
    {
      return PLATFORM_FAILURE;
    }

    result = recvAll(socket, buf, size, 0);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      free(buf);
      return result;
    }

    (buf)[size] = '\0';
    data->data.s = buf;
    return PLATFORM_SUCCESS;
  }

  if (data->type == TYPE_JSON)
  {
    uint32_t size;

    result = recvData(socket, &size, 4, 0);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }
    size = ntohl(size);

    char *buf = (char *)malloc(size + 1);
    if (buf == NULL)
    {
      return PLATFORM_FAILURE;
    }

    result = recvAll(socket, buf, size, 0);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      free(buf);
      return result;
    }

    (buf)[size] = '\0';
    data->data.json = cJSON_Parse(buf);
    free(buf);

    if (data->data.json == NULL)
    {
      return PLATFORM_FAILURE;
    }
    return PLATFORM_SUCCESS;
  }

  return PLATFORM_FAILURE;
}

int sendIntTo(socket_t socket, struct sockaddr_in *peerAddr, int value)
{
  uint8_t type = TYPE_INT;
  uint32_t size = htonl(sizeof(value));
  uint32_t number = htonl(value);
  if (sendDataTo(socket, &type, sizeof(uint8_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendDataTo(socket, &size, sizeof(uint32_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendDataTo(socket, &number, sizeof(uint32_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  return PLATFORM_SUCCESS;
}

int recvIntFrom(socket_t socket, struct sockaddr_in *peerAddr, int *out)
{
  uint8_t type;
  uint32_t size, netValue;

  int result = recvDataFrom(socket, &type, 1, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (type != TYPE_INT)
  {
    return PLATFORM_FAILURE;
  }

  result = recvDataFrom(socket, &size, 4, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (ntohl(size) != sizeof(uint32_t))
  {
    return PLATFORM_FAILURE;
  }

  result = recvDataFrom(socket, &netValue, 4, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  *out = ntohl(netValue);
  return PLATFORM_SUCCESS;
}

int sendFloatTo(socket_t socket, struct sockaddr_in *peerAddr, float value)
{
  uint8_t type = TYPE_FLOAT;
  uint32_t size = htonl(sizeof(value));

  uint32_t number;
  memcpy(&number, &value, sizeof(float));
  number = htonl(number);

  if (sendDataTo(socket, &type, sizeof(uint8_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendDataTo(socket, &size, sizeof(uint32_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendDataTo(socket, &number, sizeof(uint32_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  return PLATFORM_SUCCESS;
}

int recvFloatFrom(socket_t socket, struct sockaddr_in *peerAddr, float *out)
{
  uint8_t type;
  uint32_t size, netValue;

  int result = recvDataFrom(socket, &type, 1, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (type != TYPE_FLOAT)
  {
    return PLATFORM_FAILURE;
  }

  result = recvDataFrom(socket, &size, 4, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (ntohl(size) != sizeof(float))
  {
    return PLATFORM_FAILURE;
  }

  result = recvDataFrom(socket, &netValue, 4, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  netValue = ntohl(netValue);
  float number;
  memcpy(out, &netValue, sizeof(uint32_t));

  return PLATFORM_SUCCESS;
}

int sendStringTo(socket_t socket, struct sockaddr_in *peerAddr, const char *str)
{
  uint8_t type = TYPE_STRING;
  uint32_t length = strlen(str);
  uint32_t size = htonl(length);

  if (sendDataTo(socket, &type, sizeof(uint8_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendDataTo(socket, &size, sizeof(uint32_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendDataTo(socket, str, length, 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }

  return PLATFORM_SUCCESS;
}
int recvStringFrom(socket_t socket, struct sockaddr_in *peerAddr, char **out)
{
  uint8_t type;
  uint32_t size;
  char *str;

  int result = recvDataFrom(socket, &type, 1, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (type != TYPE_STRING)
  {
    return PLATFORM_FAILURE;
  }

  result = recvDataFrom(socket, &size, 4, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  size = ntohl(size);

  char *buf = (char *)malloc(size + 1);
  if (buf == NULL)
  {
    return PLATFORM_FAILURE;
  }

  result = recvDataFrom(socket, buf, size, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    free(buf);
    return result;
  }

  (buf)[size] = '\0';
  *out = buf;

  return PLATFORM_SUCCESS;
}

int sendJSONTo(socket_t socket, struct sockaddr_in *peerAddr, const cJSON *json)
{
  uint8_t type = TYPE_JSON;
  char *str = cJSON_PrintUnformatted(json);
  uint32_t length = strlen(str);
  uint32_t size = htonl(length);
  if (sendDataTo(socket, &type, sizeof(uint8_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendDataTo(socket, &size, sizeof(uint32_t), 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  if (sendDataTo(socket, str, length, 0, peerAddr) == PLATFORM_FAILURE)
  {
    return PLATFORM_FAILURE;
  }
  return PLATFORM_SUCCESS;
}

int recvJSONFrom(socket_t socket, struct sockaddr_in *peerAddr, cJSON **json)
{
  uint8_t type;
  uint32_t size;
  char *str;

  int result = recvDataFrom(socket, &type, 1, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (type != TYPE_JSON)
  {
    return PLATFORM_FAILURE;
  }

  result = recvDataFrom(socket, &size, 4, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }
  size = ntohl(size);

  char *buf = (char *)malloc(size + 1);
  if (buf == NULL)
  {
    return PLATFORM_FAILURE;
  }

  result = recvDataFrom(socket, buf, size, 0, peerAddr);
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    free(buf);
    return result;
  }

  (buf)[size] = '\0';
  *json = cJSON_Parse(buf);
  free(buf);

  if (*json == NULL)
  {
    return PLATFORM_FAILURE;
  }

  return PLATFORM_SUCCESS;
}

int recvAnyFrom(socket_t socket, struct sockaddr_in *peerAddr, Data *data)
{
  uint8_t rawType;
  int result = recvDataFrom(socket, &rawType, 1, 0, peerAddr);
  data->type = (NetworkedType)rawType;
  if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
  {
    return result;
  }

  if (data->type == TYPE_INT)
  {
    uint32_t size, netValue;

    result = recvDataFrom(socket, &size, 4, 0, peerAddr);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }

    if (ntohl(size) != sizeof(uint32_t))
    {
      return PLATFORM_FAILURE;
    }

    result = recvDataFrom(socket, &netValue, 4, 0, peerAddr);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }

    data->data.i = ntohl(netValue);
    return PLATFORM_SUCCESS;
  }

  if (data->type == TYPE_FLOAT)
  {
    uint32_t size, netValue;

    result = recvDataFrom(socket, &size, 4, 0, peerAddr);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }

    if (ntohl(size) != sizeof(float))
    {
      return PLATFORM_FAILURE;
    }

    result = recvDataFrom(socket, &netValue, 4, 0, peerAddr);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }
    netValue = ntohl(netValue);
    memcpy(&data->data.f, &netValue, sizeof(uint32_t));
    return PLATFORM_SUCCESS;
  }

  if (data->type == TYPE_STRING)
  {
    uint32_t size;

    result = recvDataFrom(socket, &size, 4, 0, peerAddr);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }
    size = ntohl(size);

    char *buf = (char *)malloc(size + 1);
    if (buf == NULL)
    {
      return PLATFORM_FAILURE;
    }

    result = recvDataFrom(socket, buf, size, 0, peerAddr);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      free(buf);
      return result;
    }

    (buf)[size] = '\0';
    data->data.s = buf;
    return PLATFORM_SUCCESS;
  }

  if (data->type == TYPE_JSON)
  {
    uint32_t size;

    result = recvDataFrom(socket, &size, 4, 0, peerAddr);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      return result;
    }
    size = ntohl(size);

    char *buf = (char *)malloc(size + 1);
    if (buf == NULL)
    {
      return PLATFORM_FAILURE;
    }

    result = recvDataFrom(socket, buf, size, 0, peerAddr);
    if (result == PLATFORM_CONNECTION_CLOSED || result == PLATFORM_FAILURE)
    {
      free(buf);
      return result;
    }

    (buf)[size] = '\0';
    data->data.json = cJSON_Parse(buf);
    free(buf);

    if (data->data.json == NULL)
    {
      return PLATFORM_FAILURE;
    }
    return PLATFORM_SUCCESS;
  }

  return PLATFORM_FAILURE;
}

void freeRecvData(Data *data)
{
  switch (data->type)
  {
  case TYPE_STRING:
    free(data->data.s);
    break;
  case TYPE_JSON:
    cJSON_Delete(data->data.json);
    break;
  default:
    break;
  }
}
