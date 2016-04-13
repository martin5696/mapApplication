#pragma once

#include "m4.h"

namespace ece297test {

bool courier_path_is_legal(const std::vector<DeliveryInfo>& deliveries,
                          const std::vector<unsigned>& depots,
                          const std::vector<unsigned>& path);

bool valid_courier_problem(const std::vector<DeliveryInfo>& deliveries,
                           const std::vector<unsigned>& depots);

}
