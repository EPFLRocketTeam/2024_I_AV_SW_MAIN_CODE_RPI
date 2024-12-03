#include "Client.h"
#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <vector>

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}

bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const &encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }

    return ret;
}

// ThreeFloatModule.cpp
ThreeFloatModule::ThreeFloatModule() : value1(0.0f), value2(0.0f), value3(0.0f) {}

void ThreeFloatModule::set_values(float v1, float v2, float v3)
{
    value1 = v1;
    value2 = v2;
    value3 = v3;
}

void ThreeFloatModule::generate_message(char *buffer)
{
    std::memcpy(&buffer[0], &value1, sizeof(float));
    std::memcpy(&buffer[4], &value2, sizeof(float));
    std::memcpy(&buffer[8], &value3, sizeof(float));
}

void ThreeFloatModule::unpack_message(const char *buffer)
{
    std::memcpy(&value1, &buffer[0], sizeof(float));
    std::memcpy(&value2, &buffer[4], sizeof(float));
    std::memcpy(&value3, &buffer[8], sizeof(float));
}

// OneFloatModule.cpp
OneFloatModule::OneFloatModule() : value(0.0f) {}

void OneFloatModule::set_value(float v)
{
    value = v;
}

void OneFloatModule::generate_message(char *buffer)
{
    std::memcpy(&buffer[0], &value, sizeof(float));
}

void OneFloatModule::unpack_message(const char *buffer)
{
    std::memcpy(&value, &buffer[0], sizeof(float));
}

// Manager.cpp
void Manager::set_module_configuration(char identifier, const std::vector<Module *> &modules)
{
    module_configurations[identifier] = modules;
}

void Manager::generate_combined_message(char identifier, char *combined_buffer, std::string &encoded_message)
{
    combined_buffer[0] = identifier;
    int offset = 1;
    for (auto &module : module_configurations[identifier])
    {
        char module_buffer[12];
        module->generate_message(module_buffer);
        int module_size = (module->get_type() == Module::Type::ThreeFloat) ? 12 : 4;
        std::memcpy(&combined_buffer[offset], module_buffer, module_size);
        offset += module_size;
    }
    encoded_message = base64_encode(reinterpret_cast<const unsigned char *>(combined_buffer), offset);
}

void Manager::unpack_combined_message(const std::string &encoded_buffer, char &identifier, std::vector<std::vector<float>> &values)
{
    std::string decoded_buffer = base64_decode(encoded_buffer);
    const char *combined_buffer = decoded_buffer.c_str();
    identifier = combined_buffer[0];
    int offset = 1;
    for (auto &module : module_configurations[identifier])
    {
        std::vector<float> module_values;
        if (module->get_type() == Module::Type::ThreeFloat)
        {
            float value1, value2, value3;
            module->unpack_message(&combined_buffer[offset]);
            std::memcpy(&value1, &combined_buffer[offset], sizeof(float));
            std::memcpy(&value2, &combined_buffer[offset + 4], sizeof(float));
            std::memcpy(&value3, &combined_buffer[offset + 8], sizeof(float));
            module_values.push_back(value1);
            module_values.push_back(value2);
            module_values.push_back(value3);
            offset += 12;
        }
        else if (module->get_type() == Module::Type::OneFloat)
        {
            float value;
            module->unpack_message(&combined_buffer[offset]);
            std::memcpy(&value, &combined_buffer[offset], sizeof(float));
            module_values.push_back(value);
            offset += 4;
        }
        values.push_back(module_values);
    }
}