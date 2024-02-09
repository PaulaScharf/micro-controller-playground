/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <TensorFlowLite.h>

#include "main_functions.h"

#include "vl53l8cx_handler.h"
#include "constants.h"
#include "maneuver_predictor.h"
#include "tof_detector_model_data.h"
#include "output_handler.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
// #include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
// #include "tensorflow/lite/version.h"

// #include "Freenove_WS2812_Lib_for_ESP32.h"
// #define LED_PIN 1
// Freenove_ESP32_WS2812 led = Freenove_ESP32_WS2812(1, LED_PIN, 0, TYPE_GRB);
// void setLED(uint8_t r,uint8_t g,uint8_t b) {
//   led.setLedColorData(0, r, g, b);
//   led.show();
// }

// Globals, used for compatibility with Arduino-style sketches.
namespace {
// tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;
int input_length;

// Create an area of memory to use for input, output, and intermediate arrays.
// The size of this will depend on the model you're using, and may need to be
// determined by experimentation.
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
void setup() {
  Serial.begin(115200);
  while(!Serial);
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // static tflite::MicroErrorReporter micro_error_reporter;  // NOLINT
  // error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_tof_detector_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.printf("Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.

  static tflite::AllOpsResolver resolver;
  // static tflite::MicroMutableOpResolver<10> micro_mutable_op_resolver;  // NOLINT

  // micro_mutable_op_resolver.AddDepthwiseConv2D();
  // micro_mutable_op_resolver.AddFullyConnected();
  // micro_mutable_op_resolver.AddConv2D();
  // micro_mutable_op_resolver.AddMaxPool2D();
  // micro_mutable_op_resolver.AddSoftmax();
  // micro_mutable_op_resolver.AddMul();
  // micro_mutable_op_resolver.AddAdd();
  // micro_mutable_op_resolver.AddMean();
  // micro_mutable_op_resolver.AddExpandDims();
  // micro_mutable_op_resolver.AddBuiltin(
  //     tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
  //     tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  // micro_mutable_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
  //                              tflite::ops::micro::Register_MAX_POOL_2D());
  // micro_mutable_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
  //                              tflite::ops::micro::Register_CONV_2D());
  // micro_mutable_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
  //                              tflite::ops::micro::Register_FULLY_CONNECTED());
  // micro_mutable_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
  //                              tflite::ops::micro::Register_SOFTMAX());
  // micro_mutable_op_resolver.AddBuiltin(tflite::BuiltinOperator_MUL,
  //                              tflite::ops::micro::Register_MUL());
  // micro_mutable_op_resolver.AddBuiltin(tflite::BuiltinOperator_ADD,
  //                              tflite::ops::micro::Register_ADD());
  // micro_mutable_op_resolver.AddBuiltin(tflite::BuiltinOperator_MEAN,
  //                              tflite::ops::micro::Register_MEAN());
  


  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  interpreter->AllocateTensors();

  // Obtain pointer to the model's input tensor.
  model_input = interpreter->input(0);
  // Serial.println("Some model stats");
  // Serial.println(model_input->dims->size); //3
  // Serial.println(model_input->dims->data[0]); //1
  // Serial.println(model_input->dims->data[1]); //20
  // Serial.println(model_input->dims->data[2]); //64
  // Serial.println(model_input->type); //1
  // Serial.println(kTfLiteFloat32); //1
  // Serial.println(kChannelNumber); //9
  if ((model_input->dims->size != 3) || (model_input->dims->data[0] != 1) ||  // 3 1
      (model_input->dims->data[1] != 20) ||                                  // 384
      (model_input->dims->data[2] != kChannelNumber) ||                       // 9
      (model_input->type != kTfLiteFloat32)) {                                // 1
    Serial.println(model_input->dims->size);
    Serial.println(model_input->dims->data[0]);
    Serial.println(model_input->dims->data[1]);
    Serial.println(model_input->dims->data[2]);
    Serial.println(model_input->type);
    Serial.println("Bad input tensor parameters in model");
    return;
  }

  input_length = model_input->bytes / sizeof(float);
  Serial.printf("input_length: %i \n", input_length);

  bool setup_status = SetupVL53L8CX();
  if (!setup_status) {
    Serial.println("Set up failed\n");
  }
}

// TODO: should be true if x amount of pixels in the current frame are closer than 150cm. Then wait 5 frames or so
bool IsSeeing() {
  int time = millis()/1000;
  if (time % 20 == 0) {
    return true;
  } else {
    return false;
  }
}

// This is the regular function we run to recognize Maneuvers from a pretrained
// model.
void RecognizeManeuvers() {
  const bool is_seeing = IsSeeing();
  if(is_seeing) {
    // Run inference, and report any error.
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      Serial.printf("Invoke failed on index: %d\n",
                            begin_index);
      return;
    }

    const float* prediction_scores = interpreter->output(0)->data.f;

    Serial.println(*prediction_scores);
  }
}

void loop() {
  unsigned long startTime = millis(); // Get the current time in milliseconds
 
  // Attempt to read new data from the VL53L8CX.
  bool got_data =
      ReadVL53L8CX(model_input->data.f, input_length, false);
  // If there was no new data, wait until next time.
  if (!got_data) return;

  RecognizeManeuvers();
}
