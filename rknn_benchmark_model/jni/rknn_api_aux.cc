#include "rknn_api_aux.h"

#include "util.h"

#define CASE(enum_item) \
  case enum_item: {     \
    os << #enum_item;   \
    break;              \
  }

std::ostream &operator<<(std::ostream &os, const rknn_tensor_format &data) {
  switch (data) {
    CASE(RKNN_TENSOR_NCHW)
    CASE(RKNN_TENSOR_NHWC)
    CASE(RKNN_TENSOR_FORMAT_MAX)
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const rknn_tensor_type &data) {
  switch (data) {
    CASE(RKNN_TENSOR_FLOAT32)
    CASE(RKNN_TENSOR_FLOAT16)
    CASE(RKNN_TENSOR_INT8)
    CASE(RKNN_TENSOR_UINT8)
    CASE(RKNN_TENSOR_INT16)
    CASE(RKNN_TENSOR_TYPE_MAX)
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const rknn_tensor_qnt_type &data) {
  switch (data) {
    CASE(RKNN_TENSOR_QNT_NONE)
    CASE(RKNN_TENSOR_QNT_DFP)
    CASE(RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC)
    CASE(RKNN_TENSOR_QNT_MAX)
  }
  return os;
}

#define OS_DATA(member) os << "\t." #member " = " << data.member << ",\n"

#define OS_DATA_VALUE(member, value) os << "\t." #member " = " << value << ",\n"

std::ostream &operator<<(std::ostream &os, const rknn_tensor_attr &data) {
  os << "{\n";
  OS_DATA(index);
  OS_DATA(n_dims);
  OS_DATA_VALUE(dims,
                std::vector<uint32_t>(data.dims, data.dims + data.n_dims));
  OS_DATA(name);

  OS_DATA(n_elems);
  OS_DATA(size);

  OS_DATA(fmt);
  OS_DATA(type);
  OS_DATA(qnt_type);
  OS_DATA_VALUE(fl, static_cast<int32_t>(data.fl));
  OS_DATA(zp);
  OS_DATA(scale);
  os << "}";
  return os;
}