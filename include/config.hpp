#pragma once

#include "config-utils/shared/config-utils.hpp"

#include "UnityEngine/Vector3.hpp"

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(enabled, bool, "Enabled", true);
    CONFIG_VALUE(size, double, "Size", 1.0);
    CONFIG_VALUE(offset, UnityEngine::Vector3, "Offset", UnityEngine::Vector3(0, 0, 0));

    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(enabled);
        CONFIG_INIT_VALUE(size);
        CONFIG_INIT_VALUE(offset);
    )
)