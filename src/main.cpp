#include <Arduino.h>
#include <TensorFlowLite_ESP32.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

//#include "main_functions.h"
#include "model.h"
//#include "constants.h"
#include "output_handler.h"

#define DHTPIN 4
#define DHTTYPE DHT11

//Objeto de tipo DHT sobre el que llamamos los métodos de la librería
DHT dht(DHTPIN, DHTTYPE);

// Espacio Global.
namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  int inference_count = 0;

  constexpr int kTensorArenaSize = 2000;
  uint8_t tensor_arena[kTensorArenaSize];
} 


void setup() {
  Serial.begin(9600);
  dht.begin();

  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

 // leer y mapear el modelo
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                        "Model provided is schema version %d not equal "
                        "to supported version %d.",
                        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  static tflite::AllOpsResolver resolver;

  // Construir el intérprete para poder estimar.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Separar el espacio en memoria para los tensores.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // punteros del modelo
  input = interpreter->input(0);
  output = interpreter->output(0);
}


void loop() {
  // Leer la temperatura
  float temp = dht.readTemperature();

  // Pasar el parámetro de la temperatura al tensor
  input->data.f[0] = temp;

  // Hacer la estimación y reportar algún error
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Llamado al modelo fallido: %f\n", static_cast<double>(temp));
    return;
  }

  // Obtener la salida de la estimación
  float y_quantized = output->data.f[0];
  // Dequantize the output from integer to floating-point
  float y = (y_quantized - output->params.zero_point) * output->params.scale;

  // Esta puede ser otra forma de estimar la salida
  // HandleOutput(error_reporter, temp, y_quantized);

  // Mostrar el resultado en el puerto serial
  Serial.print("Leida: ");
  Serial.println(temp);
  Serial.print("Estimada: ");
  Serial.println(y_quantized);
  delay(1000);
}