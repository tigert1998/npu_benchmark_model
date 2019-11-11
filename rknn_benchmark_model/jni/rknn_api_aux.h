#ifndef RKNN_API_AUX_
#define RKNN_API_AUX_

#include <iostream>

#include "rknn_api.h"

std::ostream &operator<<(std::ostream &os, const rknn_tensor_format &);

std::ostream &operator<<(std::ostream &os, const rknn_tensor_type &);

std::ostream &operator<<(std::ostream &os, const rknn_tensor_qnt_type &);

std::ostream &operator<<(std::ostream &os, const rknn_tensor_attr &);

#endif