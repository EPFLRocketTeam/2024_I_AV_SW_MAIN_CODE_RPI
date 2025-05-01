#ifndef CONTROL_DRIVER_H
#define CONTROL_DRIVER_H

#include "DroneController.h"
#include "VehicleOutputs.h"
#include "VehicleInputs.h"
#include <json.hpp>

using json = nlohmann::json;

class ControlDriver
{
  public:
    ControlDriver();
    virtual ~ControlDriver() = default;

    // Function to run the control algorithm
    VehicleOutputs RunControl(const VehicleInputs &input);

  protected:
    Controller ControllerFromFile(const std::string &file_path);
    Controller ControllerFromJSON(const json &doc);
    std::unique_ptr<Controller> controller;
};

#endif // CONTROL_DRIVER_H