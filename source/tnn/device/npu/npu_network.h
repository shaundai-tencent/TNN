// Tencent is pleased to support the open source community by making TNN available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#ifndef RAPIDNET_SOURCE_DEVICE_NPU_HIAI_NETWORK_H_
#define RAPIDNET_SOURCE_DEVICE_NPU_HIAI_NETWORK_H_

#include <tnn/core/abstract_device.h>
#include <tnn/core/blob_manager.h>
#include <tnn/device/npu/convert/npu_base_layer_convert.h>
#include <tnn/interpreter/net_resource.h>
#include <tnn/interpreter/net_structure.h>
#include <tnn/layer/base_layer.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "HiAiModelManagerService.h"
#include "graph/attr_value.h"
#include "graph/graph.h"
#include "graph/op/nn_defs.h"
#include "tnn/core/abstract_network.h"

namespace tnn {

class NpuNetwork : public AbstractNetwork {
public:
    // @brief default constructor
    NpuNetwork();

    // @brief virtual default destructor
    virtual ~NpuNetwork();

    // @brief init network with net cfg and net res.
    // @param net_cfg
    // @param net_res
    virtual Status Init(NetworkConfig &net_config, ModelConfig &model_config, AbstractModelInterpreter *interpreter,
                        InputShapesMap inputs_shape);

    // @brief deinit release init create resource
    virtual Status DeInit();

    //  @brief return the amount of memory required for forward
    //  @param memory_size: the memory size used by rapidnet layers for
    //  forward
    //  @return error code: If successful, returns zero. Otherwise, returns
    //  an error code.
    virtual Status GetForwardMemorySize(int &memory_size);

    //  @brief: set memory used by the rapidnet instance without forward
    //  memory, the memory size must be at least that returned by
    //  GetForwardMemorySize(). releasing or otherwise using the memory for
    //  other purposes during the rapidnet network run will result in
    //  undefined behavior.
    //  @param memory: the memory used by rapidnet layers for forward
    //  @return error code: If successful, returns zero. Otherwise, returns
    //  an error code.
    //
    virtual Status SetForwardMemory(void *memory);

    // @brief network infer
    virtual Status Reshape(const InputShapesMap &inputs);

    // @brief get rapidnet command queue
    // @param command_queue device command queue for forward
    virtual Status GetCommandQueue(void **command_queue);

    // @brief network infer, it will sync to wait result
    virtual Status Forward();

    // @brief rapidnet instance network infer, it will not wait
    virtual Status ForwardAsync(Callback call_back);

    // @brief get all input blobs
    // @param blobs input blobs name map
    virtual Status GetAllInputBlobs(BlobMap &blobs);

    // @brief get all output blobs
    // @param blobs output blobs name map
    virtual Status GetAllOutputBlobs(BlobMap &blobs);

    // @brief set device affinity
    virtual Status SetDeviceAffinity(const std::vector<int> &device_list);

private:
    // add for npu
    virtual Status InitLayers(NetResource *net_resource);

    virtual Status CreateGraphInputs(InputShapesMap& input_shape_map);

    virtual Status SetGraphInputsAndOutputs(InputShapesMap& input_shape_map);

    virtual Status BuildModel(std::string &tnn_file_path);

    AbstractDevice *device_ = nullptr;
    Context *context_       = nullptr;

    std::vector<NpuBaseLayer *> layers_;

    BlobManager *blob_manager_ = nullptr;

    NetStructure *net_structure_ = nullptr;

    // add for npu
    std::map<std::string, shared_ptr<OperatorInfo>> global_operator_map_;
    ge::Graph graph_ = ge::Graph("graph");

    std::string model_name_;
    std::shared_ptr<hiai::AiModelMngerClient> client_;
    std::vector<std::shared_ptr<hiai::AiTensor>> input_tensor_;
    std::vector<std::shared_ptr<hiai::AiTensor>> output_tensor_;
    BlobMap input_blob_map_;
    BlobMap output_blob_map_;

    Status IRInitLayers(NetworkConfig &net_config, AbstractModelInterpreter *interpreter, InputShapesMap &inputs_shape);
};

}  // namespace tnn

#endif  // RAPIDNET_SOURCE_DEVICE_NPU_HIAI_NETWORK_H_