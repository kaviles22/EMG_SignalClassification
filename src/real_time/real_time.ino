#define ANGLE_MIN 0
#define ANGLE_MAX 0

#define OPEN_ANGLE  0
#define CLOSE_ANGLE 20

#define TURN_ANGLE1 0
#define TURN_ANGLE2 60
#define TURN_ANGLE3 120

/*Libraries*/
//Tensorflow model converted to C++
#include "modeldata.h"
//functions
#include "functions.h"

#include "servo.h"

//Tensorflow custom library for ESP32
#include <TensorFlowLite_ESP32.h>
//experimental versions of regular Tensorflow headers
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"


// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;
int last_angle = TURN_ANGLE1;

// Create an area of memory to use for input, output, and intermediate arrays.
// Finding the minimum value for your model may require some trial and error.
constexpr int kTensorArenaSize = 15 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

int this_predict = -1;
int last_predict = -1;
uint8_t turning_angle = 0;
/*Functions*/

void setup() {

  
  Serial.begin(115200);
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(prosthetic_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::ops::micro::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);

  // Keep track of how many inferences we have performed.
  inference_count = 0;
  setup_servo();

}



void loop() {
  
  //{0.179, 0.551, 0.897, 0.565, 0.125, 0.601, 0.659, 0.581, 0.167, 0.575, 0.787, 0.712};
  //{0.125, 0.799, 0.475, 0.269, 0.052, 0.797, 0.741, 0.641, 0.440, 0.320, 0.249, 0.232};
  float data1[12] = {0.01748063255033152,0.6471331786528353,0.773371718450464,0.5495089664613995,0.07838395424159811,0.5914132305547034,0.5777429837032254,0.3967241023854958,0.07118843288668772,0.3587160835843334,0.6469245447925207,0.4376814438882512};
  input->data.f = prelim_collection();

  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed on x_val");
    return;
  }
  /*Serial.print("INPUT    DATA: ");
  for(int i = 0; i<12;i++){
     Serial.print(input->data.f[i]);
     Serial.print(",");
   }
   Serial.println(" ");
    Serial.println("************************************************");*/
    Serial.println("----------------------------------");
    Serial.print("MODEL OUTPUT: Open:");
    Serial.print(output->data.f[0]);
    Serial.print(", Turn: ");
    Serial.print(output->data.f[1]);
    Serial.print(", Close: ");
    Serial.print(output->data.f[2]);
    Serial.print(", Rest: ");
    Serial.println(output->data.f[3]);

    for (int i = 0; i < 3; i++) {
    if (output->data.f[i] > 0.8) this_predict = i;
    }
    //If the new prediction is the same as the previous one servos should not move.
    if(this_predict == last_predict){
      this_predict = -1;
    }
    switch(this_predict){
      case(0):
        //CERRAR MANO
        set_servo_mov(CLOSE_ANGLE);
          Serial.println("CLOSING HAND");
        /*if (get_servo_mov() != CLOSE_ANGLE){
          set_servo_mov(CLOSE_ANGLE);
          Serial.println("CLOSING HAND");
        }*/
        break;
      case(1):
        Serial.println("TURNING HAND");
        switch(get_servo_turn()){
          case(TURN_ANGLE1):
            last_angle = get_servo_turn();
            set_servo_turn(TURN_ANGLE2);
            Serial.print("Last angle: ");
            Serial.print(last_angle);
            Serial.print(", ");
            Serial.print("Moving to: ");
            Serial.println(TURN_ANGLE2);
            break;
          case(TURN_ANGLE2):
            if (last_angle == TURN_ANGLE1){
              set_servo_turn(TURN_ANGLE3);
              Serial.print("Last angle: ");
              Serial.print(last_angle);
              Serial.print(", ");
              Serial.print("Moving to: ");
              Serial.println(TURN_ANGLE3);
            } else if (last_angle == TURN_ANGLE3){
              set_servo_turn(TURN_ANGLE1);
              Serial.print("Last angle: ");
              Serial.print(last_angle);
              Serial.print(", ");
              Serial.print("Moving to: ");
              Serial.println(TURN_ANGLE1);
            }
            break;
          case(TURN_ANGLE3):
            last_angle = get_servo_turn();
            set_servo_turn(TURN_ANGLE2);
            Serial.print("Last angle: ");
            Serial.print(last_angle);
            Serial.print(", ");
            Serial.print("Moving to: ");
            Serial.println(TURN_ANGLE2);
            break;
        }
        break;
        case(2):
        //ABRIR MANO
        set_servo_mov(OPEN_ANGLE);
         Serial.println("OPENING HAND");
        /*if (get_servo_mov() != OPEN_ANGLE){
          set_servo_mov(OPEN_ANGLE);
          Serial.println("OPENING HAND"); 
        }*/
        break;
      }
   this_predict = -1;
  // Output array = [class0, class1, class2] --> Class0: musculo adentro, Class1: musculo afuera, Class2: descanso
  float y_val = output->data.f[0];
  inference_count += 1;
  if (inference_count >= 5000) inference_count = 0;

}
