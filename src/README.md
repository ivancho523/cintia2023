# Estimación de Temperatura utilizando TinyML y TensorFLowLite

Este es un ejemplo de uso de la librería [TensorFlowLite for Microcontrollers](https://www.tensorflow.org/lite/microcontrollers).

El proyecto consta de crear un modelo sencillo en Google Colab que pueda hacer estimacioines de una serir de datos que simulan el comportamiento de temperatura.

El modelo ha sido entrenado utilizando una red neuronal multicapa aprovechando la librería tensorflow y keras.


## Conversión del modelo de TensorFlow utilizando TensorFlow Lite

Para poder utilizar el modelo previamente entrenado en un microcontrolador debemos seguir los siguientes pasos:

- Instalar xxd
`!apt-get update && apt-get -qq install xxd`

- Convertir el modelo a un archivo C (se crea la carpeta tflite_models y se almacena el archivo en blanco model.tflite y model.cc)
`xxd -i tflite_models/model.tflite > model.cc`

- Guardamos el modelo en el archivo .tflite
`converter = tf.lite.TFLiteConverter.from_keras_model(model)`
`model_no_quant_tflite = converter.convert()`


### Comprobación del modelo de TFLite

* Para comprobar el funcionamiento del modelo; creamos un nuevo objeto que contenga el modelo .tflite
`tflite_model_file = '/content/model_good.tflite'`

* Creamos un intérprete y luego hacemos estimaciones
`interpreter = tf.lite.Interpreter(model_path=tflite_model_file)`


### Despliegue del modelo en el Microcontrolador ESP32

Una vez se disponga del modelo entrenado y su conversión a un archivo C, podemos integrar este archivo como el modelo que se utilizará para poder hacer las estimaciones.

Para ello descargamos de la carpeta de Google Colab el archivo [modelo_bueno.cpp] y copiamos el contenido en el archivo [model.cpp] ubicado en la carpeta `src`

Las librerías utilizadas en este ejemplo son:
[DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library)
[Adafruit_Unified_Sensor](https://github.com/adafruit/Adafruit_Sensor)
[Arduino_TensorFlowLite_ESP32](https://github.com/tanakamasayuki/Arduino_TensorFlowLite_ESP32)