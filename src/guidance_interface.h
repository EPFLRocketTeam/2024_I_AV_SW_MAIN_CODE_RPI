// #ifndef GUIDANCE_INTERFACE_H
// #define GUIDANCE_INTERFACE_H

// #include <cactus_rt/rt.h>
// #include "lv_guidance.h"
// #include "model.h"
// #include "shared_memory.h"

// using cactus_rt::CyclicThread;

// class GuidanceThread : public CyclicThread
// {
// public:
//     GuidanceThread()
//         : CyclicThread("GuidanceThread", MakeConfig()), 
//             rocket(&result, new ModelPointMass(), true) {
//             current_state = {0, 0, 0, 0, 0, 0, 0, 0, 784.8};
//             target_state = {0, 0, 2, 0, 0, 0, 0, 0, 784.8};
//             num_vectors_to_compute = 2;
//             SharedMemory<Trajectory>* Trajectory_memory;

//     }
//      // Setter for current_state
//     void set_current_state(const Vector& state) {
//         current_state = state;
//     }

//     //  Setter for target_state
//     void set_target_state(const Vector& state) {
//         target_state = state;
//     }

//     // Setter for num_vectors_to_compute
//     void set_num_vectors_to_compute(int num_vectors) {
//         if (num_vectors > 0) {  // Ensure the value is valid
//             num_vectors_to_compute = num_vectors;
//         } else {
//             throw std::invalid_argument("Number of vectors must be greater than 0.");
//         }
//     }

// protected:
//     LoopControl Loop(int64_t elapsed_ns) noexcept final;

// private:
//     static cactus_rt::CyclicThreadConfig MakeConfig();
//     LVGuidance rocket;           // LVGuidance instance
//     Trajectory result;             // Trajectory to store results
//     Vector current_state;          // Current state vector
//     Vector target_state;           // Target state vector
//     int num_vectors_to_compute;  // Number of vectors to compute in the trajectory

// };

// #endif // GUIDANCE_INTERFACE_H