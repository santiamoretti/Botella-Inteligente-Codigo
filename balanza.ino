#include "HX711.h"

const int DOUT = A1;
const int CLK = A0;
const int LED_PIN = 3; // Pin del LED

const float AGUA_DIARIA = 2.0; // Cantidad de agua diaria en kg
float aguaConsumida = 0.0;     // Cantidad de agua consumida
float taraAnterior = 0.0;      // Peso anterior
bool rellenando = false;       // Indicador de rellenado de botella

HX711 balanza;

void setup()
{
  Serial.begin(9600);
  balanza.begin(DOUT, CLK);

  pinMode(LED_PIN, OUTPUT);

  Serial.print("Lectura del valor del ADC: ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningún objeto sobre la balanza");
  Serial.println("Destarando...");
  Serial.println("...");
  balanza.set_scale(-439430.25);            // Establecemos la escala
  balanza.tare(20);                         // El peso actual es considerado Tara.
  taraAnterior = balanza.get_units(20);     // Guarda el peso inicial

  Serial.print("Peso de la tara: ");
  Serial.print(taraAnterior, 3);
  Serial.println(" kg");

  Serial.println("Listo para pesar");
}

void loop()
{
  if (Serial.available())
  {
    int inputData = Serial.parseInt(); // Lee el número ingresado desde el monitor serial

    if (inputData == 1)
    {
      float pesoActual = balanza.get_units(20);

      if (pesoActual <= 0 && !rellenando)
      {
        balanza.tare(20);      // Retarar la balanza para considerar el nuevo peso
        taraAnterior = 0.0;    // Reiniciar la tara anterior
        rellenando = true;     // Indica que se está rellenando la botella
      }
      else if (rellenando)
      {
        aguaConsumida += taraAnterior; // Agregar el peso anterior como consumo de agua
        rellenando = false;            // Reiniciar el indicador de rellenado
      }
      else
      {
        float diferenciaPeso = pesoActual - taraAnterior;
        aguaConsumida += diferenciaPeso; // Agregar el cambio de peso al agua consumida
      }

      taraAnterior = pesoActual; // Actualizar el peso anterior

      Serial.print("Peso de la tara: ");
      Serial.print(taraAnterior, 3);
      Serial.println(" kg");

      // Parpadeo del LED 3 veces
      for (int i = 0; i < 3; i++)
      {
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
        delay(500);
      }
    }
  }

  if (aguaConsumida >= AGUA_DIARIA)
  {                                 // Si se ha alcanzado la cantidad diaria de agua
    digitalWrite(LED_PIN, HIGH);   // Enciende el LED
  }

  Serial.print("Peso: ");
  Serial.print(balanza.get_units(20), 3);
  Serial.println(" kg");

  // Imprime la cantidad de agua consumida
  Serial.print("Agua consumida: ");
  Serial.print(aguaConsumida, 3);
  Serial.println(" kg");

  delay(500);
}
