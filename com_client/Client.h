#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <tuple>
#include <string>
#include <cstring>
#include <unordered_map>

class Module
{
public:
    enum class Type
    {
        ThreeFloat,
        OneFloat
    };
    virtual void generate_message(char *buffer) = 0;
    virtual void unpack_message(const char *buffer) = 0;
    virtual Type get_type() const = 0;
    virtual ~Module() = default;
};

class ThreeFloatModule : public Module
{
public:
    ThreeFloatModule();
    void set_values(float value1, float value2, float value3);
    void generate_message(char *buffer) override;
    void unpack_message(const char *buffer) override;
    Type get_type() const override { return Type::ThreeFloat; }

private:
    float value1, value2, value3;
};

class OneFloatModule : public Module
{
public:
    OneFloatModule();
    void set_value(float value);
    void generate_message(char *buffer) override;
    void unpack_message(const char *buffer) override;
    Type get_type() const override { return Type::OneFloat; }

private:
    float value;
};

class Manager
{
public:
    void set_module_configuration(char identifier, const std::vector<Module *> &modules);
    void generate_combined_message(char identifier, char *combined_buffer, std::string &encoded_message);
    void unpack_combined_message(const std::string &encoded_buffer, char &identifier, std::vector<std::vector<float>> &values);

private:
    std::unordered_map<char, std::vector<Module *>> module_configurations;
};

#endif // CLIENT_H