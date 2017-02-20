// This file contains constants, enums and structures that are used in different components.
#pragma once
#include "primitives/timestamp.h"
#include "primitives/vector.h"
#include <stdint.h>

// Tunable
constexpr int max_num_inputs = 8;            // Number of concurrent sensors we support.
constexpr int max_bytes_in_data_frame = 64;  // Current DataFrame length is 33. This param should be larger.
constexpr int max_bytes_in_data_chunk = 64;

// Not tunable: constant for Lighthouse system.
constexpr int num_base_stations = 2;
constexpr int num_cycle_phases = 4;

// Pulses are generated by Inputs and processed by PulseProcessor
struct Pulse {
    uint32_t input_idx;
    Timestamp start_time;
    TimeDelta pulse_len;
};

enum class FixLevel {
    kNoSignals      =    0,  // No signals visible at all.
    kCycleSyncing   =  100,  // Base station sync pulses are visible and we're syncing to them.
    kCycleSynced    =  200,  // We're synced to the base station sync pulses.
    kPartialVis     =  500,  // Some sensors/base stations are covered. Not enough info to get position.
    kFullFix        = 1000,  // Base station visibility is enough to extract full position.
};

struct SensorAngles {
    float angles[num_cycle_phases]; // Angles of base stations to sensor, -1/3 Pi to 1/3 Pi
    uint32_t updated_cycles[num_cycle_phases]; // Cycle id when this angle was last updated.
};

struct SensorAnglesFrame {
    Timestamp time;
    uint32_t cycle_idx;
    int32_t phase_id;
    Vector<SensorAngles, max_num_inputs> sensors;
};

struct DataFrameBit {
    Timestamp time;
    uint32_t base_station_idx;
    uint32_t cycle_idx;
    bool bit;
};

struct DataFrame {
    Timestamp time;
    uint32_t base_station_idx;
    Vector<uint8_t, max_bytes_in_data_frame> bytes;
};

// Position of an object.
struct ObjectPosition {
    Timestamp time;
    uint32_t object_idx; // Index of the object.
    FixLevel fix_level;
    float pos[3];     // 3d object position
    float pos_delta;  // Distance between base station rays. Can be used as a measure of position uncertainty.
    float q[4];       // Rotation quaternion (unit if no rotation information available)
};

// DataChunk is used to send raw data to outputs.
struct DataChunk {
    Timestamp time;
    Vector<uint8_t, max_bytes_in_data_chunk> data;  // Data of this chunk.
    uint32_t stream_idx;  // Used to distinguish between different streams going to the same output. Useful for polling mode.
    bool last_chunk;  // True if this is the last chunk in a "packet". Useful for polling mode.
};


enum class OutputCommandType {
    kMakeExclusive,  // Make given stream_idx exclusive and don't accept data chunks from other streams.
    kMakeNonExclusive,  // Remove exclusivity.
};

struct OutputCommand {
    OutputCommandType type;
    uint32_t stream_idx;
};